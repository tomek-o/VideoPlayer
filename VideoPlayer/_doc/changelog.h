/** \file
	\brief OVCP changelog
*/

/** \page changelog Changelog

Version 1.1
- FIXED: cached files list keeps previously selected and focused item after refreshing and sorting
- added "Go to file" and "Copy file to..." to context menu of opera cached files list
- Enter used to stop playing (or start if used on cached files list)
- added button toggling fullscreen mode (duplicating 'F' shortcut)
- added "Minimize" button 
- file dropped to window or passed by command line goes to "Other" tab and is played immediately

Version 1.2
- clicking on the player screen with the right mouse button toggles OSD level (same as with MPlayer) 
- FIXED: play button stayed down when clicked but no file was selected
- added "Pause" button
- added mplayer output log window (see "Settings")
- FIXED: possible hangup when playing damaged files (caused by not handling mplayer output pipe)
- FIXED: some annoying behavior related to toolbar auto hiding 
- most mplayer-style hotkeys are active only during playback (problems when entering filename to "Other" tab")

Version 1.3
- removed borlndmm.dll and cc3270mt.dll dependencies
- changed Escape action: if application is maximized it goes to normal mode, otherwise it is closing
- added option to automatically exit fullscreen when playing is stopped

Version 1.4
- FIXED: control toolbar not showing sometimes after switching to fullscreen
- changed MPlayer key color to one less likely to cause collision (video visible through other window on top of player)
- set *.flv as default extension when copying files from cache
- added option to enable MPlayer software volume amplification (for quiet movie files), default up to 200% 

Version 1.5
- software-controlled volume level is stored in settings

Version 1.6
- default extension for saved files changed to mp4
- added audio/video bitrate display to main window

Version 2.0
- added mp4 extension to open file dialog

- aplication name changed to VideoPlayer
- removed functionality related to Opera (v12) cache
- added playlists

- if single file is passed from command line: search for previous same entry on playlist before adding new entry (e.g. when player would be started multiple time with same source file or rtsp stream)
- if TEdit filter control is not focused "F"/"f" key works also when stopped (toggling fullscreen)
- added "About..." button to settings window

- settings window: close with Esc, changed window position to main form center
- OSD: show file name => 'i' key
- fluent fast forwarding - reading file length and current position
- added "Remove duplicates from list"
- added "Remove missing files from list"

Version 2.2
- Ctrl+D: delete selected files (if stopped) or currently playing file
- 'p' during playback: go to previous file (if position < 10 seconds) or go to beginning of the current file
- added "Mark" function
- added function marking files that are possible duplicates - by size
- added file length to playlists
- fixed issue with flickering at startup if multiple playlists are loaded
- delaying load of playlists until user switches to particular playlist
- remembering last playlist selected on restart
- added common.lib
- hotkeys are now configurable; previous fixed hotkeys are now default configuration
- added "show log" action type for hotkeys
- mplayer: use pausing_keep when changing volume and for OSD commands
- treat mplayer configuration path as relative to VideoPlayer.exe by default, as absolute if mplayer executable was not found there

Version 2.3
- added "Hide playlist" function
- added "Go to file" menu item 
- settings: added option to add extra parameters for mplayer (added for every file after default parameters)
- playlists can hold extra mplayer parameters for each file, added at the end of mplayer command line
- log: show mplayer command line
- default configuration: "L" as shortcut to "show log"
- default configuration: try also relative mplayer\mplayer.exe path if this file exists
- add setting for control panel position: top (default) or bottom
- changed default softvol amplification to 200% and volume to 50

Version 2.4
- FIXED default playlist selection when playlist are hidden
- FIXED "Rename playlist" for newly created playlist (not saved yet)
- added option to store separate volume setting for each file from playlist
- store and show video and audio bitrates for playlist entries (files can be softed by bitrate)
- FIXED Access Violation when unhiding playlist
- saving modified playlists with 120 s timer event (not only on exit) to prevent possible playlist loss in case of e.g. crash/reboot
- files dropped to playlist are now not played automatically (to avoid unintentional loss of previously played file bookmark)
*/

