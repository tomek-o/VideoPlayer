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
	bool modified;
	std::vector<PlaylistEntry> entries;
	std::vector<FilteredPlaylistEntry> filteredEntries;
	AnsiString filterText;
public:
	Playlist(void);
	int loadFromFile(AnsiString fileName);
	int saveToFile(AnsiString fileName);
	const std::vector<PlaylistEntry>& getEntries(void) const {
    	return entries;
	}
	const std::vector<FilteredPlaylistEntry>& getFilteredEntries(void) const {
    	return filteredEntries;
	}
	int getPosition(void) const {
		return position;
	}
	void setPosition(int position);
	AnsiString getFilterText(void) const {
    	return filterText;
	}
	const PlaylistEntry& getEntry(unsigned int id) {
    	return entries[id];
	}
	void add(const std::vector<AnsiString>& fileNames);
	void remove(const std::set<unsigned int>& ids);
	int rename(unsigned int id, AnsiString newFileName);
	int findByName(AnsiString fileName);
	void removeWithFiles(const std::set<unsigned int>& ids);
	void filter(AnsiString text);
	void removeDuplicates(void);
	void removeMissingFiles(void);

	enum SortType {
		SortRandom = 0,
		SortByFileName,
		SortBySize,
		SortByTimeStamp,

		SortTypeLimiter
	};
	int sort(enum SortType type, bool ascending);
};

#endif
