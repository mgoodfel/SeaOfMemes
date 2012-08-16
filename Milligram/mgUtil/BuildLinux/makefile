TARGET = libmgUtil.a

SOURCE = ../Source
INCDIRS = -I/usr/include -I/usr/local/include -I./ -I$(SOURCE) -I../Include -I../../
DEFINE =

UTIL = Util/mgBlockPool.o      Util/mgDebug.o \
	Util/mgDWordArray.o     Util/mgException.o         Util/mgMapDWordToPtr.o \
	Util/mgMapPtrToPtr.o    Util/mgMapStringToPtr.o    Util/mgMapStringToString.o \
	Util/mgMatrix.o         Util/mgOptionsFile.o       Util/mgPtrArray.o \
	Util/mgString.o         Util/mgStringArray.o       Util/mgMapStringToDWord.o \
	Util/mgMapXYZToPtr.o    Util/mgErrorMsg.o          Util/mgErrorTable.o  \
	Util/mgUtilErrorTable.o Util/mgMapStringToDouble.o

IMAGEUTIL = ImageUtil/mgDiffuse.o ImageUtil/mgGIF3.o ImageUtil/mgImageIO.o \
	ImageUtil/mgImageUtil.o ImageUtil/mgLZW.o

GRAPHICSUTIL = GraphicsUtil/mgSimplexNoise.o GraphicsUtil/mgBezier.o

AUDIO = Audio/mgVorbisRead.o Audio/mgAudio.o Audio/mgAudioAL.o

XMLPARSER = XMLParser/mgXMLParser.o XMLParser/mgXMLScanner.o XMLParser/mgXMLTag.o

LINUXOS = OS/Linux/mgLinuxEvent.o OS/Linux/mgLinuxFileUtil.o OS/Linux/mgLinuxLock.o \
	OS/Linux/mgLinuxMisc.o OS/Linux/mgLinuxThread.o OS/Linux/mgLinuxTimer.o OS/Linux/mgLinuxSendLog.o

GENERICOS = OS/mgOSCreate.o OS/mgMiscUtil.o 						

FILES = $(UTIL) $(IMAGEUTIL) $(GRAPHICSUTIL) $(AUDIO) $(XMLPARSER) $(LINUXOS) $(GENERICOS)
DIRLIST = Util ImageUtil GraphicsUtil Audio XMLParser OS OS/Linux

# Build Debug and Release library
CC = g++
RELEASEFLAGS = -O3
DEBUGFLAGS = -g3 -O0 -fno-inline 

LIBS = -LX11 -lGL -lm -lrt -lpthread
LIBDIRS = -L/usr/local/lib
DEBUGTARGET = ../../BuildLinux/Debug/$(TARGET)
RELEASETARGET = ../../BuildLinux/Release/$(TARGET)
 
all: debug release

debug: debugdirs $(DEBUGTARGET)

release: releasedirs $(RELEASETARGET)

DEBUGFILES = $(addprefix Debug/, $(FILES))
RELEASEFILES = $(addprefix Release/, $(FILES))

#directories
debugdirs:
	@mkdir -p ../../BuildLinux/Debug
	@mkdir -p Debug $(addprefix Debug/, $(DIRLIST))

releasedirs:
	@mkdir -p ../../BuildLinux/Release
	@mkdir -p Release $(addprefix Release/, $(DIRLIST))

# compilation rules
Debug/%.o: $(SOURCE)/%.c
	@echo "$(CC) -o $@"
	@$(CC) $(DEBUGFLAGS) $(INCDIRS) $(DEFINE) -c $< -o $@

Debug/%.o: $(SOURCE)/%.cpp
	@echo "$(CC) -o $@"
	@$(CC) $(DEBUGFLAGS) $(INCDIRS) $(DEFINE) -c $< -o $@

Release/%.o: $(SOURCE)/%.c
	@echo "$(CC) -o $@"
	@$(CC) $(RELEASEFLAGS) $(INCDIRS) $(DEFINE) -c $< -o $@

Release/%.o: $(SOURCE)/%.cpp
	@echo "$(CC) -o $@"
	@$(CC) $(RELEASEFLAGS) $(INCDIRS) $(DEFINE) -c $< -o $@

# build the library
$(DEBUGTARGET): $(DEBUGFILES)
	@echo "Link $@"
	@-rm -f $(DEBUGTARGET)
	@ar -cvq $(DEBUGTARGET) $(DEBUGFILES)

$(RELEASETARGET): $(RELEASEFILES)
	@echo "Link $@"
	@-rm -f $(RELEASETARGET)
	@ar -cvq $(RELEASETARGET) $(RELEASEFILES)

clean: 
	-rm -f -r Debug
	-rm -f -r Release


