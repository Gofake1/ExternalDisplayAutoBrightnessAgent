# David Wu, 1 Nov. 2017

CCFLAGS=-Wall
executable_name=net.gofake1.ExternalDisplayAutoBrightnessAgent
executable_prefix=/usr/local/libexec/
plist_name=net.gofake1.ExternalDisplayAutoBrightnessAgent.plist
plist_prefix=~/Library/LaunchAgents/
frameworks=-framework CoreFoundation -framework CoreGraphics \
	-framework IOKit

all: externaldisplayautobrightnessagent

externaldisplayautobrightnessagent: main.c common.c ddc.c display.c
	$(CC) $(CCFLAGS) $(frameworks) $^ -o $(executable_name)

clean:
	rm $(executable_name)

install:
	mkdir -p $(executable_prefix)
	install -s $(executable_name) $(executable_prefix)

install_plist:
	cp $(plist_name) $(plist_prefix)

uninstall:
	rm $(executable_prefix)$(executable_name)

uninstall_plist:
	rm $(plist_prefix)$(plist_prefix)
	
run:
	launchctl load $(plist_prefix)$(plist_name)

stop:
	launchctl unload $(plist_prefix)$(plist_name)