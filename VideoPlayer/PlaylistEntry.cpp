//---------------------------------------------------------------------------


#pragma hdrstop

#include "PlaylistEntry.h"
#include <json/json.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)

bool PlaylistEntry::isValid(void) const
{
	if (fileName == "")
		return false;
	return true;
}

void PlaylistEntry::fromJson(const Json::Value &jv)
{
	jv.getAString("fileName", fileName);
	size = jv.get("size", size).asUInt64();
	jv.getAString("timeStamp", timeStamp);
	jv.getBool("mark", mark);
	jv.getDouble("length", length);
	jv.getDouble("playbackProgress", playbackProgress);
	jv.getInt("bitrateVideo", bitrateVideo);
	jv.getInt("bitrateAudio", bitrateAudio);
	jv.getAString("mplayerExtraParams", mplayerExtraParams);
	jv.getInt("softVol", softVolLevel);
}

void PlaylistEntry::toJson(Json::Value &jv) const
{
	jv["fileName"] = fileName;
	jv["size"] = size;
	jv["timeStamp"] = timeStamp;
	jv["mark"] = mark;
	jv["length"] = length;
	jv["playbackProgress"] = playbackProgress;
	if (bitrateVideo != BITRATE_DEFAULT)
	{
		jv["bitrateVideo"] = bitrateVideo;
	}
	if (bitrateAudio != BITRATE_DEFAULT)
	{
		jv["bitrateAudio"] = bitrateAudio;
	}
	if (mplayerExtraParams != "")
	{
		jv["mplayerExtraParams"] = mplayerExtraParams;
	}
	if (softVolLevel != SOFTVOL_LEVEL_DEFAULT)
	{
		jv["softVol"] = softVolLevel;
	}
}


