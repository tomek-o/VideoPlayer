//---------------------------------------------------------------------------

#ifndef FormMainH
#define FormMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <ActnList.hpp>
#include <ImgList.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>

#include "Settings.h"
#include <vector>

//---------------------------------------------------------------------------
class TfrmMain : public TForm
{
__published:	// IDE-managed Components
	TPanel *pnlControl;
	TPanel *pnlControlRight;
	TSpeedButton *btnClose;
	TSpeedButton *btnSettings;
	TSpeedButton *btnStop;
	TTimer *tmrShowControl;
	TSpeedButton *btnPlay;
	TTimer *tmrCursorHide;
	TSpeedButton *btnFullscreen;
	TSpeedButton *btnMinimize;
	TSpeedButton *btnPauseStill;
	TTimer *tmrRefreshControl;
	TLabel *lblV;
	TLabel *lblA;
	TPanel *pnlMain;
	TTimer *tmrGetFilePos;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall actShowAboutExecute(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall actShowLogExecute(TObject *Sender);
	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall btnSettingsClick(TObject *Sender);
	void __fastcall btnPlayClick(TObject *Sender);
	void __fastcall pnlDragWindow(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall pnlControlMouseLeave(TObject *Sender);
	void __fastcall pnlVideoMouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y);
	void __fastcall tmrShowControlTimer(TObject *Sender);
	void __fastcall btnStopClick(TObject *Sender);
	void __fastcall tmrCursorHideTimer(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall FormMouseWheel(TObject *Sender, TShiftState Shift,
          int WheelDelta, TPoint &MousePos, bool &Handled);
	void __fastcall btnFullscreenClick(TObject *Sender);
	void __fastcall btnMinimizeClick(TObject *Sender);
	void __fastcall btnPauseStillClick(TObject *Sender);
	void __fastcall tmrRefreshControlTimer(TObject *Sender);
	void __fastcall tmrGetFilePosTimer(TObject *Sender);
private:	// User declarations
	Settings appSettings;
	void ApplySettings(void);
	bool allowControlHide;
	void ShowMediaBrowser(bool state);
	enum STATE
	{
		STOP = 0,
		PLAY,
		PAUSE
	} state;
	void SetState(enum STATE state);
	void Play(void);
	void Pause(void);
	void Skip(void);
	void Prev(void);
	void ToggleFullscreen(void);
	void ToggleOsd(void);
	void OpenFiles(std::vector<AnsiString> filenames);
	void WriteSettings(void);
	void __fastcall WMDropFiles(TWMDropFiles &message);
	int mouseMoveLastX, mouseMoveLastY;	// MouseMove is called 2x per second even if mouse is not moving - ?
	void UpdateFileLength(void);
protected:
	void __fastcall CreateParams(TCreateParams   &Params);
public:		// User declarations
	__fastcall TfrmMain(TComponent* Owner);
	void CallbackStartPlayingFn(void);
	void CallbackStopPlayingFn(void);
	void CallbackMediaInfoUpdateFn(void);
	BEGIN_MESSAGE_MAP
	MESSAGE_HANDLER(WM_DROPFILES, TWMDropFiles, WMDropFiles)	
	END_MESSAGE_MAP(TForm);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------
#endif
