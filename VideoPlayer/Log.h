/** \file
 *  \brief Global logging unit
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

#ifndef LogH
#define LogH
#include <string>
#include "common/singleton.h"

/** \brief Log detail level
*/
enum E_LOGLEVEL
{
	E_LOG_NONE = 0,
	E_LOG_TRACE,
	E_LOG_ALL
};

/** \brief Global logger
*/
class CLog: public CSingleton<CLog>
{
public:
    /** \brief Log formatted text with timestamp */
	void log(char *lpData, ...);
	/** \brief Select output log file
	 *  \return zero on no error
	 */
	int SetFile(std::string);
	void LogToFile(bool);
	/** \brief Set log detail level / disable logging */
	void SetLevel(int);
	/** \brief OnLog callback declaration
	*/
	typedef void (__closure *CallbackLog)(char *buf);
	/** \brief Callback called when new log line is added
	 *
	 *  If set, this callback is called when new log line is added. This can
	 *  be used to update user interface (i.e. add some items to TListView)
	 *  when log is written by various decoupled units (with no knowledge
	 *  about user interface)     .
	 */
	CallbackLog callbackLog;
private:
	CLog();
	//~CLog() {};
	friend CSingleton<CLog>;
	std::string sFile;
	bool bLogToFile;
	int iLogLevel;
};

/** \brief Macro to avoid unnecessary typing
*/
#define LOG CLog::Instance()->log

#endif
