
INCLUDES=-I/Users/srubin/code/libpraat -L/Users/srubin/code/libpraat -I/Users/srubin/code/libpraat/sys -I/Users/srubin/code/libpraat/NUM -I/Users/srubin/code/libpraat/stat -I/Users/srubin/code/libpraat/LPC -I/Users/srubin/code/libpraat/fon -lpraat -framework CoreFoundation -framework ApplicationServices -framework Foundation -framework AppKit -I/Users/srubin/code/libpraat/dwsys

ARCH=x86_64

all: clean praatpitch praatresynth

clean:
	rm -f praatpitch praatresynth

praatpitch:
	g++ praatpitch.cpp -o praatpitch -arch $(ARCH) -std=c++11 $(INCLUDES)

praatresynth:
	g++ praatresynth.cpp -o praatresynth -arch $(ARCH) -std=c++11 $(INCLUDES)