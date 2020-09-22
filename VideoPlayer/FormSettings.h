//---------------------------------------------------------------------------

#ifndef FormSettingsH
#define FormSettingsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
#include "Settings.h"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <Dialogs.hpp>

class TfrmSettings : public TForm
{
__published:	// IDE-managed Components
	TPanel *pnlBottom;
	TButton *btnCancel;
	TButton *btnApply;
	TPageControl *pcGeneral;
	TTabSheet *TabSheet1;
	TTabSheet *pcLogging;
	TCheckBox *chbAlwaysOnTop;
	TLabel *lblUiCapacity;
	TComboBox *cmbMaxUiLogLines;
	TCheckBox *chbLogToFile;
	TTabSheet *tsMplayer;
	TLabel *lblInstance;
	TEdit *edMplayerInstance;
	TSpeedButton *btnMplayerInstanceBrowse;
	TOpenDialog *OpenDialog;
	TButton *btnShowLogWindow;
	TCheckBox *chbExitFullscreenOnStop;
	TCheckBox *chbMplayerSoftVol;
	TLabel *lblSoftVolMax;
	TEdit *edMplayerSoftVolMax;
	TLabel *lblMplayerPercent;
	TButton *btnAbout;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall btnCancelClick(TObject *Sender);
	void __fastcall btnApplyClick(TObject *Sender);
	void __fastcall chbAlwaysOnTopClick(TObject *Sender);
	void __fastcall cmbMaxUiLogLinesChange(TObject *Sender);
	void __fastcall btnMplayerInstanceBrowseClick(TObject *Sender);
	void __fastcall btnShowLogWindowClick(TObject *Sender);
	void __fastcall chbMplayerSoftVolClick(TObject *Sender);
	void __fastcall chbMplayerSoftVolKeyPress(TObject *Sender, char &Key);
	void __fastcall btnAboutClick(TObject *Sender);
private:	// User declarations

public:		// User declarations
	__fastcall TfrmSettings(TComponent* Owner);
	Settings *appSettings;
	Settings tmpSettings;
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmSettings *frmSettings;
//---------------------------------------------------------------------------
#endif
