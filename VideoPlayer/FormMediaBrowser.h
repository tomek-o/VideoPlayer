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
struct PlaylistEntry;

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
	TMenuItem *miRenamePlaylist;
	TMenuItem *N1;
	TMenuItem *miHidePlaylist;
	TMenuItem *miUnhidePlaylist;
	void __fastcall lvCachedFilesKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall miCopyFileClick(TObject *Sender);
	void __fastcall miNewPlaylistClick(TObject *Sender);
	void __fastcall pcSourceMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall miDeletePlaylistClick(TObject *Sender);
	void __fastcall pcSourceDragOver(TObject *Sender, TObject *Source, int X,
          int Y, TDragState State, bool &Accept);
	void __fastcall pcSourceDragDrop(TObject *Sender, TObject *Source, int X,
          int Y);
	void __fastcall miRenamePlaylistClick(TObject *Sender);
	void __fastcall pcSourceChange(TObject *Sender);
	void __fastcall miHidePlaylistClick(TObject *Sender);
	void __fastcall miUnhidePlaylistClick(TObject *Sender);
private:	// User declarations
	AnsiString asLastSelectedFile;
	typedef void (__closure *CallbackStartPlaying)(void);
	void CreatePlaylistTab(AnsiString fileName, bool hidden);
	int LoadPlaylist(TTabSheet *ts);
	TfrmPlaylist* getPlaylist(int id);
	int mouseDownTabIndex;
	int tabIndexToPlaylistId(int index);
	TTabSheet* getTabSheetAtIndex(int index);
	void Play(void);
	void UpdateHiddenPlaylistsList(void);
public:		// User declarations
	__fastcall TfrmMediaBrowser(TComponent* Owner);
	void LoadPlaylists(void);
	const PlaylistEntry* GetFileToPlay(void);
	void Focus(void);
	void SetFiles(const std::vector<AnsiString>& filenames, bool switchTo);
	CallbackStartPlaying callbackStartPlaying;
	void SavePlaylists(void);
	int PlayNextFile(void);
	int PlayPrevFile(void);
	void DeleteFile(AnsiString filename);
	void SetFileLength(double length);
	void SetFileBitrateVideo(int val);
	void SetFileBitrateAudio(int val);
	void SetFilePos(double position);
	void SetFileSoftVol(int val);
	double GetFilePos(AnsiString file);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMediaBrowser *frmMediaBrowser;
//---------------------------------------------------------------------------
#endif

