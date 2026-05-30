CC := clang
CFLAGS := -Wall -Wextra -g -fblocks
LDFLAGS :=

UID := $(shell id -u)
PLIST := com.geoffrey4444.minimal.plist
LAUNCH_AGENTS := $(HOME)/Library/LaunchAgents
INSTALLED_PLIST := $(LAUNCH_AGENTS)/$(PLIST)

all: server client

server: server.c
	$(CC) $(CFLAGS) -o server server.c $(LDFLAGS)

client: client.c
	$(CC) $(CFLAGS) -o client client.c $(LDFLAGS)

install: all
	mkdir -p $(LAUNCH_AGENTS)
	cp $(PLIST) $(INSTALLED_PLIST)
	launchctl bootstrap gui/$(UID) $(INSTALLED_PLIST)

uninstall:
	-launchctl bootout gui/$(UID) $(INSTALLED_PLIST)
	-rm -f $(INSTALLED_PLIST)

reinstall: uninstall install

status:
	launchctl print gui/$(UID)/com.geoffrey4444.minimal

clean: uninstall
	rm -f server client
