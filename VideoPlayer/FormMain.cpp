//---------------------------------------------------------------------------

#include <vcl.h>                                                           
#pragma hdrstop

#include "FormMain.h"
#include "FormMediaBrowser.h"
#include "FormAbout.h"
#include "FormSettings.h"
#include "HotKeys.h"
#include "LogUnit.h"
#include "Log.h"
#include "MplayerOpenSource.h"
#include "PlaylistEntry.h"
#include "common/OS.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMain *frmMain;
//---------------------------------------------------------------------------

namespace
{

HotKeys hotKeys;

}	// namespace

void __fastcall TfrmMain::CreateParams(TCreateParams &Params) 
{
	TForm::CreateParams(Params);
	Params.ExStyle |= WS_EX_STATICEDGE;
	Params.Style |= WS_SIZEBOX;
}

__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner), allowControlHide(true), state(STOP),
	mouseMoveLastX(-1), mouseMoveLastY(-1), lastHotkey(NULL)
{
	// inform OS that we accepting dropping files
	DragAcceptFiles(Handle, True);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormCreate(TObject *Sender)
{
	AnsiString asConfigFile = ChangeFileExt( Application->ExeName, ".cfg" );
	int status = appSettings.Read(asConfigFile);
	if (status != 0)
	{
		// new installation - detect mplayer location - either app directory (default) or mplayer subfolder
		AnsiString relPath = "mplayer\\mplayer.exe";
		AnsiString asMplayerRelativePath = ExtractFileDir(Application->ExeName) + "\\" + relPath;
		if (FileExists(asMplayerRelativePath))
		{
			appSettings.Mplayer.asInstance = relPath;
		}
	}
	if (this->BorderStyle != bsSingle)
	{
		this->Width = appSettings.frmMain.iWidth;
		this->Height = appSettings.frmMain.iHeight;
	}
	this->Top = appSettings.frmMain.iPosY;
	this->Left = appSettings.frmMain.iPosX;
	if (appSettings.frmMain.bWindowMaximized)
		this->WindowState = wsMaximized;
	if (appSettings.Logging.bLogToFile)
		CLog::Instance()->SetFile(ChangeFileExt(Application->ExeName, ".log").c_str());
	else
		CLog::Instance()->SetFile("");
	Caption = Application->Title;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	hotKeys.Unregister(Handle);
	if (state == PLAY || state == PAUSE)
	{
		UpdateFilePos();
	}
	mplayer.stop(false);
	SetState(STOP);
	WriteSettings();
	frmMediaBrowser->SavePlaylists();
	CanClose = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actShowAboutExecute(TObject *Sender)
{
	frmAbout->ShowModal();
}
//---------------------------------------------------------------------------

void TfrmMain::WriteSettings(void)
{
	AnsiString asConfigFile = ChangeFileExt( Application->ExeName, ".cfg" );
#if 0
	appSettings.frmMain.bWindowMaximized = (this->WindowState == wsMaximized);
#else
	appSettings.frmMain.bWindowMaximized = false;
#endif
	if (this->WindowState != wsMaximized)
	{
		// these values are meaningless is wnd is maximized
		appSettings.frmMain.iWidth = this->Width;
		appSettings.frmMain.iHeight = this->Height;
		appSettings.frmMain.iPosY = this->Top;
		appSettings.frmMain.iPosX = this->Left;
	}
	appSettings.Mplayer.softVolLevel = mplayer.getCfg().softVolLevel;
	appSettings.Write(asConfigFile);
}

void __fastcall TfrmMain::FormShow(TObject *Sender)
{
    static bool once = false;
    if (!once)
    {
		once = true;
		frmLog->SetLogLinesLimit(appSettings.Logging.iMaxUiLogLines);		
		CLog::Instance()->SetLevel(E_LOG_TRACE);
		CLog::Instance()->callbackLog = frmLog->OnLog;

		Settings defaultSettings;
		ApplySettings(defaultSettings);

		mplayer.callbackStopPlaying = CallbackStopPlayingFn;
		mplayer.callbackMediaInfoUpdate = CallbackMediaInfoUpdateFn;

		frmMediaBrowser = new TfrmMediaBrowser(this);
		frmMediaBrowser->Parent = this;
		frmMediaBrowser->callbackStartPlaying = CallbackStartPlayingFn;

		ShowMediaBrowser(true);

		if (ParamCount() == 1)
		{
			// switch to default playlist if adding files from command line
			appSettings.mediaBrowser.asLastPlaylist = "";
		}

		frmMediaBrowser->LoadPlaylists();

		// load file specified with command line
		if (ParamCount() == 1)
		{
			//if (FileExists(ParamStr(1)))
			//	appSettings.Editor.asDefaultDir = ExtractFileDir(ParamStr(1));
			std::vector<AnsiString> files;
			files.push_back(ParamStr(1));
			OpenFiles(files);
		}
	}
	LOG("Application started");
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormDestroy(TObject *Sender)
{
	CLog::Instance()->Destroy();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actShowLogExecute(TObject *Sender)
{
	if (!frmLog->Visible)
		frmLog->Show();
	frmLog->BringToFront();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::btnCloseClick(TObject *Sender)
{
	Close();	
}
//---------------------------------------------------------------------------

void TfrmMain::ApplySettings(const Settings &prev)
{
	if (appSettings.frmMain.bAlwaysOnTop)
		this->FormStyle = fsStayOnTop;
	else
		this->FormStyle = fsNormal;

	if (appSettings.frmMain.controlPanelPosition == Settings::_frmMain::CONTROL_PANEL_BOTTOM)
	{
		pnlControl->Align = alBottom;
	}
	else
	{
    	pnlControl->Align = alTop;
	}

	MPlayer::Cfg mcfg;
	mcfg.parent = pnlMain->Handle; //this->Handle;
	mcfg.colorkey = this->Color;

	AnsiString asMplayerRelativePath = ExtractFileDir(Application->ExeName) + "\\" + appSettings.Mplayer.asInstance;
	if (FileExists(asMplayerRelativePath))
	{
		mcfg.asInstance = asMplayerRelativePath;
	}
	else
	{
		mcfg.asInstance = appSettings.Mplayer.asInstance;
	}

	mcfg.softVol = appSettings.Mplayer.softVol;
	mcfg.softVolLevel = appSettings.Mplayer.softVolLevel;
	mcfg.softVolMax = appSettings.Mplayer.softVolMax;
	mcfg.asExtraParams = appSettings.Mplayer.asExtraParams;
	if (mplayer.Configure(mcfg) != 0)
		MessageBox(this->Handle, "Failed to find mplayer instance. Check mplayer.exe location in settings.", this->Caption.c_str(), MB_ICONEXCLAMATION);

	if (prev.hotKeyConf != appSettings.hotKeyConf)
	{
		RegisterGlobalHotKeys();
	}
}

void __fastcall TfrmMain::btnSettingsClick(TObject *Sender)
{
	Settings prev = appSettings;	// keep track what is changed
	frmSettings->appSettings = &appSettings;
	frmSettings->ShowModal();
	ApplySettings(prev);
	WriteSettings();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::btnPlayClick(TObject *Sender)
{
	if (btnPlay->Down)
		Play();
	else
		btnPlay->Down = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::pnlDragWindow(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (Button == mbLeft)
	{
		enum {SC_DRAGMOVE = 0xF012};
		ReleaseCapture();
		SendMessage(this->Handle, WM_SYSCOMMAND, SC_DRAGMOVE, 0);
	}
	else if (Button == mbRight)
	{
		ToggleOsd();
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::pnlControlMouseLeave(TObject *Sender)
{
	tmrShowControl->Enabled = false;
	tmrShowControl->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::pnlVideoMouseMove(TObject *Sender, TShiftState Shift,
      int X, int Y)
{
	if (X == mouseMoveLastX && Y == mouseMoveLastY)
		return;
	//LOG("X=%d, Y=%d", X, Y);
	mouseMoveLastX = X;
	mouseMoveLastY = Y;
	if (pnlMain->Cursor != crDefault)
	{
		tmrCursorHide->Enabled = false;
		tmrCursorHide->Enabled = true;
		pnlMain->Cursor = crDefault;
	}
}
//---------------------------------------------------------------------------

bool TfrmMain::MousePosOverControlPanel(const TPoint &Position)
{
	if (Position.x < 0 || Position.x >= Width)
		return false;

	if (appSettings.frmMain.controlPanelPosition == Settings::_frmMain::CONTROL_PANEL_BOTTOM)
	{
        // bottom
		if (Position.y >= Height - pnlControl->Height && Position.y < Height)
			return true;
	}
	else
	{
        // top
		if (Position.y >= 0 && Position.y < pnlControl->Height)
			return true;
	}
	return false;
}


void __fastcall TfrmMain::tmrShowControlTimer(TObject *Sender)
{
	TPoint Position;
	try
	{
		// apprently that gives exception "call to OS function failed"
		// when suspending to RAM
		Position = ScreenToClient(Mouse->CursorPos);
	}
	catch(...)
	{
		return;
	}
	if (pnlControl->Visible == false)
	{
		if (MousePosOverControlPanel(Position) == true)
			pnlControl->Visible = true;
	}
	else
	{
		if (MousePosOverControlPanel(Position) == false)
		{
			if (allowControlHide)
			{
				pnlControl->Visible = false;
			}
		}
	}
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::FormKeyDown(TObject *Sender, WORD &Key,
	  TShiftState Shift)
{
#if 0
	LOG("Key = %d, CTRL = %d, SHIFT = %d, ALT = %d\n",
		Key,
		Shift.Contains(ssCtrl), Shift.Contains(ssShift), Shift.Contains(ssAlt));
#endif
	const HotKeyConf* cfg = hotKeys.Find(appSettings.hotKeyConf, Key, Shift, false);
	if (cfg == lastHotkey)
	{
		// anti-repeat
		return;
	}
	lastHotkey = cfg;
	if (cfg)
	{
		ExecAction(cfg->action);
		tmrAntirepeat->Enabled = false;
		tmrAntirepeat->Enabled = true;
	}
}
//---------------------------------------------------------------------------


void TfrmMain::ShowMediaBrowser(bool state)
{
	if (state == true)
	{
		frmMediaBrowser->Visible = true;
		allowControlHide = false;
		tmrShowControl->Enabled = false;
		pnlControl->Visible = true;
		frmMediaBrowser->Focus();
	}
	else
	{
		frmMediaBrowser->Visible = false;
		tmrShowControl->Enabled = true;
		allowControlHide = true;
	}
}

void TfrmMain::CallbackStartPlayingFn(void)
{
	Play();
}

void TfrmMain::CallbackStopPlayingFn(void)
{
	int status = frmMediaBrowser->PlayNextFile();
	if (status != 0)
	{
		if (appSettings.frmMain.bExitFullScreenOnStop && (WindowState == wsMaximized))
		{
			ToggleFullscreen();
		}
		SetState(STOP);
	}
}

void TfrmMain::CallbackMediaInfoUpdateFn(void)
{
	MPlayer::MediaInfo &mediaInfo = mplayer.mediaInfo;
	AnsiString text;
	if (mediaInfo.videoBitrateKnown)
	{
		text.sprintf("V: %d", mediaInfo.videoBitrate);
	}
	else
	{
		text = "V: ?";
	}
	lblV->Caption = text;
	if (mediaInfo.audioBitrateKnown)
	{
		text.sprintf("A: %d", mediaInfo.audioBitrate);
	}
	else
	{
		text = "A: ?";
	}
	lblA->Caption = text;

	double length = mplayer.getFileLength();
	if (length >= 0)
	{
    	frmMediaBrowser->SetFileLength(length);
	}
}

void TfrmMain::Play(void)
{
	enum STATE prevState = state;
	if (state == STOP || state == PLAY /* playing next file immediately from Stop callbak */)
	{
		const PlaylistEntry *entry = frmMediaBrowser->GetFileToPlay();
		if (entry)
		{
			double filePosition = frmMediaBrowser->GetFilePos(entry->fileName);
			SetState(PLAY);
			int volume = -1;
			if (appSettings.Mplayer.useSeparateVolumeForEachFile)
			{
				volume = entry->softVolLevel;
				LOG("File volume: %d\n", volume);
			}
			mplayer.play(entry->fileName, volume, entry->mplayerExtraParams);
			mplayer.setOsdLevel(appSettings.Mplayer.osdLevel);
			if (appSettings.Mplayer.showFileNameOnPlayStart)
			{
				AnsiString text;
				text.sprintf("File: %s", ExtractFileName(entry->fileName).c_str());
				mplayer.osdShowText(text, 2000);
			}
			if (prevState == STOP)
			{
				if (filePosition > 1.0)
				{
					mplayer.seekAbsolute(filePosition);
				}
			}
		}
		else
		{
			btnPlay->Down = false;
		}
	}
	else if (state == PAUSE)
	{
		mplayer.pause();
		SetState(PLAY);
	}
}


void __fastcall TfrmMain::btnStopClick(TObject *Sender)
{
	if (state == PLAY || state == PAUSE)
	{
		UpdateFilePos();
	}
	else if (state == STOP)
	{
    	frmMediaBrowser->SetFilePos(0.0);
	}
	mplayer.stop(false);
	SetState(STOP);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::tmrCursorHideTimer(TObject *Sender)
{
	if (pnlMain->Cursor != crNone)
		pnlMain->Cursor = crNone;
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::FormMouseWheel(TObject *Sender, TShiftState Shift,
      int WheelDelta, TPoint &MousePos, bool &Handled)
{
	int delta = (WheelDelta>0)?(1):(-1);
#if 0
	mplayer.changeVolume(delta);
#else
	int val = mplayer.getCfg().softVolLevel + delta;
	if (val < 0)
		val = 0;
	else if (val > 100)
		val = 100; //mplayer.getCfg().softVolMax;
	appSettings.Mplayer.softVolLevel = val;
	mplayer.changeVolumeAbs(val);
	if (appSettings.Mplayer.useSeparateVolumeForEachFile)
	{
    	frmMediaBrowser->SetFileSoftVol(val);
	}
#endif
}
//---------------------------------------------------------------------------

void TfrmMain::ToggleFullscreen(void)
{
	if (WindowState == wsMaximized)
	{
        // maximized -> normal switch
		WindowState = wsNormal;
		tmrCursorHide->Enabled = false;
		pnlMain->Cursor = crDefault;
		btnFullscreen->Down = false;
	}
	else
	{
		// keep size/position settings before maximizing
		appSettings.frmMain.iWidth = this->Width;
		appSettings.frmMain.iHeight = this->Height;
		appSettings.frmMain.iPosY = this->Top;
		appSettings.frmMain.iPosX = this->Left;
		// maximize
		WindowState = wsMaximized;
		tmrCursorHide->Enabled = true;
		btnFullscreen->Down = true;
		tmrRefreshControl->Enabled = true;
	}
}

void TfrmMain::ToggleOsd(void)
{
	appSettings.Mplayer.osdLevel++;
	if (appSettings.Mplayer.osdLevel > Settings::_Mplayer::OSD_LEVEL_MAX)
		appSettings.Mplayer.osdLevel = Settings::_Mplayer::OSD_LEVEL_MIN;
	mplayer.setOsdLevel(appSettings.Mplayer.osdLevel);
}

void __fastcall TfrmMain::btnFullscreenClick(TObject *Sender)
{
	ToggleFullscreen();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::btnMinimizeClick(TObject *Sender)
{
	Application->Minimize();	
}
//---------------------------------------------------------------------------

// fires an event when a file, or files are dropped onto the application.
void __fastcall TfrmMain::WMDropFiles(TWMDropFiles &message)
{
	std::vector<AnsiString> fileNames;

	int Count = DragQueryFile((HDROP)message.Drop, 0xFFFFFFFF, NULL, MAX_PATH);

	fileNames.resize(Count);
	for (int i=0; i<Count; i++)
	{
		fileNames[i].SetLength(MAX_PATH);
	}

    // index through the files and query the OS for each file name...
    for (int index = 0; index < Count; ++index)
    {
		AnsiString& fname = fileNames[index];
		// the following code gets the FileName of the dropped file. it
        // looks cryptic but that's only because it is. Hey, Why do you think
        // Delphi and C++Builder are so popular anyway? Look up DragQueryFile
        // the Win32.hlp Windows API help file.
		fname.SetLength(DragQueryFile((HDROP)message.Drop, index, fname.c_str(), MAX_PATH));
	}
	OpenFiles(fileNames);

	// tell the OS that you're finished...
	DragFinish((HDROP) message.Drop);
}

void TfrmMain::OpenFiles(std::vector<AnsiString> filenames)
{
	frmMediaBrowser->SetFiles(filenames, true);
	frmMediaBrowser->SetFilePos(0.0);	
	mplayer.stop(false);
	SetState(STOP);
	Play();
}


void __fastcall TfrmMain::btnPauseStillClick(TObject *Sender)
{
	if (state == PLAY)
	{
		Pause();
	}
	else if (state == PAUSE)
	{
		mplayer.frame_step();
		SetState(PAUSE);
	}
	else
	{
    	btnPauseStill->Down = false;
	}
}
//---------------------------------------------------------------------------

void TfrmMain::Pause(void)
{
	if (state == PLAY)
		SetState(PAUSE);
	else
		SetState(PLAY);
	mplayer.pause();
}

void TfrmMain::Skip(void)
{
	if (state != PLAY && state != PAUSE)
	{
		return;
	}
	mplayer.stop(false);
	state = STOP;	// forcing PAUSE -> STOP transition
	int status = frmMediaBrowser->PlayNextFile();
	if (status != 0)
	{
		if (appSettings.frmMain.bExitFullScreenOnStop && (WindowState == wsMaximized))
		{
			ToggleFullscreen();
		}
		SetState(STOP);
	}
}

void TfrmMain::Prev(void)
{
	if (state != PLAY && state != PAUSE)
	{
		return;
	}
	mplayer.stop(false);
	state = STOP;	// forcing PAUSE -> STOP transition
	int status = frmMediaBrowser->PlayPrevFile();
	if (status != 0)
	{
		if (appSettings.frmMain.bExitFullScreenOnStop && (WindowState == wsMaximized))
		{
			ToggleFullscreen();
		}
		SetState(STOP);
	}
}

void TfrmMain::SetState(enum STATE state)
{
	this->state = state;
	switch (state)
	{
	case STOP:
		btnPlay->Down = false;
		btnPauseStill->Down = false;
		ShowMediaBrowser(true);		
		break;
	case PAUSE:
		btnPlay->Down = false;
		btnPauseStill->Down = true;
		break;
	case PLAY:
		btnPlay->Down = true;
		btnPauseStill->Down = false;
		ShowMediaBrowser(false);
		break;
	default:
		break;
	}
}


void __fastcall TfrmMain::tmrRefreshControlTimer(TObject *Sender)
{
    tmrRefreshControl->Enabled = false;
	if (pnlControl->Visible)
	{
		pnlControl->Refresh();
		pnlControlRight->Refresh();
		pnlControl->BringToFront();
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::tmrGetFilePosTimer(TObject *Sender)
{
	if (state == PLAY)
	{
		mplayer.getTimePos();
	}
}
//---------------------------------------------------------------------------

void TfrmMain::UpdateFilePos(void)
{
	double position = mplayer.getFilePosition();
	if (position >= 0)
	{
		frmMediaBrowser->SetFilePos(position);
	}
}

void TfrmMain::RegisterGlobalHotKeys(void)
{
	int rc = hotKeys.RegisterGlobal(appSettings.hotKeyConf, Handle);
	if (rc > 0)
	{
		AnsiString msg;
		msg.sprintf("Failed to register %d global hotkey(s), see log for details", rc);
		MessageBox(this->Handle, msg.c_str(), this->Caption.c_str(), MB_ICONINFORMATION);
	}
}

void __fastcall TfrmMain::WMHotKey(TWMHotKey &Message)
{
	TForm::Dispatch(&Message);
	const HotKeyConf* cfg = hotKeys.FindGlobal(Message.HotKey);
	if (IsWin7OrLater() == false)
	{
		// MOD_NOREPEAT not supported, use own antirepeat
		if (cfg == lastHotkey)
		{
			// anti-repeat
			return;
		}
		lastHotkey = cfg;
	}
	if (cfg)
	{
		ExecAction(cfg->action);
		if (IsWin7OrLater() == false)
		{
			tmrAntirepeat->Enabled = false;
			tmrAntirepeat->Enabled = true;
		}
	}
}

void TfrmMain::ExecAction(const struct Action& action)
{
	switch (action.type)
	{
	case Action::TYPE_NONE:
		break;
	case Action::TYPE_PLAY_STOP:
		if (state == PLAY || state == PAUSE)
		{
			UpdateFilePos();
			mplayer.stop(false);
			SetState(STOP);
		}
		break;
	case Action::TYPE_PLAY_PAUSE:
		if (state == PLAY || state == PAUSE)
		{
			Pause();
		}
		break;
	case Action::TYPE_SEEK_M3:
		if (state == PLAY || state == PAUSE)
		{
			mplayer.seekRelative(-3);
		}
		break;
	case Action::TYPE_SEEK_P3:
		if (state == PLAY || state == PAUSE)
		{
			mplayer.seekRelative(+3);
		}
		break;
	case Action::TYPE_SEEK_M60:
		if (state == PLAY || state == PAUSE)
		{
			mplayer.seekRelative(-60);
		}
		break;
	case Action::TYPE_SEEK_P60:
		if (state == PLAY || state == PAUSE)
		{
			mplayer.seekRelative(+60);
		}
		break;
	case Action::TYPE_TOGGLE_FULLSCREEN:
		if (state == PLAY || state == PAUSE)
		{
			ToggleFullscreen();
		}
		else if (state == STOP)
		{
			AnsiString cname = Screen->ActiveControl->ClassName();
			if (cname != "TEdit" && cname != "TMemo")
			{
				ToggleFullscreen();
			}
		}
		break;
	case Action::TYPE_EXIT_FS_EXIT:
		if (WindowState != wsMaximized)
			Close();
		else
			ToggleFullscreen();
		break;
	case Action::TYPE_SHOW_FILE_INFO:
		if (state == PLAY || state == PAUSE)
		{
			AnsiString text;
			text.sprintf("File: %s", ExtractFileName(mplayer.getFilename()).c_str());
			mplayer.osdShowText(text, 2000);
		}
		break;
	case Action::TYPE_MINIMIZE:
		if (state == PLAY || state == PAUSE)
		{
			Application->Minimize();
		}
		else if (state == STOP)
		{
			AnsiString cname = Screen->ActiveControl->ClassName();
			if (cname != "TEdit" && cname != "TMemo")
			{
				Application->Minimize();
			}
		}
		break;
	case Action::TYPE_TOGGLE_OSD:
		if (state == PLAY || state == PAUSE)
		{
			ToggleOsd();
		}
		break;
	case Action::TYPE_SKIP:
		if (state == PLAY || state == PAUSE)
		{
			Skip();
		}
		break;
	case Action::TYPE_PREV:
		if (state == PLAY || state == PAUSE)
		{
			double position = mplayer.getFilePosition();
			if (position < 10.0)
			{
				Prev();
			}
			else
			{
				mplayer.seekAbsolute(0.0);
			}
			break;
		}
		break;
	case Action::TYPE_DELETE_FILE:
		if (state == PLAY || state == PAUSE)
		{
			AnsiString filename = mplayer.getFilename();
			AnsiString msg;
				msg.sprintf("Delete %s?", filename.c_str());

			if (MessageBox(this->Handle, msg.c_str(),
				Application->Title.c_str(), MB_YESNO | MB_DEFBUTTON2 | MB_ICONEXCLAMATION) != IDYES)
			{
				return;
			}
			Skip();
			Sleep(1500);	/** \todo Wait properly until mplayer quits and releases file */
			frmMediaBrowser->DeleteFile(filename);
		}
		break;
	case Action::TYPE_SHOW_LOG:
		actShowLogExecute(NULL);
		break;

	default:
		break;
	}
}

void __fastcall TfrmMain::tmrAntirepeatTimer(TObject *Sender)
{
	tmrAntirepeat->Enabled = false;
	lastHotkey = NULL;
}
//---------------------------------------------------------------------------

