//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormMediaBrowser.h"
#include "FormPlaylist.h"
#include "Log.h"
#include <SysUtils.hpp>
#include <shellapi.h>
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
	LoadPlaylists();
}
//---------------------------------------------------------------------------

int TfrmMediaBrowser::LoadPlaylist(AnsiString fileName)
{
	TTabSheet *ts = new TTabSheet(pcSource);
	ts->Caption = ChangeFileExt(ExtractFileName(fileName), "");
	ts->PageControl = pcSource;
	ts->Visible = true;

	TfrmPlaylist *frmPlaylist = new TfrmPlaylist(ts);
	int rc = frmPlaylist->loadFromFile(fileName);
	if (rc != 0 && FileExists(fileName))
	{
    	LOG("Error %d loading playlist from %s", rc, fileName.c_str());
	}
	frmPlaylist->Visible = true;
	frmPlaylist->Parent = ts;

	playlistForms.push_back(frmPlaylist);

	return rc;
}

void TfrmMediaBrowser::LoadPlaylists(void)
{
	AnsiString dir = GetPlaylistsDir();
	if (dir[dir.Length()] != '\\')
		dir += "\\";	
	LOG("Loading playlists from %s...\n", dir.c_str());

	WIN32_FIND_DATA file;
	AnsiString asSrchPath = dir + "*." + PLAYLIST_EXTENSION;
	HANDLE hFind = FindFirstFile(asSrchPath.c_str(), &file);
	int hasfiles = (hFind != INVALID_HANDLE_VALUE);

	AnsiString defaultPlaylist = DEFAULT_PLAYLIST_NAME + "." + PLAYLIST_EXTENSION;
	LoadPlaylist(dir + defaultPlaylist);


	while (hasfiles)
	{
		AnsiString filename = dir + file.cFileName;

		if (ExtractFileName(filename) != defaultPlaylist)
		{
			LoadPlaylist(filename);
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
}

TfrmPlaylist* TfrmMediaBrowser::getPlaylist(int id)
{
	std::list<TfrmPlaylist*>::iterator iter;
	int i = 0;
	for (iter = playlistForms.begin(); iter != playlistForms.end(); ++iter)
	{
		if (i == id)
		{
			TfrmPlaylist *frm = *iter;
			return frm;
		}
		i++;
	}
	return NULL;
}

AnsiString TfrmMediaBrowser::GetFileToPlay(void)
{
	std::list<TfrmPlaylist*>::iterator iter;
	int id = 0;
	for (iter = playlistForms.begin(); iter != playlistForms.end(); ++iter)
	{
		if (id == pcSource->ActivePageIndex)
		{
			TfrmPlaylist *frm = *iter;
			return frm->getFileToPlay();
		}
		id++;
	}
	return "";
}

void __fastcall TfrmMediaBrowser::btnMplayerInstanceBrowseClick(TObject *Sender)
{
#if 0
	OpenDialog->InitialDir = ExtractFileDir(edOtherFile->Text);
	OpenDialog->FileName = edOtherFile->Text;
	OpenDialog->Filter = "Video files|*.mp4;*.avi;*.mpg;*.wmv;*.flv;*.mov;*.rmvb|All files|*.*";
	OpenDialog->FilterIndex = 0;
	if (OpenDialog->Execute())
		edOtherFile->Text = OpenDialog->FileName;
#endif
}
//---------------------------------------------------------------------------

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
#if 0
	if (pcSource->ActivePage == tsOther)
	{
		edOtherFile->SetFocus();
	}
#endif
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
	frm->SetFiles(filenames);
	//pcSource->ActivePage = pcSource->Pages[0];
}

void TfrmMediaBrowser::SavePlaylists(void)
{
	std::list<TfrmPlaylist*>::iterator iter;
	for (iter = playlistForms.begin(); iter != playlistForms.end(); ++iter)
	{
		TfrmPlaylist *frm = *iter;
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
	LoadPlaylist(GetPlaylistsDir() + "\\" + name + "." + PLAYLIST_EXTENSION);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMediaBrowser::pcSourceMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	mouseDownTabIndex = pcSource->IndexOfTabAt(X, Y);
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

	delete ts;	

	std::list<TfrmPlaylist*>::iterator iter;
	int id = 0;
	for (iter = playlistForms.begin(); iter != playlistForms.end(); ++iter)
	{
		if (id == mouseDownTabIndex)
		{
			TfrmPlaylist *frm = *iter;
			if (frm->deletePlaylistFile())
			{
				MessageBox(this->Handle, "Failed to delete playlist file. Check if file is write protected.",
					Application->Title.c_str(), MB_ICONEXCLAMATION);
			}
			playlistForms.erase(iter);
			break;
		}
		id++;
	}
}
//---------------------------------------------------------------------------

