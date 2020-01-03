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

Version ...
- added mp4 extension to open file dialog

*/

/*
TODO:
- multiselect?
- about button
- disable screensaver while playing
- przy pewnych proporcjach obrazu obraz zas³ania pnlControl ca³kowicie lub czêœciowo
- mplayer pan / wide (zoom: W/E keys)

*/
