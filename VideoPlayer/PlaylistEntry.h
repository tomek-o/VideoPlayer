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
	PlaylistEntry(void):
		size(0)
	{}
	bool isValid(void) const;
};

#endif
