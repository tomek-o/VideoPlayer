//---------------------------------------------------------------------------

#ifndef ThreadMplayerControlH
#define ThreadMplayerControlH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
class ThreadMplayerControl : public TThread
{
private:
protected:
	void __fastcall Execute();
	HANDLE hPipe;
	char buf[32*1024];
	AnsiString Input;
	AnsiString Line;
	typedef void (__closure *CallbackConsoleLineRx)(AnsiString line);
public:
	__fastcall ThreadMplayerControl(HANDLE hPipe);
	CallbackConsoleLineRx onConsoleLineRx;
};
//---------------------------------------------------------------------------
#endif
