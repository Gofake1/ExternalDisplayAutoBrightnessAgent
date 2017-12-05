# ExternalDisplayAutoBrightnessAgent
Automatically manage brightness of displays connected to your Mac

### What It Does
This daemon syncs external monitors with the brightness level of your Mac's built-in display. The goal is to effectively create a single brightness level to worry about, rather than manually keep multiple brightnesses in sync.

Macs without built-in displays are not supported.
External monitors must have native macOS support or implement [DDC](https://en.wikipedia.org/wiki/Display_Data_Channel).

By default, the daemon updates every thirty seconds; if you want to change this, modify ```kTimerRate``` in ```main.c```. You can tweak ```kExternalDisplayListMaxSize``` to lower memory usage.

### Install
```
make install
make install_plist
make run
```

Manual: Move the executable to ```/usr/local/libexec```, and the plist to ```~/Library/LaunchAgents```. Run
```
launchctl load ~/Library/LaunchAgents/net.gofake1.ExternalDisplayAutoBrightnessAgent.plist
```
to start the daemon.

### Build
Requires macOS, a C compiler, and ```make```. Tested on macOS 10.13.

### Issues
Report DDC issues to [ddcctl](https://github.com/kfix/ddcctl).
```/usr/local/var/log/net.gofake1.ExternalDisplayAutoBrightnessAgent.log``` contains warnings and errors that would be helpful in bug reports.

### Acknowledgments
Authors of [brightness](https://github.com/nriley/brightness).

Authors of [ddcctl](https://github.com/kfix/ddcctl).

*This project is available under the MIT License.*