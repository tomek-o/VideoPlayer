//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ThreadMplayerWait.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall ThreadMplayerWait::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall ThreadMplayerWait::ThreadMplayerWait(HANDLE hProcess, CallbackStopPlaying callbackStopPlaying)
	: TThread(false),
	hProcess(hProcess),
	callbackStopPlaying(callbackStopPlaying),
	useCallback(true)
{
	this->FreeOnTerminate = true;
}
//---------------------------------------------------------------------------
void __fastcall ThreadMplayerWait::Execute()
{
	WaitForSingleObject(hProcess, INFINITE);
	GetExitCodeProcess(hProcess, &ExitCode);
	Synchronize(&StopPlaying);
}
//---------------------------------------------------------------------------

void __fastcall ThreadMplayerWait::StopPlaying(void)
{
	if (useCallback)
	{
		if (callbackStopPlaying)
			callbackStopPlaying();
	}
}
