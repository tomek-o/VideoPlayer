//---------------------------------------------------------------------------


#pragma hdrstop

#include "Settings.h"
#include "common\KeybKeys.h"
#include <algorithm>
#include <fstream>
#include <json/json.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)

Settings appSettings;

inline void strncpyz(char* dst, const char* src, int dstsize) {
	strncpy(dst, src, dstsize);
	dst[dstsize-1] = '\0';
}

void Settings::SetDefault(void)
{
	frmMain.iWidth = 562;
	frmMain.iHeight = 320;
	frmMain.iPosX = 30;
	frmMain.iPosY = 30;
	frmMain.bWindowMaximized = false;
	frmMain.bAlwaysOnTop = false;
	frmMain.bExitFullScreenOnStop = true;

	Logging.bLogToFile = false;
	Logging.bFlush = false;
	Logging.iMaxFileSize = Settings::_Logging::DEF_MAX_FILE_SIZE;
	Logging.iMaxUiLogLines = 5000;

	{
		hotKeyConf.clear();
		struct HotKeyConf hk;
		struct Action action;

		hk.keyCode = "VK_SPACE";
		hk.modifiers = 0;
		action.type = Action::TYPE_PLAY_PAUSE;
		hk.action = action;
		hotKeyConf.push_back(hk);

		hk.keyCode = "VK_RETURN";
		hk.modifiers = 0;
		action.type = Action::TYPE_PLAY_STOP;
		hk.action = action;
		hotKeyConf.push_back(hk);

		hk.keyCode = "VK_LEFT";
		hk.modifiers = 0;
		action.type = Action::TYPE_SEEK_M3;
		hk.action = action;
		hotKeyConf.push_back(hk);

		hk.keyCode = "VK_RIGHT";
		hk.modifiers = 0;
		action.type = Action::TYPE_SEEK_P3;
		hk.action = action;
		hotKeyConf.push_back(hk);

		hk.keyCode = "VK_DOWN";
		hk.modifiers = 0;
		action.type = Action::TYPE_SEEK_M60;
		hk.action = action;
		hotKeyConf.push_back(hk);

		hk.keyCode = "VK_UP";
		hk.modifiers = 0;
		action.type = Action::TYPE_SEEK_P60;
		hk.action = action;
		hotKeyConf.push_back(hk);

		hk.keyCode = "F key";
		hk.modifiers = 0;
		action.type = Action::TYPE_TOGGLE_FULLSCREEN;
		hk.action = action;
		hotKeyConf.push_back(hk);

		hk.keyCode = "I key";
		hk.modifiers = 0;
		action.type = Action::TYPE_SHOW_FILE_INFO;
		hk.action = action;
		hotKeyConf.push_back(hk);

		hk.keyCode = "M key";
		hk.modifiers = 0;
		action.type = Action::TYPE_MINIMIZE;
		hk.action = action;
		hotKeyConf.push_back(hk);

		hk.keyCode = "N key";
		hk.modifiers = 0;
		action.type = Action::TYPE_SKIP;
		hk.action = action;
		hotKeyConf.push_back(hk);

		hk.keyCode = "P key";
		hk.modifiers = 0;
		action.type = Action::TYPE_PREV;
		hk.action = action;
		hotKeyConf.push_back(hk);

		hk.keyCode = "O key";
		hk.modifiers = 0;
		action.type = Action::TYPE_TOGGLE_OSD;
		hk.action = action;
		hotKeyConf.push_back(hk);

		hk.keyCode = "D key";
		hk.modifiers = 0;
		action.type = Action::TYPE_DELETE_FILE;
		hk.action = action;
		hotKeyConf.push_back(hk);

		hk.keyCode = "VK_ESCAPE";
		hk.modifiers = 0;
		action.type = Action::TYPE_EXIT_FS_EXIT;
		hk.action = action;
		hotKeyConf.push_back(hk);

		for (std::list<HotKeyConf>::iterator iter = hotKeyConf.begin(); iter != hotKeyConf.end(); ++iter)
		{
            HotKeyConf &cfg = *iter;
			int id = vkey_find(cfg.keyCode.c_str());
			if (id >= 0)
			{
				cfg.vkCode = vkey_list[id].vkey;
			}
			else
			{
				cfg.vkCode = -1;
			}
		}
	}
}

int Settings::Read(AnsiString asFileName)
{
	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;

    SetDefault();

	try
	{
		std::ifstream ifs(asFileName.c_str());
		std::string strConfig((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		ifs.close();
		bool parsingSuccessful = reader.parse( strConfig, root );
		if ( !parsingSuccessful )
		{
			return 2;
		}
	}
	catch(...)
	{
		return 1;
	}

	int maxX = GetSystemMetrics(SM_CXSCREEN);
	/** \todo Ugly fixed taskbar margin */
	int maxY = GetSystemMetrics(SM_CYSCREEN) - 32;

	const Json::Value &frmMainJson = root["frmMain"];
	frmMain.iWidth = frmMainJson.get("AppWidth", 350).asInt();
	frmMain.iHeight = frmMainJson.get("AppHeight", 300).asInt();
	if (frmMain.iWidth < 250 || frmMain.iWidth > maxX + 20)
	{
		frmMain.iWidth = 250;
	}
	if (frmMain.iHeight < 200 || frmMain.iHeight > maxY + 20)
	{
		frmMain.iHeight = 200;
	}
	frmMain.iPosX = frmMainJson.get("AppPositionX", 30).asInt();
	frmMain.iPosY = frmMainJson.get("AppPositionY", 30).asInt();
	if (frmMain.iPosX < 0)
		frmMain.iPosX = 0;
	if (frmMain.iPosY < 0)
		frmMain.iPosY = 0;
	if (frmMain.iPosX + frmMain.iWidth > maxX)
		frmMain.iPosX = maxX - frmMain.iWidth;
	if (frmMain.iPosY + frmMain.iHeight > maxY)
		frmMain.iPosY = maxY - frmMain.iHeight;
	frmMain.bWindowMaximized = frmMainJson.get("Maximized", false).asBool();
	frmMain.bAlwaysOnTop = frmMainJson.get("AlwaysOnTop", false).asBool();
	frmMain.bExitFullScreenOnStop = frmMainJson.get("ExitFullScreenOnStop", frmMain.bExitFullScreenOnStop).asBool();

	const Json::Value &LoggingJson = root["Logging"];
	Logging.bLogToFile = LoggingJson.get("LogToFile", false).asBool();
	Logging.bFlush = LoggingJson.get("Flush", Logging.bFlush).asBool();
	Logging.iMaxFileSize = LoggingJson.get("MaxFileSize", Logging.iMaxFileSize).asInt();
	if (Logging.iMaxFileSize < Settings::_Logging::MIN_MAX_FILE_SIZE || Logging.iMaxFileSize > Settings::_Logging::MIN_MAX_FILE_SIZE)
	{
		Logging.iMaxFileSize = Settings::_Logging::DEF_MAX_FILE_SIZE;
	}
	Logging.iMaxUiLogLines = LoggingJson.get("MaxUiLogLines", 5000).asInt();

	{
		const Json::Value &jv = root["Mplayer"];
		Mplayer.asInstance = jv.get("Instance", Mplayer.asInstance).asAString();
		Mplayer.softVol = jv.get("SoftVol", Mplayer.softVol).asBool();
		Mplayer.softVolMax = jv.get("SoftVolMax", Mplayer.softVolMax).asInt();
		if (Mplayer.softVolMax < 50 || Mplayer.softVolMax > _Mplayer::SOFTVOL_MAX_LIMIT)
		{
			Mplayer.softVolMax = _Mplayer::SOFTVOL_MAX_DEF;
		}
		Mplayer.softVolLevel = jv.get("SoftVolLevel", Mplayer.softVolLevel).asInt();
		if (Mplayer.softVolLevel < 0 || Mplayer.softVolLevel > Mplayer.softVolMax)
			Mplayer.softVolLevel = 100;
		jv.getInt("OsdLevel", Mplayer.osdLevel);
		if (Mplayer.osdLevel < _Mplayer::OSD_LEVEL_MIN || Mplayer.osdLevel > _Mplayer::OSD_LEVEL_MAX)
			Mplayer.osdLevel = _Mplayer::OSD_LEVEL_DEFAULT;
		jv.getBool("ShowFileNameOnPlayStart", Mplayer.showFileNameOnPlayStart);
	}

	{
		const Json::Value &jv = root["MediaBrowser"];
		jv.getAString("LastPlaylist", mediaBrowser.asLastPlaylist);
	}

	{
		const Json::Value &hotkeyConfJson = root["hotkeyConf"];
		if (hotkeyConfJson.type() == Json::arrayValue)
		{
            hotKeyConf.resize(hotkeyConfJson.size());
			std::list<HotKeyConf>::iterator iter = hotKeyConf.begin();

			for (unsigned int i=0; i<hotkeyConfJson.size(); i++)
			{
				const Json::Value &hotkeyJson = hotkeyConfJson[i];
				class HotKeyConf &cfg = *iter++;
				if (hotkeyJson.type() == Json::objectValue)
				{
					cfg.keyCode = hotkeyJson.get("keyCode", cfg.keyCode.c_str()).asString().c_str();
					int id = vkey_find(cfg.keyCode.c_str());
					if (id >= 0)
					{
						cfg.vkCode = vkey_list[id].vkey;
					}
					else
					{
						cfg.vkCode = -1;
					}
					cfg.modifiers = hotkeyJson.get("modifiers", cfg.modifiers).asInt();
					cfg.global = hotkeyJson.get("global", cfg.global).asBool();
					const Json::Value &action = hotkeyJson["action"];
					cfg.action.type = static_cast<Action::Type>(action.get("type", cfg.action.type).asInt());
					cfg.action.id = action.get("id", cfg.action.id).asInt();
				}
			}
		}
	}	

	return 0;
}

int Settings::Write(AnsiString asFileName)
{
	Json::Value root;
	Json::StyledWriter writer("\t");

	root["frmMain"]["AppWidth"] = frmMain.iWidth;
	root["frmMain"]["AppHeight"] = frmMain.iHeight;
	root["frmMain"]["AppPositionX"] = frmMain.iPosX;
	root["frmMain"]["AppPositionY"] = frmMain.iPosY;
	root["frmMain"]["Maximized"] = frmMain.bWindowMaximized;
	root["frmMain"]["AlwaysOnTop"] = frmMain.bAlwaysOnTop;
	root["frmMain"]["ExitFullScreenOnStop"] = frmMain.bExitFullScreenOnStop;

	root["Logging"]["LogToFile"] = Logging.bLogToFile;
	root["Logging"]["Flush"] = Logging.bFlush;
	root["Logging"]["MaxFileSize"] = Logging.iMaxFileSize;
	root["Logging"]["MaxUiLogLines"] = Logging.iMaxUiLogLines;

	{
		Json::Value &jv = root["Mplayer"];
		jv["Instance"] = Mplayer.asInstance;
		jv["SoftVol"] =  Mplayer.softVol;
		jv["SoftVolMax"] =  Mplayer.softVolMax;
		jv["SoftVolLevel"] = Mplayer.softVolLevel;
		jv["OsdLevel"] = Mplayer.osdLevel;
		jv["ShowFileNameOnPlayStart"] = Mplayer.showFileNameOnPlayStart;
	}

	{
		Json::Value &jv = root["MediaBrowser"];
		jv["LastPlaylist"] = mediaBrowser.asLastPlaylist;
	}

	{
		int i = 0;
		std::list<HotKeyConf>::iterator iter;
		for (iter = hotKeyConf.begin(); iter != hotKeyConf.end(); ++iter)
		{
			struct HotKeyConf &cfg = *iter;
			Json::Value &cfgJson = root["hotkeyConf"][i++];
			cfgJson["keyCode"] = cfg.keyCode.c_str();
			cfgJson["modifiers"] = cfg.modifiers;
			cfgJson["global"] = cfg.global;
			cfgJson["action"]["type"] = cfg.action.type;
			cfgJson["action"]["id"] = cfg.action.id;
		}
	}	

	std::string outputConfig = writer.write( root );

	try
	{
		std::ofstream ofs(asFileName.c_str());
		ofs << outputConfig;
		ofs.close();
	}
	catch(...)
	{
    	return 1;
	}

	return 0;
}


