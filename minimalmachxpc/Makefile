CC := clang
CFLAGS := -Wall -Wextra -g -fblocks
LDFLAGS :=
SWIFTC := swiftc
SWIFTFLAGS :=

UID := $(shell id -u)
PLIST := com.geoffrey4444.minimal.plist
SWIFTPLIST := com.geoffrey4444.minimalswift.plist
LAUNCH_AGENTS := $(HOME)/Library/LaunchAgents
INSTALLED_PLIST := $(LAUNCH_AGENTS)/$(PLIST)
INSTALLED_SWIFTPLIST := $(LAUNCH_AGENTS)/$(SWIFTPLIST)

all: server client swift-server swift-client

swift-server: server.swift
	$(SWIFTC) $(SWIFTFLAGS) -o swift-server server.swift

swift-client: client.swift
	$(SWIFTC) $(SWIFTFLAGS) -o swift-client client.swift

server: server.c
	$(CC) $(CFLAGS) -o server server.c $(LDFLAGS)

client: client.c
	$(CC) $(CFLAGS) -o client client.c $(LDFLAGS)

install: all
	mkdir -p $(LAUNCH_AGENTS)
	cp $(PLIST) $(INSTALLED_PLIST)
	launchctl bootstrap gui/$(UID) $(INSTALLED_PLIST)
	cp $(SWIFTPLIST) $(INSTALLED_SWIFTPLIST)
	launchctl bootstrap gui/$(UID) $(INSTALLED_SWIFTPLIST)

uninstall:
	-launchctl bootout gui/$(UID) $(INSTALLED_PLIST)
	-launchctl bootout gui/$(UID) $(INSTALLED_SWIFTPLIST)
	-rm -f $(INSTALLED_PLIST)
	-rm -f $(INSTALLED_SWIFTPLIST)

reinstall: uninstall install

status:
	launchctl print gui/$(UID)/com.geoffrey4444.minimal
	launchctl print gui/$(UID)/com.geoffrey4444.minimalswift

clean: uninstall
	rm -f server client swift-server swift-client
