//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormMediaBrowser.h"
#include "FormPlaylist.h"
#include "Log.h"
#include "Settings.h"
#include <SysUtils.hpp>
#include <shellapi.h>
#include <assert.h>
#include <algorithm>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMediaBrowser *frmMediaBrowser;
//---------------------------------------------------------------------------

#pragma warn -8091	// incorrectly issued by BDS2006

namespace {

AnsiString GetPlaylistsDir(void) {
	return ExtractFileDir(Application->ExeName) + "\\playlists";
}

const AnsiString DEFAULT_PLAYLIST_NAME = "Default";
const AnsiString PLAYLIST_EXTENSION = "json";

int ItemAtPos(HWND ControlHandle, int X, int Y)
{
	TTCHitTestInfo HitTestInfo;
	int HitIndex;

	HitTestInfo.pt.x = X;
	HitTestInfo.pt.y = Y;
	HitTestInfo.flags = 0;
	HitIndex = SendMessage(ControlHandle, TCM_HITTEST, 0, long(&HitTestInfo));
	return HitIndex;
}

}

__fastcall TfrmMediaBrowser::TfrmMediaBrowser(TComponent* Owner)
	: TForm(Owner),
	mouseDownTabIndex(-1)
{
	callbackStartPlaying = NULL;
	//pcSource->ActivePage = tsOther;
	if (!DirectoryExists(GetPlaylistsDir()))
	{
		if (CreateDir(GetPlaylistsDir()) == false)
		{
        	LOG("Failed to create directory: %s", GetPlaylistsDir().c_str());
		}
	}
	//LoadPlaylists();
}
//---------------------------------------------------------------------------

void TfrmMediaBrowser::CreatePlaylistTab(AnsiString fileName)
{
	TTabSheet *ts = new TTabSheet(pcSource);
	ts->Caption = ChangeFileExt(ExtractFileName(fileName), "");
	ts->PageControl = pcSource;
	ts->Visible = true;

	TfrmPlaylist *frmPlaylist = new TfrmPlaylist(ts, fileName);

	frmPlaylist->Visible = true;
	frmPlaylist->Parent = ts;
	frmPlaylist->callbackStartPlaying = Play;
}

int TfrmMediaBrowser::LoadPlaylist(TTabSheet *ts)
{
	TfrmPlaylist* frmPlaylist = getPlaylist(ts->TabIndex);
	int rc = frmPlaylist->load();
	if (rc != 0 /* && FileExists(fileName)*/)
	{
		LOG("Error %d loading playlist from %s", rc, frmPlaylist->getFileName().c_str());
	}
	frmPlaylist->Visible = true;
	frmPlaylist->Parent = ts;
	frmPlaylist->callbackStartPlaying = Play;
	return rc;
}

void TfrmMediaBrowser::LoadPlaylists(void)
{
	AnsiString dir = GetPlaylistsDir();
	if (dir[dir.Length()] != '\\')
		dir += "\\";	
	LOG("Loading playlists from %s...", dir.c_str());

	WIN32_FIND_DATA file;
	AnsiString asSrchPath = dir + "*." + PLAYLIST_EXTENSION;
	HANDLE hFind = FindFirstFile(asSrchPath.c_str(), &file);
	int hasfiles = (hFind != INVALID_HANDLE_VALUE);

	AnsiString defaultPlaylist = DEFAULT_PLAYLIST_NAME + "." + PLAYLIST_EXTENSION;
	CreatePlaylistTab(dir + defaultPlaylist);
	//LoadPlaylist(dir + defaultPlaylist);

	while (hasfiles)
	{
		AnsiString filename = dir + file.cFileName;

		if (ExtractFileName(filename) != defaultPlaylist)
		{
            CreatePlaylistTab(filename);
			//LoadPlaylist(filename);
		}

		hasfiles = FindNextFile(hFind, &file);
	}
	FindClose(hFind);

	if (pcSource->PageCount > 1)
	{
		// toggling pages to fix weird problem with wrong page displayed at startup
		pcSource->ActivePage = pcSource->Pages[1];
		pcSource->ActivePage = pcSource->Pages[0];
	}

	int playlistId = 0;
	for (int i=0; i<pcSource->PageCount; i++)
	{
		TfrmPlaylist* frmPlaylist = getPlaylist(i);
		if (frmPlaylist)
		{
			AnsiString name = ExtractFileName(frmPlaylist->getFileName());
			name = UpperCase(name);
			if (UpperCase(name) == UpperCase(appSettings.mediaBrowser.asLastPlaylist))
			{
				playlistId = i;
				break;
			}
		}
	}

	pcSource->ActivePage = pcSource->Pages[playlistId];	

	pcSourceChange(NULL);
}

TfrmPlaylist* TfrmMediaBrowser::getPlaylist(int id)
{
	if (id < 0 || id >= pcSource->PageCount)
		return NULL;
	TTabSheet *ts = pcSource->Pages[id];
	if (ts->ControlCount <= 0)
		return NULL;
	TfrmPlaylist *frm = dynamic_cast<TfrmPlaylist*>(ts->Controls[0]);
	return frm;
}

AnsiString TfrmMediaBrowser::GetFileToPlay(void)
{
	TfrmPlaylist *frm = getPlaylist(pcSource->ActivePageIndex);
	if (frm == NULL)
		return "";
	return frm->getFileToPlay();		
}

void __fastcall TfrmMediaBrowser::lvCachedFilesKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
	if (Key == VK_RETURN)
	{
		if(callbackStartPlaying)
			callbackStartPlaying();
	}
}
//---------------------------------------------------------------------------

void TfrmMediaBrowser::Focus(void)
{
	this->SetFocus();
	TfrmPlaylist *frm = getPlaylist(pcSource->ActivePageIndex);
	if (frm)
	{
        frm->SetFocus();
		frm->lvPlaylist->SetFocus();
		if (frm->lvPlaylist->Selected)
		{
			frm->lvPlaylist->Selected->Focused = true;
		}
	}
}

void __fastcall TfrmMediaBrowser::miGoToFileClick(TObject *Sender)
{
#if 0
	TListItem* item = lvCachedFiles->Selected;
	if (item == NULL)
		return;
	int id = item->Index;
	AnsiString params;
	params.sprintf("/e, /select, %s", operaCache[id].filename.c_str());
	ShellExecute(NULL, "open", "explorer.exe", params.c_str(), NULL, SW_SHOWNORMAL);
#endif
}
//---------------------------------------------------------------------------

void __fastcall TfrmMediaBrowser::miCopyFileClick(TObject *Sender)
{
#if 0
	TListItem* item = lvCachedFiles->Selected;
	if (item == NULL)
		return;
	int id = item->Index;
	if (SaveDialog->Execute())
	{
		SHFILEOPSTRUCT fos;
		fos.hwnd = Handle;
		//operacja kopiowania
		fos.wFunc = FO_COPY;
		//plik �r�d�owy
		fos.pFrom = operaCache[id].filename.c_str();
		//plik docelowy
		fos.pTo = SaveDialog->FileName.c_str();
		fos.fFlags = 0;
		SHFileOperation(&fos);
	}
#endif	
}
//---------------------------------------------------------------------------

void TfrmMediaBrowser::SetFiles(const std::vector<AnsiString>& filenames, bool switchTo)
{
	TfrmPlaylist *frm = getPlaylist(pcSource->ActivePageIndex);
	frm->setFiles(filenames);
}

void TfrmMediaBrowser::SavePlaylists(void)
{
	for (int i=0; i<pcSource->PageCount; i++)
	{
		TfrmPlaylist *frm = getPlaylist(i);
		assert(frm);
		frm->saveToFile();
	}
}

void __fastcall TfrmMediaBrowser::miNewPlaylistClick(TObject *Sender)
{
	AnsiString name;
	name = FormatDateTime("yyyymmdd_hhnnss", Now());
	bool ret = InputQuery("Playlist name", "Name for new playlist", name);
	if (ret == false)
		return;
	CreatePlaylistTab(GetPlaylistsDir() + "\\" + name + "." + PLAYLIST_EXTENSION);
	// switch to new playlist (bug with new playlist visible already)
	pcSource->ActivePage = pcSource->Pages[pcSource->PageCount - 1];
}
//---------------------------------------------------------------------------

void __fastcall TfrmMediaBrowser::miRenamePlaylistClick(TObject *Sender)
{
	if (mouseDownTabIndex < 0)
	{
		LOG("Rename playlist failed: tabIndex = %d", mouseDownTabIndex);
		return;
	}
	if (mouseDownTabIndex == 0)
	{
		MessageBox(this->Handle, "Default playlist cannot be renamed", Application->Title.c_str(), MB_ICONINFORMATION);
		return;
	}
	TTabSheet *ts = pcSource->Pages[mouseDownTabIndex];

	AnsiString name = ts->Caption;
	bool ret = InputQuery("Playlist name", "Name for the playlist", name);
	if (ret == false || name == ts->Caption)
		return;

	AnsiString fileName = GetPlaylistsDir() + "\\" + name + "." + PLAYLIST_EXTENSION;
	for (int i=0; i<pcSource->PageCount; i++)
	{
		TfrmPlaylist *frm = getPlaylist(i);
		assert(frm);
		if (frm->getFileName() == fileName)
		{
			MessageBox(this->Handle, "Playlist with this name already exists.",
				Application->Title.c_str(), MB_ICONEXCLAMATION);
			return;
		}
	}

	TfrmPlaylist *frm = getPlaylist(mouseDownTabIndex);
	assert(frm);
	if (frm->renamePlaylistFile(fileName))
	{
		MessageBox(this->Handle, "Failed to rename playlist file. Check if file is write protected.",
			Application->Title.c_str(), MB_ICONEXCLAMATION);
		return;
	}

	ts->Caption = name;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMediaBrowser::pcSourceMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (Button == mbLeft)
	{
		mouseDownTabIndex = pcSource->IndexOfTabAt(X, Y);
		pcSource->BeginDrag(0);
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMediaBrowser::miDeletePlaylistClick(TObject *Sender)
{
	if (mouseDownTabIndex < 0)
	{
        LOG("Delete playlist failed: tabIndex = %d", mouseDownTabIndex);
		return;
	}
	if (mouseDownTabIndex == 0)
	{
		MessageBox(this->Handle, "Default playlist cannot be deleted", Application->Title.c_str(), MB_ICONINFORMATION);
		return;
	}
	TTabSheet *ts = pcSource->Pages[mouseDownTabIndex];

	AnsiString caption;
	caption.sprintf("Delete %s?", ts->Caption.c_str());
	if (MessageBox(this->Handle, caption.c_str(),
		Application->Title.c_str(), MB_YESNO | MB_DEFBUTTON2 | MB_ICONEXCLAMATION) != IDYES)
	{
		return;
	}

	TfrmPlaylist *frm = getPlaylist(mouseDownTabIndex);
	assert(frm);
	if (frm->deletePlaylistFile())
	{
		MessageBox(this->Handle, "Failed to delete playlist file. Check if file is write protected.",
			Application->Title.c_str(), MB_ICONEXCLAMATION);
	}

	delete ts;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMediaBrowser::pcSourceDragOver(TObject *Sender,
      TObject *Source, int X, int Y, TDragState State, bool &Accept)
{
	Accept = Sender->ClassNameIs("TPageControl");
}
//---------------------------------------------------------------------------

void __fastcall TfrmMediaBrowser::pcSourceDragDrop(TObject *Sender,
      TObject *Source, int X, int Y)
{
	if (!Sender->ClassNameIs("TPageControl"))
		return;
	TPageControl *pc = (TPageControl*)Sender;
	int i = ItemAtPos(pc->Handle, X, Y); //GetTabIndex(Sender, X, Y);
	if (i != pc->ActivePage->PageIndex)
	{
		pc->ActivePage->PageIndex = i;
	}
}
//---------------------------------------------------------------------------

void TfrmMediaBrowser::Play(void)
{
	if (callbackStartPlaying)
		callbackStartPlaying();
}

int TfrmMediaBrowser::PlayNextFile(void)
{
	TfrmPlaylist *frm = getPlaylist(pcSource->ActivePageIndex);
	if (frm)
	{
		return frm->playNextFile();
	}
	return -1;
}

int TfrmMediaBrowser::PlayPrevFile(void)
{
	TfrmPlaylist *frm = getPlaylist(pcSource->ActivePageIndex);
	if (frm)
	{
		return frm->playPrevFile();
	}
	return -1;
}

void TfrmMediaBrowser::DeleteFile(AnsiString filename)
{
	TfrmPlaylist *frm = getPlaylist(pcSource->ActivePageIndex);
	if (frm)
	{
		frm->deleteFile(filename);
	}
}

void TfrmMediaBrowser::SetFileLength(double length)
{
	TfrmPlaylist *frm = getPlaylist(pcSource->ActivePageIndex);
	if (frm)
	{
		frm->setFileLength(length);
	}
}

void TfrmMediaBrowser::SetFilePos(double position)
{
	TfrmPlaylist *frm = getPlaylist(pcSource->ActivePageIndex);
	if (frm)
	{
		frm->setFilePosition(position);
	}
}

double TfrmMediaBrowser::GetFilePos(AnsiString file)
{
	TfrmPlaylist *frm = getPlaylist(pcSource->ActivePageIndex);
	if (frm)
	{
		return frm->getFilePosition(file);
	}
	return 0.0;
}

void __fastcall TfrmMediaBrowser::pcSourceChange(TObject *Sender)
{
	TfrmPlaylist *frm = getPlaylist(pcSource->ActivePageIndex);
	if (frm)
	{
		int rc = frm->load();
		if (rc != 0)
		{
			LOG("Error %d loading playlist from %s", rc, frm->getFileName().c_str());
		}
		appSettings.mediaBrowser.asLastPlaylist = ExtractFileName(frm->getFileName());
	}
}
//---------------------------------------------------------------------------

