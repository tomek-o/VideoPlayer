/** \file
 *  \brief Log display form module
 */

/* Copyright (C) 2008-2009 Tomasz Ostrowski <tomasz.o.ostrowski at gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.

 * Miniscope is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <ClipBrd.hpp>

#include "LogUnit.h"
#include "common/ScopedLock.h"
#include "common/Mutex.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmLog *frmLog;
//---------------------------------------------------------------------------
__fastcall TfrmLog::TfrmLog(TComponent* Owner)
	: TForm(Owner),
	iMaxUiLogLines(1000)
{
	callbackClose = NULL;
}
//---------------------------------------------------------------------------

void TfrmLog::OnLog(char *buf)
{
	ScopedLock<Mutex> lock(mutex);
	/** \note DO NOT use any VCL code for safety */
	AddTextToLog(false, clWindowText, buf);
}

/** Actual GUI update
 */
void __fastcall TfrmLog::tmrUpdateTimer(TObject *Sender)
{
	if (!Visible)
		return;
	ScopedLock<Mutex> lock(mutex);

	if (queDisplay.empty())
		return;

	// first save the current caret location
	int selStart = redMain->SelStart;
	int selLength = redMain->SelLength;
	// get the line number of the first visible line
	int firstLine = ::SendMessage(redMain->Handle, EM_GETFIRSTVISIBLELINE, 0 , 0);
	// disable screen redraws
	redMain->Lines->BeginUpdate();

	std::deque<LogEntry>::iterator iter;
	for(iter = queDisplay.begin(); iter != queDisplay.end(); ++iter)
	{
		redMain->SelStart = redMain->GetTextLen();
		redMain->SelLength = 0;
		redMain->SelAttributes->Color = iter->color;
		redMain->SelAttributes->Name = "Courier New";
		redMain->SelText = iter->asText;
	}
	queDisplay.clear();

	while ((unsigned int)redMain->Lines->Count > iMaxUiLogLines)
	{
		redMain->Lines->Delete(0);
	}

	// restore the caret location
	redMain->SelStart = selStart;
	redMain->SelLength = selLength;

	if (chbAutoScroll->Checked)
	{
		redMain->SelStart = redMain->GetTextLen();
		redMain->Perform( EM_SCROLLCARET, 0 , 0 );
	}
	else
	{
		// get the current first visible line,
		// compute the number of lines to scroll, and scroll...
		int currLine = ::SendMessage(redMain->Handle, EM_GETFIRSTVISIBLELINE, 0, 0);
		::SendMessage(redMain->Handle, EM_LINESCROLL, 0, firstLine - currLine);
	}
	redMain->Lines->EndUpdate();
}
//---------------------------------------------------------------------------

void __fastcall TfrmLog::FormClose(TObject *Sender, TCloseAction &Action)
{
	if (callbackClose)
		callbackClose();	
}
//---------------------------------------------------------------------------

void TfrmLog::AddTextToLog(bool bUseTimestamp, TColor color, AnsiString asText)
{
	/** \note DO NOT use any VCL code for safety */
	struct LogEntry logEntry;
	logEntry.color = color;
	logEntry.asText = asText;
	queDisplay.push_back(logEntry);
	if (queDisplay.size() > this->iMaxUiLogLines)
		queDisplay.pop_front();
}
void __fastcall TfrmLog::FormResize(TObject *Sender)
{
	redMain->Refresh();
}
//---------------------------------------------------------------------------

void __fastcall TfrmLog::btnClearClick(TObject *Sender)
{
	redMain->Clear();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmLog::miCopyClick(TObject *Sender)
{
	Clipboard()->SetTextBuf( redMain->SelText.c_str() );	
}
//---------------------------------------------------------------------------

