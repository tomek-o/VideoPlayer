//---------------------------------------------------------------------------

#ifndef PlaylistEntryH
#define PlaylistEntryH
//---------------------------------------------------------------------------

#include <System.hpp>
#include <stdint.h>

namespace Json
{
	class Value;
}

struct PlaylistEntry
{
	AnsiString fileName;
	uint64_t size;
	AnsiString timeStamp;
	bool mark;
	double length;			// value < 0 if not checked; value = 0 if not recognized by mplayer
	double playbackProgress;// storing highest playback position in relation to file time
    enum { BITRATE_DEFAULT = -1 };
	int bitrateVideo;
	int bitrateAudio;
	int softVolLevel;
	enum { SOFTVOL_LEVEL_DEFAULT = 50 };
	AnsiString mplayerExtraParams;
	PlaylistEntry(void):
		size(0),
		mark(false),
		length(-1.0),
		playbackProgress(0.0),
		bitrateVideo(BITRATE_DEFAULT),
		bitrateAudio(BITRATE_DEFAULT),
		softVolLevel(SOFTVOL_LEVEL_DEFAULT)
	{}
	bool isValid(void) const;
	void fromJson(const Json::Value &jv);
	void toJson(Json::Value &jv) const;
};

#endif
