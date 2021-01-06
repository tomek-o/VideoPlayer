//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ThreadMplayerControl.h"
#include "Log.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall ThreadMplayerControl::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

bool __fastcall GetNextLine(AnsiString &Line, AnsiString &buf, char &lastchar)
{
	AnsiString res;
	int d, a;
	d = buf.Pos((char)13);
	a = buf.Pos((char)10);
	if ((d == 0) && (a == 0)) return false;		// no line-ending

	// Normal line ending: 13,10 (MingW) or 10 (CygWin)
	// Status-line ending: 13 (MingW and CygWin)

	// drawback: under MingW, every line has to be processed for statusline format,
	// because line type cannot be determined correctly by line ending

	if (a != 0) {
		// there is a 10
		if ((a == 1) && (lastchar == 13)) {
			// if a 13,10 ending was split
			// do not erase Line, which should contain previous line
			buf.Delete(1,1);
			lastchar = 10;
			return true;
		}
		if (d == a-1) {
			// line ending: 13,10
			Line = buf.SubString(1, a-2);
			buf.Delete(1, a);
			lastchar = 10;
			return true;
		}
		if ((d == 0) || (d > a)) {
			// line ending: 10
			Line = buf.SubString(1, a-1);
			buf.Delete(1, a);
			lastchar = 10;
			return true;
		}
	}
	if (d != 0) {
		// line ending: 13
		Line = buf.SubString(1, d-1);
		buf.Delete(1, d);
		lastchar = 13;
		return true;
	}
	return false;
}


__fastcall ThreadMplayerControl::ThreadMplayerControl(HANDLE hPipe)
	: TThread(false), hPipe(hPipe), onConsoleLineRx(NULL)
{
	this->FreeOnTerminate = true;
}
//---------------------------------------------------------------------------
void __fastcall ThreadMplayerControl::Execute()
{
	bool ok;
	unsigned long n;

	char lastchar = 0;
	bool ok2;
	do {
		ok = ReadFile(hPipe, buf, sizeof(buf)-1, &n, NULL);
		if (ok && n) {
			buf[n] = 0;
			Input += AnsiString(buf);
			// now split Input into lines, 0x0D and 0x0A separate them.
			do {
				ok2 = GetNextLine(Line, Input, lastchar);
				if (ok2) {
//					if (lastchar == 10) Synchronize(ShowLine);
					if (Line != "") {
						if (strncmp(Line.c_str(), "ANS_TIME_POSITION=", strlen("ANS_TIME_POSITION="))) {
							LOG(Line.c_str());
						}
						Synchronize(&SynchronizedConsoleLineRx);
					}
				}
			} while (ok2);
		}
		else
		{
        	Sleep(1);
		}
	} while (!Terminated);
}
//---------------------------------------------------------------------------

void __fastcall ThreadMplayerControl::SynchronizedConsoleLineRx(void)
{
	if (onConsoleLineRx) {
		onConsoleLineRx(Line);
	}
}
