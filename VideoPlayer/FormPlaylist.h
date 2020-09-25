//---------------------------------------------------------------------------

#ifndef FormPlaylistH
#define FormPlaylistH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------

#include "Playlist.h"
#include <Menus.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>

class TfrmPlaylist : public TForm
{
__published:	// IDE-managed Components
	TListView *lvPlaylist;
	TPopupMenu *popupMenu;
	TMenuItem *miAddFiles;
	TOpenDialog *OpenDialog;
	TPanel *pnlBottom;
	TLabel *lblFilter;
	TEdit *edFilter;
	TButton *btnFilterClear;
	TMenuItem *miRenameFile;
	TMenuItem *miDeleteFiles;
	TImageList *imagelistPlaylist;
	TMenuItem *miShufflePlaylist;
	TMenuItem *N1;
	TMenuItem *N2;
	TMenuItem *miRemoveDuplicates;
	TMenuItem *miRemoveMissingFromList;
	void __fastcall miAddFilesClick(TObject *Sender);
	void __fastcall lvPlaylistData(TObject *Sender, TListItem *Item);
	void __fastcall lvPlaylistKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall edFilterChange(TObject *Sender);
	void __fastcall btnFilterClearClick(TObject *Sender);
	void __fastcall lvPlaylistDblClick(TObject *Sender);
	void __fastcall miRenameFileClick(TObject *Sender);
	void __fastcall miDeleteFilesClick(TObject *Sender);
	void __fastcall lvPlaylistColumnClick(TObject *Sender, TListColumn *Column);
	void __fastcall miShufflePlaylistClick(TObject *Sender);
	void __fastcall miRemoveDuplicatesClick(TObject *Sender);
	void __fastcall miRemoveMissingFromListClick(TObject *Sender);
private:	// User declarations
	AnsiString fileName;
	Playlist playlist;
	void update(void);
	typedef void (__closure *CallbackStartPlaying)(void);
	void play(void);
public:		// User declarations
	__fastcall TfrmPlaylist(TComponent* Owner);
	int loadFromFile(AnsiString fileName);
	int saveToFile(AnsiString fileName = "");
	int deletePlaylistFile(void);
	AnsiString getFileToPlay(void);
	void setFiles(const std::vector<AnsiString>& filenames);
	CallbackStartPlaying callbackStartPlaying;
	int playNextFile(void);
	int playPrevFile(void);
	AnsiString getFileName(void) const {
    	return fileName;
	}
	int renamePlaylistFile(AnsiString newName);
	void deleteFile(AnsiString filename);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmPlaylist *frmPlaylist;
//---------------------------------------------------------------------------
#endif
