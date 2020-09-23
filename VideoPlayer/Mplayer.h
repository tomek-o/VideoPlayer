//---------------------------------------------------------------------------

#ifndef MplayerH
#define MplayerH
//---------------------------------------------------------------------------
#include <System.hpp>
#include <Graphics.hpp>

class ThreadMplayerWait;
class ThreadMplayerControl;

class MPlayer
{
private:
	HANDLE hMPlayer;
	HANDLE hPipeRead;
	HANDLE hPipeWrite;
//	TClientWaitThread *ClientWaitThread;
//	TControlThread *ControlThread;
	int process_priority;

//	TExecCallback callback;
	bool myPlayerExited;

	AnsiString filename;

	int run(AnsiString cmdLine);
	int sendCommand(AnsiString cmd);

	ThreadMplayerWait* mplayerWaitThread;
	ThreadMplayerControl* mplayerControlThread;
	typedef void (__closure *CallbackStopPlaying)(void);
	typedef void (__closure *CallbackMediaInfoUpdate)(void);

public:
	MPlayer();
	~MPlayer();
	struct Cfg
	{
		HANDLE parent;
		TColor colorkey;
		AnsiString asInstance;
		bool softVol;
		int softVolLevel;
		int softVolMax;
	};
	int Configure(const Cfg& cfg);
	const Cfg& getCfg(void)
	{
    	return cfg;
	}
	void __fastcall setCmdLine(AnsiString cmdline);
	void __fastcall lineReceived(AnsiString line);
	void __fastcall playerExited();

	int play(AnsiString filename);
	AnsiString getFilename(void) const
	{
    	return filename;
	}
	int frame_step(void);
	int pause(void);
	int seekRelative(int seconds);
	int toggleOsd(void);
	int setOsdLevel(int level);
	int changeVolume(int delta);
	int changeVolumeAbs(int val);
	int osdShowText(AnsiString text, int duration);
	int stop(bool useCallback = true);
	void kill();

	void OnStopPlayingFn(void);
	CallbackStopPlaying callbackStopPlaying;
	CallbackMediaInfoUpdate callbackMediaInfoUpdate;

	void OnConsoleLineReceived(AnsiString line);

	struct MediaInfo
	{
		bool videoBitrateKnown;
		int videoBitrate;
		bool audioBitrateKnown;
		int audioBitrate;
		MediaInfo(void):
			videoBitrateKnown(false),
			videoBitrate(0),
			audioBitrateKnown(false),
			audioBitrate(0)
		{}
	} mediaInfo;
private:
	Cfg cfg;
};

extern class MPlayer mplayer;

#endif
