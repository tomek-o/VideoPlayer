//---------------------------------------------------------------------------

#ifndef PlaylistH
#define PlaylistH
//---------------------------------------------------------------------------

#include "PlaylistEntry.h"
#include <System.hpp>
#include <vector>
#include <set>

struct FilteredPlaylistEntry
{
	unsigned int id;
	PlaylistEntry entry;
	FilteredPlaylistEntry(void):
		id(0xFFFFFFFF)
	{}
};

class Playlist
{
private:
	int position;
	std::vector<PlaylistEntry> entries;
	std::vector<FilteredPlaylistEntry> filteredEntries;
	AnsiString filterText;
public:
	Playlist(void);
	int loadFromFile(AnsiString fileName);
	int saveToFile(AnsiString fileName);
	const std::vector<FilteredPlaylistEntry>& getFilteredEntries(void) const {
    	return filteredEntries;
	}
	int getPosition(void) const {
		return position;
	}
	void setPosition(int position) {
    	this->position = position;
	}
	AnsiString getFilterText(void) const {
    	return filterText;
	}
	void add(const std::vector<AnsiString>& fileNames);
	void remove(const std::set<unsigned int>& ids);
	void filter(AnsiString text);
};

#endif
