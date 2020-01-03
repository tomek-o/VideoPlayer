//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormPlaylist.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmPlaylist *frmPlaylist;
//---------------------------------------------------------------------------
__fastcall TfrmPlaylist::TfrmPlaylist(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

int TfrmPlaylist::loadFromFile(AnsiString fileName)
{
	this->fileName = fileName;
	int rc = playlist.loadFromFile(this->fileName);
	update();
	if (playlist.getPosition() < lvPlaylist->Items->Count)
		lvPlaylist->Selected = lvPlaylist->Items->Item[playlist.getPosition()];
	return rc;
}

int TfrmPlaylist::saveToFile(AnsiString fileName)
{
	if (fileName != "")
		this->fileName = fileName;
	if (lvPlaylist->Selected)
	{
		playlist.setPosition(lvPlaylist->Selected->Index);
	}
	else
	{
    	playlist.setPosition(-1);
	}
	return playlist.saveToFile(this->fileName);
}

int TfrmPlaylist::deletePlaylistFile(void)
{
	if (fileName == "")
		return -1;
	if (DeleteFile(fileName) == false)
		return -2;
	return 0;
}


void __fastcall TfrmPlaylist::miAddFilesClick(TObject *Sender)
{
	OpenDialog->Filter = "Video files|*.mp4;*.avi;*.mpg;*.wmv;*.flv;*.mov;*.rmvb|All files|*.*";
	OpenDialog->FilterIndex = 0;
	if (OpenDialog->Execute())
	{
		for (int i=0; i<OpenDialog->Files->Count; i++)
		{
			playlist.add(OpenDialog->Files->Strings[i]);
		}
		update();
	}
}
//---------------------------------------------------------------------------

void TfrmPlaylist::update(void)
{
	const std::vector<PlaylistEntry>& entries = playlist.getEntries();
	lvPlaylist->Items->Count = entries.size();
	lvPlaylist->Invalidate(); 
}

void __fastcall TfrmPlaylist::lvPlaylistData(TObject *Sender, TListItem *Item)
{
	int id = Item->Index;
	const std::vector<PlaylistEntry>& entries = playlist.getEntries();
	const PlaylistEntry &entry = entries[id];

	Item->Caption = ExtractFileName(entry.fileName);
	AnsiString asSize;
	asSize.sprintf("%.1f", static_cast<double>(entry.size) / (1024*1024));
	Item->SubItems->Add(asSize);
}
//---------------------------------------------------------------------------

AnsiString TfrmPlaylist::getFileToPlay(void)
{
	int id = -1;
	if (lvPlaylist->Selected)
		id = lvPlaylist->Selected->Index;
	else
	{
		if (lvPlaylist->Items->Count)
			id = 0;
	}
	const std::vector<PlaylistEntry>& entries = playlist.getEntries();
	return entries[id].fileName;
}

void TfrmPlaylist::SetFiles(const std::vector<AnsiString>& filenames)
{
	for (int i=0; i<lvPlaylist->Items->Count; i++)
	{
		lvPlaylist->Items->Item[i]->Selected = false;
	}
	int originalCount = lvPlaylist->Items->Count;

	for (unsigned int i=0; i<filenames.size(); i++)
	{
		playlist.add(filenames[i]);
	}
	update();
	if (filenames.size())
	{
		lvPlaylist->Items->Item[originalCount]->Selected = true;
	}
}

void __fastcall TfrmPlaylist::lvPlaylistKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
	if (Key == VK_DELETE)
	{
		if (!lvPlaylist->Selected)
			return;
		std::vector<PlaylistEntry> newEntries;
		const std::vector<PlaylistEntry>& entries = playlist.getEntries();
		for (int i=0; i<lvPlaylist->Items->Count; i++)
		{
			if (lvPlaylist->Items->Item[i]->Selected == false)
			{
				newEntries.push_back(entries[i]);
			}
		}
		playlist.setEntries(newEntries);
		update();
	}
}
//---------------------------------------------------------------------------

