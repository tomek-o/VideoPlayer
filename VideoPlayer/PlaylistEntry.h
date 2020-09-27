//---------------------------------------------------------------------------

#ifndef PlaylistEntryH
#define PlaylistEntryH
//---------------------------------------------------------------------------

#include <System.hpp>
#include <stdint.h>

struct PlaylistEntry
{
	AnsiString fileName;
	uint64_t size;
	AnsiString timeStamp;
	bool mark;
	double length;			// value < 0 if not checked; value = 0 if not recognized by mplayer
	PlaylistEntry(void):
		size(0),
		mark(false),
		length(-1.0)
	{}
	bool isValid(void) const;
};

#endif
