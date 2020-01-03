//---------------------------------------------------------------------------


#pragma hdrstop

#include "PlaylistEntry.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

bool PlaylistEntry::isValid(void) const
{
	if (fileName == "")
		return false;
	return true;
}

