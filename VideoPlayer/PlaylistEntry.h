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
	PlaylistEntry(void):
		size(0),
		mark(false)
	{}
	bool isValid(void) const;
};

#endif
