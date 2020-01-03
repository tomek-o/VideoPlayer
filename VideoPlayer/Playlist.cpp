//---------------------------------------------------------------------------


#pragma hdrstop

#include "Playlist.h"
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

	position = jPlaylist.get("position", position).asInt();
	if (position >= static_cast<int>(entries.size()))
	{
		position = -1;
		if (!entries.empty())
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

void Playlist::add(AnsiString fileName)
{
	std::vector<PlaylistEntry>::const_iterator iter;
#if 0
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
	PlaylistEntry newEntry;
	newEntry.fileName = fileName;
    newEntry.size = getFileSize(fileName.c_str());

	entries.push_back(newEntry);
}

