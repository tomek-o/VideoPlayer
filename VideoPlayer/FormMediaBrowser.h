//---------------------------------------------------------------------------

#ifndef FormMediaBrowserH
#define FormMediaBrowserH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <StdCtrls.hpp>
#include <Dialogs.hpp>
#include <Menus.hpp>
#include <ImgList.hpp>
#include <vector>
#include <list>
//---------------------------------------------------------------------------

class TfrmPlaylist;

class TfrmMediaBrowser : public TForm
{
__published:	// IDE-managed Components
	TPageControl *pcSource;
	TOpenDialog *OpenDialog;
	TPopupMenu *PopupMenu;
	TMenuItem *miNewPlaylist;
	TImageList *imListListView;
	TSaveDialog *SaveDialog;
	TMenuItem *miDeletePlaylist;
	void __fastcall lvCachedFilesKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall miGoToFileClick(TObject *Sender);
	void __fastcall miCopyFileClick(TObject *Sender);
	void __fastcall miNewPlaylistClick(TObject *Sender);
	void __fastcall pcSourceMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall miDeletePlaylistClick(TObject *Sender);
	void __fastcall pcSourceDragOver(TObject *Sender, TObject *Source, int X,
          int Y, TDragState State, bool &Accept);
	void __fastcall pcSourceDragDrop(TObject *Sender, TObject *Source, int X,
          int Y);
private:	// User declarations
	AnsiString asLastSelectedFile;
	typedef void (__closure *CallbackStartPlaying)(void);
	int LoadPlaylist(AnsiString fileName);
	void LoadPlaylists(void);
	TfrmPlaylist* getPlaylist(int id);
	int mouseDownTabIndex;
public:		// User declarations
	__fastcall TfrmMediaBrowser(TComponent* Owner);
	AnsiString GetFileToPlay(void);
	void Focus(void);
	void SetFiles(const std::vector<AnsiString>& filenames, bool switchTo);
	CallbackStartPlaying callbackStartPlaying;
	void SavePlaylists(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMediaBrowser *frmMediaBrowser;
//---------------------------------------------------------------------------
#endif

