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
	TPanel *Panel1;
	TLabel *lblFilter;
	TEdit *edFilter;
	TButton *btnFilterClear;
	void __fastcall miAddFilesClick(TObject *Sender);
	void __fastcall lvPlaylistData(TObject *Sender, TListItem *Item);
	void __fastcall lvPlaylistKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall edFilterChange(TObject *Sender);
	void __fastcall btnFilterClearClick(TObject *Sender);
private:	// User declarations
	AnsiString fileName;
	Playlist playlist;
	void update(void);
public:		// User declarations
	__fastcall TfrmPlaylist(TComponent* Owner);
	int loadFromFile(AnsiString fileName);
	int saveToFile(AnsiString fileName = "");
	int deletePlaylistFile(void);
	AnsiString getFileToPlay(void);
	void SetFiles(const std::vector<AnsiString>& filenames);	
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmPlaylist *frmPlaylist;
//---------------------------------------------------------------------------
#endif
