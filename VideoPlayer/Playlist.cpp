//---------------------------------------------------------------------------


#pragma hdrstop

#include "Playlist.h"
#include "Log.h"
#include <algorithm>
#include <fstream>
#include <json/json.h>

#include <SysUtils.hpp>

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace
{

int getFileDetails(char* fileName, uint64_t& usize, AnsiString& timeStamp) {
	usize = 0;
	timeStamp = "";

	/* Gets the size of the file passed in fileName */
	HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ , NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return -1;
	}

	LARGE_INTEGER size;
	if (!GetFileSizeEx(hFile, &size)) {
		LOG("Failed to read file size for %s", fileName);
	} else {
		usize = size.QuadPart;
	}

	FILETIME ftCreate, ftAccess, ftWrite;
	if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
	{
		LOG("Failed to retrieve time stamps for %s\n", fileName);
	}
	else
	{
		SYSTEMTIME st;
		FileTimeToLocalFileTime( &ftWrite, &ftWrite );
		FileTimeToSystemTime( &ftWrite, &st );
		timeStamp.sprintf("%04d.%02d.%02d %02d:%02d:%02d",
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond
			);
	}

	CloseHandle(hFile);
	return 0;
}

}

Playlist::Playlist(void):
	position(-1),
	modified(false)
{

}

int Playlist::loadFromFile(AnsiString fileName)
{
	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;

    LOG("Loading %s...", fileName.c_str());

	try
	{
		std::ifstream ifs(fileName.c_str());
		std::string strConfig((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		ifs.close();
		bool parsingSuccessful = reader.parse( strConfig, root );
		if ( !parsingSuccessful )
		{
			return 2;
		}
	}
	catch(...)
	{
		return 1;
	}

	const Json::Value& jPlaylist = root["playlist"];

	const Json::Value& jEntries = jPlaylist["entries"];
	entries.clear();
	if (jEntries.type() == Json::arrayValue)
	{
        LOG("    %u entries", jEntries.size());
		for (unsigned int i=0; i<jEntries.size(); i++)
		{
			const Json::Value& jv = jEntries[i];
			PlaylistEntry entry;
			jv.getAString("fileName", entry.fileName);
			entry.size = jv.get("size", entry.size).asUInt64();
			jv.getAString("timeStamp", entry.timeStamp);
			jv.getBool("mark", entry.mark);
			jv.getDouble("length", entry.length);
			if (entry.isValid())
			{
            	entries.push_back(entry);
			}
		}
	}

	jPlaylist.getAString("filterText", filterText);

	filter(filterText);

	position = jPlaylist.get("position", position).asInt();
	if (position >= static_cast<int>(filteredEntries.size()))
	{
		position = -1;
		if (!filteredEntries.empty())
			position = 0;
	}

	return 0;
}

int Playlist::saveToFile(AnsiString fileName)
{
	if (!modified)
	{
		LOG("Skipping saving %s (not modified)", fileName.c_str());
		return 0;
	}

	Json::Value root;
	Json::StyledWriter writer("\t");

	Json::Value &jPlaylist = root["playlist"];

	Json::Value &jEntries = jPlaylist["entries"];
	jEntries.resize(0);
	for (unsigned int i=0; i<entries.size(); i++)
	{
		Json::Value &jEntry = jEntries.append(Json::Value(Json::objectValue));
		const PlaylistEntry &entry = entries[i];
		jEntry["fileName"] = entry.fileName;
		jEntry["size"] = entry.size;
		jEntry["timeStamp"] = entry.timeStamp;
		jEntry["mark"] = entry.mark;
		jEntry["length"] = entry.length;
	}

	jPlaylist["position"] = position;
	jPlaylist["filterText"] = filterText;

	std::string outputConfig = writer.write( root );

	try
	{
		std::ofstream ofs(fileName.c_str());
		ofs << outputConfig;
		ofs.close();
	}
	catch(...)
	{
		return 1;
	}

	return 0;
}

void Playlist::setPosition(int position) {
	if (this->position != position)
	{
		this->position = position;
		modified = true;
	}
}

void Playlist::add(const std::vector<AnsiString>& fileNames)
{
#if 0
	std::vector<PlaylistEntry>::const_iterator iter;
	// skipping duplicates
	for (iter = entries.begin(); iter != entries.end(); ++iter)
	{
		const PlaylistEntry& entry = *iter;
		if (entry.fileName == fileName)
		{
			return;
		}
	}
#endif
	for (unsigned int i=0; i<fileNames.size(); i++)
	{
		PlaylistEntry newEntry;
		const AnsiString& fileName = fileNames[i];
		newEntry.fileName = fileName;
		getFileDetails(fileName.c_str(), newEntry.size, newEntry.timeStamp);

		entries.push_back(newEntry);
	}
	filter(filterText);
	modified = true;
}

void Playlist::remove(const std::set<unsigned int>& ids)
{
	std::vector<PlaylistEntry> newEntries;

	for (unsigned int i=0; i<entries.size(); i++)
	{
		if (ids.find(i) == ids.end())
		{
			newEntries.push_back(entries[i]);
		}
	}

	entries = newEntries;
	filter(filterText);
	modified = true;
}

void Playlist::removeWithFiles(const std::set<unsigned int>& ids)
{
	std::vector<PlaylistEntry> newEntries;

	for (unsigned int i=0; i<entries.size(); i++)
	{
		PlaylistEntry& entry = entries[i];
		if (ids.find(i) == ids.end())
		{
			newEntries.push_back(entry);
		}
		else
		{
			if (DeleteFile(entry.fileName) == false)
			{
				LOG("Failed to delete %s", entry.fileName.c_str());
				newEntries.push_back(entry);
			}
			else
			{
            	LOG("Deleted %s", entry.fileName.c_str());
			}
		}
	}

	entries = newEntries;
	filter(filterText);
	modified = true;
}

void Playlist::removeDuplicates(void)
{
	std::set<AnsiString> filenames;
	std::vector<PlaylistEntry> newEntries;
	for (unsigned int i=0; i<entries.size(); i++)
	{
		const PlaylistEntry& entry = entries[i];
		if (filenames.find(entry.fileName) == filenames.end())
		{
			filenames.insert(entry.fileName);
			newEntries.push_back(entry);
		}
	}
	entries = newEntries;
	filter(filterText);
	modified = true;
}

void Playlist::removeMissingFiles(void)
{
	std::vector<PlaylistEntry> newEntries;
	for (unsigned int i=0; i<entries.size(); i++)
	{
		const PlaylistEntry& entry = entries[i];
		if (FileExists(entry.fileName))
		{
			newEntries.push_back(entry);
		}
	}
	entries = newEntries;
	filter(filterText);
	modified = true;
}

int Playlist::rename(unsigned int id, AnsiString newFileName)
{
	PlaylistEntry& entry = entries[id];
	if (RenameFile(entry.fileName, newFileName) == false)
	{
		return -1;
	}
	entry.fileName = newFileName;
	filter(filterText);
	modified = true;
	return 0;
}

int Playlist::findByName(AnsiString fileName)
{
	for (unsigned int i=0; i<entries.size(); i++)
	{
		if (entries[i].fileName == fileName)
			return i;
	}
	return -1;
}

void Playlist::filter(AnsiString text)
{
	if (filterText != text)
	{
		modified = true;
	}
	filteredEntries.clear();
	filterText = text;
	std::vector<PlaylistEntry>::const_iterator iter;
	bool empty = (text == "");
	AnsiString needle = UpperCase(filterText);
	for (unsigned int i=0; i<entries.size(); i++)
	{
		const PlaylistEntry& entry = entries[i];
		if (empty || UpperCase(ExtractFileName(entry.fileName)).Pos(needle) > 0)
		{
			FilteredPlaylistEntry fpe;
			fpe.entry = entry;
			fpe.id = i;
			filteredEntries.push_back(fpe);
		}
	}
}

namespace
{

bool compareFileNameAsc(const PlaylistEntry& e1, const PlaylistEntry& e2)
{
	return UpperCase(e1.fileName).AnsiCompare(UpperCase(e2.fileName)) > 0;
}

bool compareFileNameDesc(const PlaylistEntry& e1, const PlaylistEntry& e2)
{
	return UpperCase(e1.fileName).AnsiCompare(UpperCase(e2.fileName)) < 0;
}

bool compareSizeAsc(const PlaylistEntry& e1, const PlaylistEntry& e2)
{
	return e1.size > e2.size;
}

bool compareSizeDesc(const PlaylistEntry& e1, const PlaylistEntry& e2)
{
	return e1.size < e2.size;
}

bool compareTimeStampAsc(const PlaylistEntry& e1, const PlaylistEntry& e2)
{
	return UpperCase(e1.timeStamp).AnsiCompare(UpperCase(e2.timeStamp)) > 0;
}

bool compareTimeStampDesc(const PlaylistEntry& e1, const PlaylistEntry& e2)
{
	return UpperCase(e1.timeStamp).AnsiCompare(UpperCase(e2.timeStamp)) < 0;
}

bool compareMarkAsc(const PlaylistEntry& e1, const PlaylistEntry& e2)
{
	return e1.mark > e2.mark;
}

bool compareMarkDesc(const PlaylistEntry& e1, const PlaylistEntry& e2)
{
	return e1.mark < e2.mark;
}

bool compareLengthAsc(const PlaylistEntry& e1, const PlaylistEntry& e2)
{
	return e1.length > e2.length;
}

bool compareLengthDesc(const PlaylistEntry& e1, const PlaylistEntry& e2)
{
	return e1.length < e2.length;
}


}

int Playlist::sort(enum SortType type, bool ascending)
{
#pragma warn -8091	// incorrectly issued by BDS2006

	switch(type)
	{
	case SortRandom:
		std::random_shuffle(entries.begin(), entries.end());
		break;
	case SortByFileName:
		if (ascending)
			std::stable_sort(entries.begin(), entries.end(), compareFileNameAsc);
		else
			std::stable_sort(entries.begin(), entries.end(), compareFileNameDesc);
		break;
	case SortBySize:
		if (ascending)
			std::stable_sort(entries.begin(), entries.end(), compareSizeAsc);
		else
			std::stable_sort(entries.begin(), entries.end(), compareSizeDesc);
		break;
	case SortByTimeStamp:
		if (ascending)
			std::stable_sort(entries.begin(), entries.end(), compareTimeStampAsc);
		else
			std::stable_sort(entries.begin(), entries.end(), compareTimeStampDesc);
		break;
	case SortByMark:
		if (ascending)
			std::stable_sort(entries.begin(), entries.end(), compareMarkAsc);
		else
			std::stable_sort(entries.begin(), entries.end(), compareMarkDesc);
		break;
	case SortByLength:
		if (ascending)
			std::stable_sort(entries.begin(), entries.end(), compareLengthAsc);
		else
			std::stable_sort(entries.begin(), entries.end(), compareLengthDesc);
		break;
	default:
		return -1;
	}
	filter(filterText);
	modified = true;
	return 0;
}

void Playlist::markSet(unsigned int id)
{
	PlaylistEntry& entry = entries[id];
	entry.mark = true;
	filter(filterText);
	modified = true;
}

void Playlist::markClear(unsigned int id)
{
	PlaylistEntry& entry = entries[id];
	entry.mark = false;
	filter(filterText);	
	modified = true;
}

void Playlist::markDuplicatesBySize(void)
{
	std::map<uint64_t, unsigned int> sizes;
	for (unsigned int i=0; i<entries.size(); i++)
	{
		PlaylistEntry& entry = entries[i];
		std::map<uint64_t, unsigned int>::iterator iter = sizes.find(entry.size);
		if (iter != sizes.end())
		{
			entry.mark = true;
			PlaylistEntry& prevEntry = entries[iter->second];
			prevEntry.mark = true;
			modified = true;
		}
		else
		{
			sizes[entry.size] = i;
		}
	}
	if (modified)
	{
		filter(filterText);
	}
}

void Playlist::setLength(unsigned int id, double length)
{
	PlaylistEntry& entry = entries[id];
	if (entry.length != length)
	{
		entry.length = length;
		filter(filterText);
		modified = true;
	}
}



