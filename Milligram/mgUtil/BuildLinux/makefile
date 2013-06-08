TARGET = libmgUtil.a

SOURCE = ../Source
INCDIRS = -I/usr/include -I/usr/local/include -I./ -I../../ -I../Include -I$(SOURCE) 
DEFINE =

UTIL = $(addprefix Util/, mgBlockPool.o mgDebug.o mgDWordArray.o mgException.o \
	mgMapDWordToPtr.o mgMapDWordToDWord.o mgMapPtrToPtr.o mgMapStringToPtr.o \
	mgMapStringToString.o mgMatrix.o mgOptionsFile.o mgPtrArray.o mgString.o \
	mgStringArray.o mgMapStringToDWord.o mgMapXYZToPtr.o mgErrorMsg.o mgErrorTable.o \
	mgUtilErrorTable.o mgMapStringToDouble.o)

IMAGEUTIL = $(addprefix ImageUtil/, mgDiffuse.o mgGIF3.o mgImageIO.o mgImageUtil.o mgLZW.o)

GRAPHICSUTIL = $(addprefix GraphicsUtil/, mgSimplexNoise.o mgBezier.o)

AUDIO = $(addprefix Audio/, mgVorbisRead.o mgAudio.o mgAudioAL.o)

XMLPARSER = $(addprefix XMLParser/, mgXMLParser.o mgXMLScanner.o mgXMLTag.o)

LINUXOS = $(addprefix OS/Linux/, mgLinuxEvent.o mgLinuxFileUtil.o mgLinuxLock.o \
	mgLinuxMisc.o mgLinuxThread.o mgLinuxTimer.o mgLinuxSendLog.o)

GENERICOS = $(addprefix OS/, mgOSCreate.o mgMiscUtil.o) 

FILES = $(UTIL) $(IMAGEUTIL) $(GRAPHICSUTIL) $(AUDIO) $(XMLPARSER) $(LINUXOS) $(GENERICOS)
DIRLIST = Util ImageUtil GraphicsUtil Audio XMLParser OS OS/Linux

# Build Debug and Release library
CC = g++
RELEASEFLAGS = -O3
DEBUGFLAGS = -g3 -O0 -fno-inline 

LIBS = -lm -lrt -lpthread
LIBDIRS = #empty
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


