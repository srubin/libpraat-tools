PREFIX=/usr/local/bin
LIBPRAAT=/Users/srubin/code/libpraat

INCLUDES=-I$(LIBPRAAT) -L$(LIBPRAAT) -I$(LIBPRAAT)/sys -I$(LIBPRAAT)/NUM -I$(LIBPRAAT)/stat -I$(LIBPRAAT)/LPC -I$(LIBPRAAT)/fon  -I$(LIBPRAAT)/dwsys
CPPFLAGS=-lpraat -framework CoreFoundation -framework ApplicationServices -framework Foundation -framework AppKit
BUILD=build
ARCH=x86_64
CC=g++


all: directories $(BUILD)/praatpitch $(BUILD)/praatresynth $(BUILD)/praatfeatures

install: all
	cp $(BUILD)/praatpitch $(PREFIX)/.
	cp $(BUILD)/praatresynth $(PREFIX)/.
	cp $(BUILD)/praatfeatures $(PREFIX)/.

clean:
	rm -f $(BUILD)/praatpitch $(BUILD)/praatresynth $(BUILD)/praatfeatures
	rm -rf $(BUILD)

directories: $(BUILD)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/praatpitch: praatpitch.cpp
	$(CC) praatpitch.cpp -o $(BUILD)/praatpitch -arch $(ARCH) -std=c++11 $(INCLUDES) $(CPPFLAGS)

$(BUILD)/praatresynth: praatresynth.cpp
	$(CC) praatresynth.cpp -o $(BUILD)/praatresynth -arch $(ARCH) -std=c++11 $(INCLUDES) $(CPPFLAGS)

$(BUILD)/praatfeatures: praatfeatures.cpp
	$(CC) praatfeatures.cpp -o $(BUILD)/praatfeatures -arch $(ARCH) -std=c++11 $(INCLUDES) $(CPPFLAGS)

.PHONY: clean

.PHONY: all

.PHONY: install