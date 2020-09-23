//---------------------------------------------------------------------------


#pragma hdrstop

/** See MPlayer slave mode manual: http://www.mplayerhq.hu/DOCS/tech/slave.txt
*/

#include "Mplayer.h"
#include "ThreadMplayerWait.h"
#include "ThreadMplayerControl.h"
#include <SysUtils.hpp>
#include <math.h>
#include <stdio.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)

MPlayer mplayer;

MPlayer::MPlayer():
	hMPlayer(NULL),
	hPipeRead(NULL),
	hPipeWrite(NULL),
	callbackStopPlaying(NULL),
	callbackMediaInfoUpdate(NULL),
	mplayerWaitThread(NULL),
	mplayerControlThread(NULL)
{
	cfg.parent = INVALID_HANDLE_VALUE;
	cfg.colorkey = clBlack;
}

int MPlayer::Configure(const Cfg& cfg)
{
	this->cfg = cfg;
	if (!FileExists(cfg.asInstance)) {
		return -1;
	}
	return 0;
}

MPlayer::~MPlayer()
{
}

int MPlayer::run(AnsiString cmdLine)
{
	HANDLE DummyPipe1, DummyPipe2;

	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = true;

	if (!CreatePipe(&hPipeRead, &DummyPipe1, &sa, 0)) return false;

	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = true;
	if (!CreatePipe(&DummyPipe2, &hPipeWrite, &sa, 0)) {
		CloseHandle(hPipeRead);
		CloseHandle(DummyPipe1);
		return -1;
	}

	STARTUPINFO si;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = DummyPipe2;
	si.hStdOutput = DummyPipe1;
	si.hStdError = DummyPipe1;

	// create process

	PROCESS_INFORMATION pi;

	if (!CreateProcess(NULL, cmdLine.c_str(), NULL, NULL, true, DETACHED_PROCESS | ABOVE_NORMAL_PRIORITY_CLASS,
	NULL, NULL, &si, &pi)) {
		CloseHandle(hPipeRead);
		CloseHandle(hPipeWrite);
		CloseHandle(DummyPipe1);
		CloseHandle(DummyPipe2);
		return -2;
	}

	CloseHandle(DummyPipe1);
	CloseHandle(DummyPipe2);

	hMPlayer = pi.hProcess;

	mplayerWaitThread = new ThreadMplayerWait(hMPlayer, &OnStopPlayingFn);
	mplayerControlThread = new ThreadMplayerControl(hPipeRead);
	mplayerControlThread->onConsoleLineRx = OnConsoleLineReceived;

	return 0;
}

void MPlayer::OnStopPlayingFn(void)
{
	hMPlayer = NULL;
	CloseHandle(hPipeRead);
	hPipeRead = NULL;
	CloseHandle(hPipeWrite);
	hPipeWrite = NULL;
	mplayerWaitThread = NULL;
	if (mplayerControlThread) {
		mplayerControlThread->Terminate();
		mplayerControlThread = NULL;	
	}
	if (callbackStopPlaying)
		callbackStopPlaying();
}

int MPlayer::play(AnsiString filename)
{
	AnsiString cmdLine;
	this->filename = filename;

	mediaInfo.videoBitrate = mediaInfo.audioBitrateKnown = false;
	if (this->callbackMediaInfoUpdate)
	{
    	callbackMediaInfoUpdate();
	}

	cmdLine.cat_printf("\"%s\" ", cfg.asInstance.c_str());
	cmdLine.cat_printf("-slave -identify -colorkey 0x%x -quiet -keepaspect ", cfg.colorkey);
	if (cfg.softVol)
	{
		int db;
		if (cfg.softVolLevel <= 0)
		{
			db = -120;
		}
		else
		{
			db = (int)(20.0f * log10((float)cfg.softVolLevel*cfg.softVolMax/100.0f/100.0f));
		}
		cmdLine.cat_printf("-softvol -af volume=%d -softvol-max %u ", db, cfg.softVolMax);
	}
	//cmdLine.cat_printf("-framedrop -nofs -noterm-osd -idx -hr-mp3-seek -nobps -ni ");
	cmdLine.cat_printf("-wid %d \"%s\" ", (int)cfg.parent, filename.c_str());
	int status = run(cmdLine);
	if (status)
	{
		if (callbackStopPlaying)
			callbackStopPlaying();
	}
	return status;
}

int MPlayer::pause(void)
{
	if ((hMPlayer == NULL) || (hPipeWrite == NULL))
		return -1;
	return sendCommand("pause");
}

int MPlayer::frame_step(void)
{
	if ((hMPlayer == NULL) || (hPipeWrite == NULL))
		return -1;
	return sendCommand("frame_step");
}

int MPlayer::seekRelative(int seconds)
{
	if ((hMPlayer == NULL) || (hPipeWrite == NULL))
		return -1;
	AnsiString msg;
	msg.sprintf("seek %d", seconds);
	return sendCommand(msg);
}

int MPlayer::toggleOsd(void)
{
	return sendCommand("osd");
}

int MPlayer::setOsdLevel(int level)
{
	if ((hMPlayer == NULL) || (hPipeWrite == NULL))
		return -1;
	AnsiString msg;
	msg.sprintf("osd %d", level);
	return sendCommand(msg);
}

int MPlayer::changeVolume(int delta)
{
	AnsiString msg;
	msg.sprintf("volume %d", delta);
	return sendCommand(msg);
}

int MPlayer::changeVolumeAbs(int val)
{
	AnsiString msg;
	cfg.softVolLevel = val;
	msg.sprintf("volume %d 1", val);
	return sendCommand(msg);
}

int MPlayer::osdShowText(AnsiString text, int duration)
{
	AnsiString msg;
	if (text.Pos("'") != 0)
	{
		// not sure how to escape this...
		return -1;
	}
	msg.sprintf("osd_show_text '%s' %d", text.c_str(), duration);
	return sendCommand(msg);
}

int MPlayer::stop(bool useCallback)
{
	if ((hMPlayer == NULL) || (hPipeWrite == NULL))
		return -1;
	if (mplayerWaitThread && (useCallback == false)) {
		mplayerWaitThread->useCallback = false;
	}
	sendCommand("quit");
	if (mplayerControlThread) {
		mplayerControlThread->Terminate();
		mplayerControlThread = NULL;
	}
	if (mplayerWaitThread) {
		mplayerWaitThread->Terminate();
		mplayerWaitThread = NULL;
	}
	return 0;
}

int MPlayer::sendCommand(AnsiString cmd)
{
	if ((hMPlayer == NULL) || (hPipeWrite == NULL))
		return -1;
	cmd += '\n';
	unsigned long dummy;
	bool status = WriteFile(hPipeWrite, cmd.c_str(), cmd.Length(), &dummy, NULL);
	if (status == false)
		return -2;
	return 0;
}

void MPlayer::OnConsoleLineReceived(AnsiString line)
{
	int bitrate;
	line = line.Trim();

	if (line.Pos("ID_VIDEO_BITRATE=") == 1)
	{
		if (sscanf(line.c_str(), "ID_VIDEO_BITRATE=%d", &bitrate) == 1)
		{
			mediaInfo.videoBitrateKnown = true;
			mediaInfo.videoBitrate = bitrate;
			if (this->callbackMediaInfoUpdate)
			{
				callbackMediaInfoUpdate();
			}
		}
	}

	if (line.Pos("ID_AUDIO_BITRATE=") == 1)
	{
		if(sscanf(line.c_str(), "ID_AUDIO_BITRATE=%d", &bitrate) == 1)
		{
			mediaInfo.audioBitrateKnown = true;
			mediaInfo.audioBitrate = bitrate;
			if (this->callbackMediaInfoUpdate)
			{
				callbackMediaInfoUpdate();
			}
		}
	}
}

