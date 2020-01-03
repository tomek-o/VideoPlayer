//---------------------------------------------------------------------------

#ifndef ThreadMplayerWaitH
#define ThreadMplayerWaitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
class ThreadMplayerWait : public TThread
{
private:
	HANDLE hProcess;
	unsigned long ExitCode;
	typedef void (__closure *CallbackStopPlaying)(void);
	CallbackStopPlaying callbackStopPlaying;
	void __fastcall StopPlaying(void);
protected:
	void __fastcall Execute();
public:
	__fastcall ThreadMplayerWait(HANDLE hProcess, CallbackStopPlaying callbackStopPlaying);
	bool useCallback;
};
//---------------------------------------------------------------------------
#endif
