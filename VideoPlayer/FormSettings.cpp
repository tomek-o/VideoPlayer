//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormSettings.h"
#include "FormHotkeys.h"
#include "LogUnit.h"
#include "FormAbout.h"
#include <SysUtils.hpp>
#include <FileCtrl.hpp>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmSettings *frmSettings;
//---------------------------------------------------------------------------
__fastcall TfrmSettings::TfrmSettings(TComponent* Owner)
	: TForm(Owner)
{
	this->appSettings = NULL;
	frmHotkeys = new TfrmHotkeys(tsHotkeys);
	frmHotkeys->Parent = tsHotkeys;
	frmHotkeys->Visible = true;

	pcSettings->ActivePage = tsMplayer;
}
//---------------------------------------------------------------------------
void __fastcall TfrmSettings::FormShow(TObject *Sender)
{
    assert(appSettings);
	tmpSettings = *appSettings;
	chbAlwaysOnTop->Checked = tmpSettings.frmMain.bAlwaysOnTop;
	chbExitFullscreenOnStop->Checked = tmpSettings.frmMain.bExitFullScreenOnStop;

	chbLogToFile->Checked = tmpSettings.Logging.bLogToFile;
	cmbMaxUiLogLines->ItemIndex = -1;
	for (int i=0; i<cmbMaxUiLogLines->Items->Count; i++)
	{
		if ((unsigned int)StrToInt(cmbMaxUiLogLines->Items->Strings[i]) >= tmpSettings.Logging.iMaxUiLogLines)
		{
			cmbMaxUiLogLines->ItemIndex = i;
			break;
		}
	}
	if (cmbMaxUiLogLines->ItemIndex == -1)
	{
		cmbMaxUiLogLines->ItemHeight = cmbMaxUiLogLines->Items->Count - 1;
	}

	edMplayerInstance->Text = tmpSettings.Mplayer.asInstance;
	chbMplayerSoftVol->Checked = tmpSettings.Mplayer.softVol;
	edMplayerSoftVolMax->Text = tmpSettings.Mplayer.softVolMax;
	edMplayerSoftVolMax->Enabled = chbMplayerSoftVol->Checked;
	chbMplayerShowFileNameOnPlayStart->Checked = tmpSettings.Mplayer.showFileNameOnPlayStart;
	edMplayerExtraParams->Text = tmpSettings.Mplayer.asExtraParams;

	frmHotkeys->SetCfg(&tmpSettings.hotKeyConf);
}
//---------------------------------------------------------------------------
void __fastcall TfrmSettings::btnCancelClick(TObject *Sender)
{
	this->Close();	
}
//---------------------------------------------------------------------------
void __fastcall TfrmSettings::btnApplyClick(TObject *Sender)
{
	tmpSettings.frmMain.bExitFullScreenOnStop = chbExitFullscreenOnStop->Checked;
	tmpSettings.Logging.bLogToFile = chbLogToFile->Checked;
	tmpSettings.Mplayer.asInstance = edMplayerInstance->Text;


	tmpSettings.Mplayer.softVol = chbMplayerSoftVol->Checked;
	tmpSettings.Mplayer.softVolMax = StrToIntDef(edMplayerSoftVolMax->Text, 200);
	if (tmpSettings.Mplayer.softVolMax < 50 || tmpSettings.Mplayer.softVolMax > 1000)
		tmpSettings.Mplayer.softVolMax = 200;

	tmpSettings.Mplayer.showFileNameOnPlayStart = chbMplayerShowFileNameOnPlayStart->Checked;

	tmpSettings.Mplayer.asExtraParams = edMplayerExtraParams->Text;

	*appSettings = tmpSettings;
	this->Close();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::chbAlwaysOnTopClick(TObject *Sender)
{
	tmpSettings.frmMain.bAlwaysOnTop = chbAlwaysOnTop->Checked;	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::cmbMaxUiLogLinesChange(TObject *Sender)
{
	tmpSettings.Logging.iMaxUiLogLines = StrToInt(cmbMaxUiLogLines->Text);	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::btnMplayerInstanceBrowseClick(TObject *Sender)
{
	OpenDialog->Filter = "Executables|*.exe";
	OpenDialog->InitialDir = ExtractFileDir(edMplayerInstance->Text);
	OpenDialog->FileName = edMplayerInstance->Text;
	OpenDialog->Options = TOpenOptions() << ofFileMustExist;
	if (OpenDialog->Execute())
		edMplayerInstance->Text = OpenDialog->FileName;
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::btnShowLogWindowClick(TObject *Sender)
{
	frmLog->Show();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::chbMplayerSoftVolClick(TObject *Sender)
{
	edMplayerSoftVolMax->Enabled = chbMplayerSoftVol->Checked;	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::chbMplayerSoftVolKeyPress(TObject *Sender,
      char &Key)
{
	edMplayerSoftVolMax->Enabled = chbMplayerSoftVol->Checked;	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::btnAboutClick(TObject *Sender)
{
	frmAbout->ShowModal();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::FormKeyPress(TObject *Sender, char &Key)
{
	if (Key == VK_ESCAPE)
		Close();	
}
//---------------------------------------------------------------------------

