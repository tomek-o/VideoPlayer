//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormPlaylist.h"
#include "Log.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TfrmPlaylist::TfrmPlaylist(TComponent* Owner, AnsiString fileName)
	: TForm(Owner),
	callbackStartPlaying(NULL),
	fileName(fileName),
	fileLoaded(false)
{
}
//---------------------------------------------------------------------------

int TfrmPlaylist::load(void)
{
	if (fileLoaded)
		return 0;
	int rc = playlist.loadFromFile(this->fileName);
	edFilter->Text = playlist.getFilterText();
	update();
	int position = playlist.getPosition();
	int count = lvPlaylist->Items->Count;
	if (position >= 0 && position < count)
	{
		lvPlaylist->Items->Item[position]->Selected = true;
		//lvPlaylist->Selected->MakeVisible(true);
		lvPlaylist->Selected->MakeVisible(false);
	}
	fileLoaded = true;
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
	if (FileExists(fileName))
	{
		if (DeleteFile(fileName) == false)
			return -2;
	}
	return 0;
}


void __fastcall TfrmPlaylist::miAddFilesClick(TObject *Sender)
{
	OpenDialog->Filter = "Video files|*.mp4;*.avi;*.mpg;*.wmv;*.flv;*.mov;*.rmvb|All files|*.*";
	OpenDialog->FilterIndex = 0;
	const std::vector<PlaylistEntry>& entries = playlist.getEntries();
	if (!entries.empty())
	{
		const AnsiString& fname = entries[entries.size()-1].fileName;
		AnsiString dir = ExtractFileDir(fname);
		if (DirectoryExists(dir))
		{
        	OpenDialog->InitialDir = dir;
		}
	}
	if (OpenDialog->Execute())
	{
		std::vector<AnsiString> fileNames;
		for (int i=0; i<OpenDialog->Files->Count; i++)
		{
			fileNames.push_back(OpenDialog->Files->Strings[i]);
		}
		playlist.add(fileNames);
		update();
	}
}
//---------------------------------------------------------------------------

void TfrmPlaylist::update(void)
{
	const std::vector<FilteredPlaylistEntry>& entries = playlist.getFilteredEntries();
	//LOG("%p update: %u filtered entries (%p)", this, entries.size(), &entries);
	lvPlaylist->Items->Count = entries.size();
	lvPlaylist->Invalidate(); 
}

void __fastcall TfrmPlaylist::lvPlaylistData(TObject *Sender, TListItem *Item)
{
	int id = Item->Index;
	const std::vector<FilteredPlaylistEntry>& entries = playlist.getFilteredEntries();
	const FilteredPlaylistEntry &entry = entries[id];

	if (entry.entry.mark)
	{
		Item->ImageIndex = 2;
	}
	else
	{
		Item->ImageIndex = -1;
	}
	Item->SubItems->Add(ExtractFileName(entry.entry.fileName));
	AnsiString asSize;
	asSize.sprintf("%.1f MB", static_cast<double>(entry.entry.size) / (1024*1024));
	Item->SubItems->Add(asSize);
	if (entry.entry.length < 0)
	{
		Item->SubItems->Add("");
	}
	else if (entry.entry.length > 0.1)
	{
		double length = entry.entry.length;
		int hours = static_cast<int>(length / 3600);
		length -= hours * 3600;
		int minutes = static_cast<int>(length / 60);
		length -= minutes * 60;
		int seconds = static_cast<int>(length);
		AnsiString text;
		text.sprintf("%d:%02d:%02d", hours, minutes, seconds);
		Item->SubItems->Add(text);
	}
	else
	{
		Item->SubItems->Add("?");
	}
	Item->SubItems->Add(entry.entry.timeStamp);
}
//---------------------------------------------------------------------------

const PlaylistEntry* TfrmPlaylist::getFileToPlay(void)
{
	int id = -1;
	if (lvPlaylist->Selected)
	{
		id = lvPlaylist->Selected->Index;
	}
	else
	{
		if (lvPlaylist->Items->Count)
			id = 0;
	}
	if (id >= 0)
	{
		const std::vector<FilteredPlaylistEntry>& entries = playlist.getFilteredEntries();
		return &entries[id].entry;
	}
	else
	{
		return NULL;
	}
}

void TfrmPlaylist::setFiles(const std::vector<AnsiString>& filenames)
{
	for (int i=0; i<lvPlaylist->Items->Count; i++)
	{
		lvPlaylist->Items->Item[i]->Selected = false;
	}
	if (filenames.size() > 1)
	{
		int originalCount = lvPlaylist->Items->Count;
		playlist.add(filenames);
		update();
		if (lvPlaylist->Items->Count > originalCount)
		{
			lvPlaylist->Items->Item[originalCount]->Selected = true;
		}
	}
	else if (filenames.size() == 1)
	{
		int id = playlist.findByName(filenames[0]);
		if (id < 0)
		{
			int originalCount = lvPlaylist->Items->Count;
			playlist.add(filenames);
			update();
			if (lvPlaylist->Items->Count > originalCount)
			{
				lvPlaylist->Items->Item[originalCount]->Selected = true;
			}
		}
		else
		{
			if (lvPlaylist->Items->Count > id)
			{
				lvPlaylist->Items->Item[id]->Selected = true;
			}
		}
	}
}

void __fastcall TfrmPlaylist::lvPlaylistKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
	if (Key == VK_DELETE)
	{
		if (!lvPlaylist->Selected)
			return;
		std::set<unsigned int> ids;
		const std::vector<FilteredPlaylistEntry>& entries = playlist.getFilteredEntries();
		for (int i=0; i<lvPlaylist->Items->Count; i++)
		{
			if (lvPlaylist->Items->Item[i]->Selected)
			{
				ids.insert(entries[i].id);
			}
		}
		playlist.remove(ids);
		update();
		lvPlaylist->ClearSelection();
	}
	else if (Key == VK_RETURN)
	{
    	play();
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmPlaylist::edFilterChange(TObject *Sender)
{
	AnsiString text = edFilter->Text;
	playlist.filter(text);
	update();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmPlaylist::btnFilterClearClick(TObject *Sender)
{
	edFilter->Text = "";	
}
//---------------------------------------------------------------------------

void __fastcall TfrmPlaylist::lvPlaylistDblClick(TObject *Sender)
{
	play();
}
//---------------------------------------------------------------------------

void TfrmPlaylist::play(void)
{
	assert(callbackStartPlaying);
	TListItem *item = lvPlaylist->Selected;
	if (item == NULL)
		return;
	item->MakeVisible(false);
	callbackStartPlaying();
}

int TfrmPlaylist::playNextFile(void)
{
	TListItem *item = lvPlaylist->Selected;
	if (item == NULL)
		return -2;
	int id = item->Index;
	if (id < lvPlaylist->Items->Count - 1)
	{
		item->Selected = false;
		id++;
		TListItem *nextItem = lvPlaylist->Items->Item[id];
		nextItem->Selected = true;
		play();
		return 0;
	}
	return -3;
}

int TfrmPlaylist::playPrevFile(void)
{
	TListItem *item = lvPlaylist->Selected;
	if (item == NULL)
		return -2;
	int id = item->Index;
	if (id > 0)
	{
		item->Selected = false;
		id--;
		TListItem *nextItem = lvPlaylist->Items->Item[id];
		nextItem->Selected = true;
		play();
		return 0;
	}
	return -3;
}

int TfrmPlaylist::renamePlaylistFile(AnsiString newName)
{
	if (RenameFile(fileName, newName) == false)
	{
		return -1;
	}
	fileName = newName;
	return 0;
}

void __fastcall TfrmPlaylist::miRenameFileClick(TObject *Sender)
{
	TListItem *item = lvPlaylist->Selected;
	if (item == NULL)
		return;

	item->MakeVisible(false);

	int id = item->Index;
	const std::vector<FilteredPlaylistEntry>& entries = playlist.getFilteredEntries();
	const FilteredPlaylistEntry &entry = entries[id];

	AnsiString name = ExtractFileName(entry.entry.fileName);
	bool ret = InputQuery("File name", "Name for the file", name);
	if (ret == false || name == ExtractFileName(entry.entry.fileName))
		return;

	AnsiString newFileName = ExtractFileDir(entry.entry.fileName) + "\\" + name;
	int status = playlist.rename(entry.id, newFileName);
	if (status != 0)
	{
		MessageBox(this->Handle, "Failed to rename file.", Application->Title.c_str(), MB_ICONEXCLAMATION);
		return;
	}

	update();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPlaylist::miDeleteFilesClick(TObject *Sender)
{
	if (!lvPlaylist->Selected)
		return;
	std::set<unsigned int> ids;
	const std::vector<FilteredPlaylistEntry>& entries = playlist.getFilteredEntries();
	for (int i=0; i<lvPlaylist->Items->Count; i++)
	{
		if (lvPlaylist->Items->Item[i]->Selected)
		{
			ids.insert(entries[i].id);
		}
	}
	AnsiString msg;
	if (ids.size() == 1)
	{
		const PlaylistEntry& entry = playlist.getEntry(*ids.begin());
		msg.sprintf("Delete %s?", entry.fileName.c_str());
	}
	else
	{
		msg.sprintf("Delete %u files?", ids.size());
	}

	if (MessageBox(this->Handle, msg.c_str(),
		Application->Title.c_str(), MB_YESNO | MB_DEFBUTTON2 | MB_ICONEXCLAMATION) != IDYES)
	{
		return;
	}

	playlist.removeWithFiles(ids);
	update();
	lvPlaylist->ClearSelection();
}
//---------------------------------------------------------------------------

void TfrmPlaylist::deleteFile(AnsiString filename)
{
	std::set<unsigned int> ids;
	const std::vector<PlaylistEntry>& entries = playlist.getEntries();
	for (unsigned int i=0; i<entries.size(); i++)
	{
		if (entries[i].fileName == filename)
			ids.insert(i);
	}
	playlist.removeWithFiles(ids);
	update();
	//lvPlaylist->ClearSelection();
}

void __fastcall TfrmPlaylist::lvPlaylistColumnClick(TObject *Sender,
      TListColumn *Column)
{
	bool ascending = true;
	if (Column->ImageIndex == 0)
	{
		ascending = false;
	}

	for (int i=0; i<lvPlaylist->Columns->Count; i++)
	{
    	lvPlaylist->Column[i]->ImageIndex = -1;
	}

	if (ascending)
	{
		Column->ImageIndex = 0;
	}
	else
	{
		Column->ImageIndex = 1;
	}

	enum Playlist::SortType sortType;
	switch (Column->Index)
	{
	case 0:
		sortType = Playlist::SortByMark;
		break;
	case 1:
		sortType = Playlist::SortByFileName;
		break;
	case 2:
		sortType = Playlist::SortBySize;
		break;
	case 3:
		sortType = Playlist::SortByLength;
		break;
	case 4:
		sortType = Playlist::SortByTimeStamp;
		break;
	default:
		assert(!"Unhandled sort column!");
		return;
	}

	playlist.sort(sortType, ascending);

	update();
	lvPlaylist->ClearSelection();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmPlaylist::miShufflePlaylistClick(TObject *Sender)
{
	playlist.sort(Playlist::SortRandom, false);
	update();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPlaylist::miRemoveDuplicatesClick(TObject *Sender)
{
	playlist.removeDuplicates();
	update();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmPlaylist::miRemoveMissingFromListClick(TObject *Sender)
{
	playlist.removeMissingFiles();
	update();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmPlaylist::miMarkSetClick(TObject *Sender)
{
	if (!lvPlaylist->Selected)
		return;
	const std::vector<FilteredPlaylistEntry>& entries = playlist.getFilteredEntries();
	for (int i=0; i<lvPlaylist->Items->Count; i++)
	{
		if (lvPlaylist->Items->Item[i]->Selected)
		{
			playlist.markSet(entries[i].id);
		}
	}
	update();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPlaylist::miMarkRemoveClick(TObject *Sender)
{
	if (!lvPlaylist->Selected)
		return;
	const std::vector<FilteredPlaylistEntry>& entries = playlist.getFilteredEntries();
	for (int i=0; i<lvPlaylist->Items->Count; i++)
	{
		if (lvPlaylist->Items->Item[i]->Selected)
		{
			playlist.markClear(entries[i].id);
		}
	}
	update();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPlaylist::miMarkDuplicatesBySizeClick(TObject *Sender)
{
	playlist.markDuplicatesBySize();
	update();
}
//---------------------------------------------------------------------------

void TfrmPlaylist::setFileLength(double length)
{
	TListItem *item = lvPlaylist->Selected;
	if (item == NULL)
		return;
	int id = item->Index;
	const std::vector<FilteredPlaylistEntry>& entries = playlist.getFilteredEntries();
	const FilteredPlaylistEntry &entry = entries[id];
    playlist.setLength(entry.id, length);
	update();
}

void TfrmPlaylist::setFilePosition(double position)
{
	TListItem *item = lvPlaylist->Selected;
	if (item == NULL)
		return;
	int id = item->Index;
	const std::vector<FilteredPlaylistEntry>& entries = playlist.getFilteredEntries();
	const FilteredPlaylistEntry &entry = entries[id];
	playlist.setFilePos(entry.id, position);
}

double TfrmPlaylist::getFilePosition(AnsiString file) const {
	return playlist.getFilePos(file);
}

void __fastcall TfrmPlaylist::miGoToFileClick(TObject *Sender)
{
	if (!lvPlaylist->Selected)
		return;
	const std::vector<FilteredPlaylistEntry>& entries = playlist.getFilteredEntries();
	const FilteredPlaylistEntry &entry = entries[lvPlaylist->Selected->Index];

	AnsiString params;
	params.sprintf("/e, /select, %s", entry.entry.fileName.c_str());
	ShellExecute(NULL, "open", "explorer.exe", params.c_str(), NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPlaylist::miSetMplayerExtraParamsClick(TObject *Sender)
{
	if (!lvPlaylist->Selected)
		return;

	AnsiString params;
	{
		const PlaylistEntry& entry = playlist.getEntry(lvPlaylist->Selected->Index);
		params = entry.mplayerExtraParams;
		bool ret = InputQuery("Extra parameters", "Extra parameters for mplayer", params);
		if (ret == false)
			return;
	}

	const std::vector<FilteredPlaylistEntry>& entries = playlist.getFilteredEntries();
	for (int i=0; i<lvPlaylist->Items->Count; i++)
	{
		if (lvPlaylist->Items->Item[i]->Selected)
		{
			playlist.setMplayerExtraParams(entries[i].id, params);
		}
	}
}
//---------------------------------------------------------------------------

