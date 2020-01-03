//---------------------------------------------------------------------------

#ifndef PlaylistH
#define PlaylistH
//---------------------------------------------------------------------------

#include "PlaylistEntry.h"
#include <System.hpp>
#include <vector>


class Playlist
{
private:
	int position;
	std::vector<PlaylistEntry> entries;
public:
	Playlist(void);
	int loadFromFile(AnsiString fileName);
	int saveToFile(AnsiString fileName);
	const std::vector<PlaylistEntry>& getEntries(void) const {
    	return entries;
	}
	void setEntries(const std::vector<PlaylistEntry>& entries) {
    	this->entries = entries;
	}
	int getPosition(void) const {
		return position;
	}
	void setPosition(int position) {
    	this->position = position;
	}
	void add(AnsiString fileName);
};

#endif
