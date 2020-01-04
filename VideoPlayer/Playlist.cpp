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

LONGLONG getFileSize(char* fileName) {
	/* Gets the size of the file passed in fileName */
	HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ , NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return 0;
	}

	LARGE_INTEGER size;
	if (!GetFileSizeEx(hFile, &size)) {
		CloseHandle(hFile);
		return -1; // error condition, could call GetLastError to find out more
	}
	CloseHandle(hFile);
	return size.QuadPart;
}

}

Playlist::Playlist(void):
	position(-1)
{

}

int Playlist::loadFromFile(AnsiString fileName)
{
	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;

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
		for (unsigned int i=0; i<jEntries.size(); i++)
		{
			const Json::Value& jv = jEntries[i];
			PlaylistEntry entry;
			jv.getAString("fileName", entry.fileName);
			entry.size = jv.get("size", entry.size).asUInt64();
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
		newEntry.size = getFileSize(fileName.c_str());

		entries.push_back(newEntry);
	}
	filter(filterText);
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
		}
	}

	entries = newEntries;
	filter(filterText);
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
	return 0;
}

void Playlist::filter(AnsiString text)
{
	filteredEntries.clear();
	filterText = text;
	std::vector<PlaylistEntry>::const_iterator iter;
	bool empty = (text == "");
	AnsiString needle = UpperCase(filterText);
	for (unsigned int i=0; i<entries.size(); i++)
	{
		const PlaylistEntry& entry = entries[i];
		if (empty || UpperCase(entry.fileName).Pos(needle) > 0)
		{
			FilteredPlaylistEntry fpe;
			fpe.entry = entry;
			fpe.id = i;
			filteredEntries.push_back(fpe);
		}
	}
}


