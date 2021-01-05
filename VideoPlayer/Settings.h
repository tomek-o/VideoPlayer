/** \file
*/
//---------------------------------------------------------------------------

#ifndef SettingsH
#define SettingsH
//---------------------------------------------------------------------------
#include <System.hpp>

class Settings
{
public:
	int Read(AnsiString asFileName);
	int Write(AnsiString asFileName);
	void SetDefault(void);
	struct _gui
	{
		enum { SCALING_MIN = 50 };
		enum { SCALING_MAX = 200 };
		int scalingPct;					///< scaling (percentage)
	} gui;
	struct _frmMain
	{
		int iPosX, iPosY;				///< main window coordinates
		int iHeight, iWidth;			///< main window size
		bool bWindowMaximized;			///< is main window maximize?
		bool bAlwaysOnTop;
		bool bExitFullScreenOnStop;
	} frmMain;
	struct _Logging
	{
		bool bLogToFile;
		bool bFlush;
		enum {
			MIN_MAX_FILE_SIZE = 0,
			MAX_MAX_FILE_SIZE = 1000*1024*1024
		};
		enum {
            DEF_MAX_FILE_SIZE = 10*1024*1024
        };
		int iMaxFileSize;
		unsigned int iMaxUiLogLines;
	} Logging;
	struct _Mplayer
	{
		AnsiString asInstance;
		bool softVol;
		int softVolLevel;
		int softVolMax;
		enum { SOFTVOL_MAX_LIMIT = 1000 };
		enum { SOFTVOL_MAX_DEF = 300 };
		int osdLevel;
		enum { OSD_LEVEL_MIN = 0 };
		enum { OSD_LEVEL_DEFAULT = 1 };
		enum { OSD_LEVEL_MAX = 3 };
		bool showFileNameOnPlayStart;
		_Mplayer(void):
			asInstance("mplayer.exe"),
			softVol(true),
			softVolLevel(100),
			softVolMax(SOFTVOL_MAX_DEF),
			osdLevel(OSD_LEVEL_DEFAULT),
			showFileNameOnPlayStart(false)
		{}
	} Mplayer;
	struct MediaBrowser
	{
        AnsiString asLastPlaylist;
		MediaBrowser(void)
		{}
	} mediaBrowser;
};

extern Settings appSettings;

#endif
