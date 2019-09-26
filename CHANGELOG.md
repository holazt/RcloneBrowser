# Change Log

## [1.5.2] - 2019-09-26
- code cleanup - clean compilation with -Werror enabled, GCC8 compilation

## [1.5.1] - 2019-09-25
- after task edit initiated by double click main window does not get proper focus back and subsequent Run click might lead to wrong task execution. For time being I disable double click edit - until proper fix is produced.

## [1.5] - 2019-09-25
- tasks - jobs can be saved/edited/run/deleted. No need to created the same job again and again.
- on Google drive DriveSharedWithMe can be mounted to local filesystem
- DriveSharedWithMe checkbox is now disabled for non Google destinations - it is Google only feature and turning it on for other destinations does not make sense - could even crash the browser.
- verbose option is now always on and has been removed from UI - which means that stats will be always displayed. No more wondering how long it is going to take for some long job to finish.
- fixed an issue with local remote on Windows when local drive content was not properly displayed
- replaced remote Amazon icon with generic S3 one. S3 became name on its own and almost de-facto standard in cloud access used by many rclone supported destinations
- new application logo

## [1.4.1] - 2019-09-18
- small GUI tweaks to make all progress fields always visible (they were too small for large transfers) and adjust some screen sizes to make all GUI elements visible
- update all builds with latest Qt (5.13.1)

## [1.4] - 2019-08-23
- Fix compliation errors and update all builds with latest Qt (5.13)
- Fix Config button command
- Further fix and tweak progress display. Add ETA and Total Size fields
- Fix remotes icons display
- Add sftp icon
- Fix progress display for rclone > 1.37 (by DinCahill)
- Add a Public Link option to the right-click menu (by DinCahill)
- Add preference: Show hidden files and folders (by DinCahill)
- Add Mega icon (by DinCahill)
- Refresh when Shared is toggled (by DinCahill)
- Disable Upload button for Shared (by DinCahill)
- Support for shared Google Drive files. Enable the checkbox when you open a remote, and all rclone commands will be passed --drive-shared-with-me (by DinCahill)
- Set cache mode for mounts (by DinCahill)
- Fixed missing leading / in path (required for some SFTP servers) (by DinCahill)

## [1.2] - 2017-03-11
- Calculate size of folders, issue #4
- Copy transfer command to clipboard, issue #20
- Support custom .rclone.conf location, #21
- Export list of files, issue #27
- Bugfix for folder refresh not working after rename, issue #30
- Remember empty text fields in transfer dialog, issue #32
- Error message when too old rclone version is selected
- Support portable mode, issue #28
- Create .deb packages, issue #26

## [1.1] - 2017-01-31
- Added `--transfer` option in UI, issue #1
- Supports encrypted `.rclone.conf` configuration file, issue #2
- Fixed crash when canceling active stream
- Added ETA tooltip for transfer progress bars
- Allow to specify extra arguments for rclone, issue #7
- Fix for browsing Hubic remotes, issue #10
- Support high-dpi mode for macOS

## [1.0.0] - 2017-01-29
- Allows to browse and modify any rclone remote, including encrypted ones
- Uses same configuration file as rclone, no extra configuration required
- Simultaneously navigate multiple repositories in separate tabs
- Lists files hierarchically with file name, size and modify date
- All rclone commands are executed asynchronously, no freezing GUI
- File hierarchy is lazily cached in memory, for faster traversal of folders
- Allows to upload, download, create new folders, rename or delete files and folders
- Can process multiple upload or download jobs in background
- Drag & drop support for dragging files from local file explorer for uploading
- Streaming media files for playback in player like mpv or similar
- Mount and unmount folders on macOS and GNU/Linux
- Optionally minimizes to tray, with notifications when upload/download finishes

[1.5.2]: https://github.com/kapitainsky/RcloneBrowser/releases/tag/1.5.2
[1.5.1]: https://github.com/kapitainsky/RcloneBrowser/releases/tag/1.5.1
[1.5]: https://github.com/kapitainsky/RcloneBrowser/releases/tag/1.5
[1.4.1]: https://github.com/kapitainsky/RcloneBrowser/releases/tag/1.4.1
[1.4]: https://github.com/kapitainsky/RcloneBrowser/releases/tag/1.4
[1.2]: https://github.com/mmozeiko/RcloneBrowser/releases/tag/1.2
[1.1]: https://github.com/mmozeiko/RcloneBrowser/releases/tag/1.1
[1.0.0]: https://github.com/mmozeiko/RcloneBrowser/releases/tag/1.0.0
