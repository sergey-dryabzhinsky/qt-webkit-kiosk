== Qt Webkit Kiosk ==

=== About ===

This is simple browser application written on Qt & QtWebEngine, and is a dirty hackport of Sergey Dryabzhinsky's much more stable qt-webkit-kiosk.

It requires Qt5.4, as this is the first version of Qt to officially include QtWebEngine.

Usualy runing in fullscreen mode, but supports maximized and fixed size window mode.

This browser uses only one application window. So, no popups, no plugins in separate processes, like Chrome do.

Supports several parameters via configuration file: proxy, user-agent, click sound.

Also supports hiding printer dialog and uses default or defined printer.

=== Commandline usage ===

For version 1.04.04 , 1.05.05

Usage: 

 --help -h                       Print usage and exit
 --version -V                    Print version and exit
 --config options.ini            Configuration INI-file
 --uri http://www.example.com/   Open this URI
 --clear-cache -C                Clear cached request data

=== Hot keys ===

 Key	 Action

Ctrl+Q	 Close program
F5	 Reload page
Ctrl+R	 Reload page with cache clean
F11	 Toggle fullscreen mode

- added in version 1.05.10, 1.04.07

Up	 Scroll Up
Down	 Scroll Down
PgUp	 Scroll Page Up
PgDown	 Scroll Page Down
Home	 Scroll to top
End	 Scroll to bottom

Ctrl+Home,
HomePage	Load home page
Backspace       Go back via pages history
