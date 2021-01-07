/** \file
 *  \brief Log display form module
 *
 *  Log display window is coupled using OnLog callback.
 *  Log callback enqueues supplied log line to private data containter.
 *  Data container size is limited - oldest log entries are discarded on
 *  overflow.
 *  OnLog callback is not coupled with VCL, so Synchronize() is not required.
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

#ifndef LogUnitH
#define LogUnitH
//---------------------------------------------------------------------------

#include "common/Mutex.h"
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <StdCtrls.hpp>
#include <Menus.hpp>
#include <deque>
#include <string>

//---------------------------------------------------------------------------
/** \brief Log display window
*/
class TfrmLog : public TForm
{
__published:	// IDE-managed Components
	TTimer *tmrUpdate;
	TRichEdit *redMain;
	TPanel *pnlBottom;
	TButton *btnClear;
	TCheckBox *chbAutoScroll;
	TPopupMenu *pmenuRichEdit;
	TMenuItem *miCopy;
	void __fastcall tmrUpdateTimer(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormResize(TObject *Sender);
	void __fastcall btnClearClick(TObject *Sender);
	void __fastcall miCopyClick(TObject *Sender);
private:	// User declarations
	Mutex mutex;
	typedef void (__closure *CallbackClose)(void);

	struct LogEntry
	{
		TColor color;
		AnsiString asText;
	};
	std::deque<LogEntry> queDisplay;
	void AddTextToLog(bool bUseTimestamp, TColor color, AnsiString asText);

	unsigned int iMaxUiLogLines;
		
public:		// User declarations
	__fastcall TfrmLog(TComponent* Owner);
	void OnLog(char *buf);                         ///< add text to log window
	void SetLogLinesLimit(int lines) {
    	iMaxUiLogLines = lines;
	}
	CallbackClose callbackClose;                   ///< called when log window is closing
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmLog *frmLog;
//---------------------------------------------------------------------------
#endif
