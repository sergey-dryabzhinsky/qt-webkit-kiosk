Qt WebKit Kiosk-mode browser
===============

This is simple browser application written on Qt &amp; QtWebkit.

Qt versions `4.[78]` and `5.[2-5]` are supported.

Note: if you use libqt5webkit5-dev (>= 5.212~) package - you should build project with Qt >= 5.7.

Usualy runing in fullscreen mode, but supports maximized and fixed size window mode.

This browser uses only one application window. So, no popups, no plugins in separate processes, like Chromium do. And no tabs.

Support several parameters via configuration file: proxy, user-agent, click sound, plugins, etc.

Also hides printer dialog, uses default or defined printer, adds custom js/css files to every loaded page, disables text selection.

## TODO

- Support QtWebEngine instead of QtWebkit.

## Downloads

Downloads now on public [gDrive Folder](https://drive.google.com/folderview?id=0B6CU04AyADvoV19PMlhJSVA2TDQ&usp=sharing).

For Windows available [Qt bundles](https://drive.google.com/folderview?id=0B6CU04AyADvoXzUxdW5KeEt5cW8&usp=sharing) public folder. It needed by project installer without Qt.

All "installers" are just [7-zip](https://7-zip.org) self-extracting archives.

### Launchpad

- Main PPA: ppa:sergey-dryabzhinsky/qt-webkit-kiosk
- Additional: ppa:sergey-dryabzhinsky/qt48
