//---------------------------------------------------------------------------

#ifndef HotkeysH
#define HotkeysH
//---------------------------------------------------------------------------

#include <list>
#include <Classes.hpp>
#include "HotKeyConf.h"

class HotKeys
{
public:
	HotKeys(void);
	/** \brief Find key in configuration
	*/
	const HotKeyConf* Find(const std::list<HotKeyConf> &conf, WORD &Key, TShiftState Shift, bool global);
	const HotKeyConf* FindGlobal(int id);
	int RegisterGlobal(const std::list<HotKeyConf> &conf, HWND hwnd);
	void Unregister(HWND hwnd);
private:
	struct GlobalHotKey
	{
		struct HotKeyConf hotKeyConf;
		int id;
		bool registered;
		bool remove;
		GlobalHotKey(void):
			registered(false),
			remove(false)
		{
		}
	};
	std::list<GlobalHotKey> globalHotKeys;
	int nextId;
	enum {
		MIN_ID = 0x0000,
		MAX_ID = 0xBFFF
	};
	int FindNextId(void);
};

#endif
