//---------------------------------------------------------------------------

#include <vcl.h>                                                           
#pragma hdrstop

#include "FormMain.h"
#include "FormMediaBrowser.h"
#include "FormAbout.h"
#include "FormSettings.h"
#include "LogUnit.h"
#include "Log.h"
#include "Mplayer.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMain *frmMain;
//---------------------------------------------------------------------------

void __fastcall TfrmMain::CreateParams(TCreateParams &Params) 
{ 
	TForm::CreateParams(Params);
	Params.ExStyle |= WS_EX_STATICEDGE;
	Params.Style |= WS_SIZEBOX;
}

__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner), allowControlHide(true), state(STOP)
{
	// inform OS that we accepting dropping files
	DragAcceptFiles(Handle, True);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormCreate(TObject *Sender)
{
	AnsiString asConfigFile = ChangeFileExt( Application->ExeName, ".cfg" );
	appSettings.Read(asConfigFile);
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
	mplayer.stop();
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

		frmMediaBrowser = new TfrmMediaBrowser(this);
		frmMediaBrowser->Parent = this;
		frmMediaBrowser->callbackStartPlaying = CallbackStartPlayingFn;
		mplayer.callbackStopPlaying = CallbackStopPlayingFn;
		mplayer.callbackMediaInfoUpdate = CallbackMediaInfoUpdateFn;
		ApplySettings();

		ShowMediaBrowser(true);

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

void TfrmMain::ApplySettings(void)
{
	if (appSettings.frmMain.bAlwaysOnTop)
		this->FormStyle = fsStayOnTop;
	else
		this->FormStyle = fsNormal;
	MPlayer::Cfg mcfg;
	mcfg.parent = pnlMain->Handle; //this->Handle;
	mcfg.colorkey = this->Color;
	mcfg.asInstance = appSettings.Mplayer.asInstance;
	mcfg.softVol = appSettings.Mplayer.softVol;
	mcfg.softVolLevel = appSettings.Mplayer.softVolLevel;
	mcfg.softVolMax = appSettings.Mplayer.softVolMax;
	if (mplayer.Configure(mcfg) != 0)
		MessageBox(this->Handle, "Failed to find mplayer instance. Check mplayer.exe location in settings.", this->Caption.c_str(), MB_ICONEXCLAMATION);
}

void __fastcall TfrmMain::btnSettingsClick(TObject *Sender)
{
	frmSettings->appSettings = &appSettings;
	frmSettings->ShowModal();
	ApplySettings();
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
		mplayer.toggleOsd();
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
	TPoint Position = ScreenToClient(Mouse->CursorPos);
	if (pnlControl->Visible == false)
	{
		if (Position.y >= Height - pnlControl->Height &&
			Position.y < Height &&
			Position.x >= 0 &&
			Position.x < Width)
		{
		}
		else
		{
			tmrShowControl->Enabled = false;
		}
	}
	tmrShowControl->Enabled = true;
	if (Cursor != crDefault)
	{
		tmrCursorHide->Enabled = false;
		tmrCursorHide->Enabled = true;
		Cursor = crDefault;
	}
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::tmrShowControlTimer(TObject *Sender)
{
	//tmrShowControl->Enabled = false;
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
		if (Position.y >= Height - pnlControl->Height &&
			Position.y < Height &&
			Position.x >= 0 &&
			Position.x < Width)
			pnlControl->Visible = true;
	}
	else
	{
		if ((Position.y >= Height - pnlControl->Height &&
			Position.y < Height &&
			Position.x >= 0 &&
			Position.x < Width) == false)
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
	switch(Key)
	{
	case VK_ESCAPE:
		if (WindowState != wsMaximized)
			Close();
		else
			ToggleFullscreen();
		return;
	default:
		break;
	}

	if (state == PLAY || state == PAUSE)
	{
		switch(Key)
		{
			case VK_RETURN:
				mplayer.stop();
				break;
			case VK_LEFT:
				mplayer.seekRelative(-3);
				break;
			case VK_RIGHT:
				mplayer.seekRelative(3);
				break;
			case VK_DOWN:
				mplayer.seekRelative(-60);
				break;
			case VK_UP:
				mplayer.seekRelative(60);
				break;
			case ' ':
				Pause();
				break;
			case 'f':
			case 'F':
				ToggleFullscreen();
				break;
			case 'o':
			case 'O':
				mplayer.toggleOsd();
				break;
		}
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
		allowControlHide = true;
	}
}

void TfrmMain::CallbackStartPlayingFn(void)
{
	Play();
}

void TfrmMain::CallbackStopPlayingFn(void)
{
	if (appSettings.frmMain.bExitFullScreenOnStop && (WindowState == wsMaximized))
	{
    	ToggleFullscreen();
	}
	SetState(STOP);
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
}

void TfrmMain::Play(void)
{
	if (state == STOP)
	{
		AnsiString file = frmMediaBrowser->GetFileToPlay();
		if (file != "")
		{
			SetState(PLAY);
			mplayer.play(file);
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
    SetState(STOP);
	mplayer.stop();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::tmrCursorHideTimer(TObject *Sender)
{
	if (Cursor != crNone)
		Cursor = crNone;
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
#endif
}
//---------------------------------------------------------------------------

void TfrmMain::ToggleFullscreen(void)
{
	if (WindowState == wsMaximized)
	{
		WindowState = wsNormal;
		tmrCursorHide->Enabled = false;
		Cursor = crDefault;
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
