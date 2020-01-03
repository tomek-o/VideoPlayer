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
	void __fastcall miAddFilesClick(TObject *Sender);
	void __fastcall lvPlaylistData(TObject *Sender, TListItem *Item);
	void __fastcall lvPlaylistKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall edFilterChange(TObject *Sender);
	void __fastcall btnFilterClearClick(TObject *Sender);
	void __fastcall lvPlaylistDblClick(TObject *Sender);
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
	void playNextFile(void);		
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmPlaylist *frmPlaylist;
//---------------------------------------------------------------------------
#endif
