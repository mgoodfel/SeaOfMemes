TARGET = libmgUtil.a

SOURCE = ../Source
INCDIRS = -I./ -I../../ -I../Include -I$(SOURCE) 
DEFINE = -DEMSCRIPTEN -U__unix__

UTIL = $(addprefix Util/, mgBlockPool.bc mgDebug.bc mgDWordArray.bc mgException.bc \
	mgMapDWordToPtr.bc mgMapDWordToDWord.bc mgMapPtrToPtr.bc mgMapStringToPtr.bc \
	mgMapStringToString.bc mgMatrix.bc mgOptionsFile.bc mgPtrArray.bc mgString.bc \
	mgStringArray.bc mgMapStringToDWord.bc mgMapXYZToPtr.bc mgErrorMsg.bc mgErrorTable.bc \
	mgUtilErrorTable.bc mgMapStringToDouble.bc)

AUDIO = $(addprefix Audio/, mgAudio.bc mgAudioScript.bc)

GRAPHICSUTIL = $(addprefix GraphicsUtil/, mgSimplexNoise.bc mgBezier.bc)

XMLPARSER = $(addprefix XMLParser/, mgXMLParser.bc mgXMLScanner.bc mgXMLTag.bc)

SCRIPTOS = $(addprefix OS/Script/, mgScriptEvent.bc mgScriptFileUtil.bc mgScriptLock.bc \
	mgScriptMisc.bc mgScriptThread.bc mgScriptTimer.bc)

GENERICOS = $(addprefix OS/, mgOSCreate.bc mgMiscUtil.bc) 

FILES = $(UTIL) $(GRAPHICSUTIL) $(AUDIO) $(XMLPARSER) $(SCRIPTOS) $(GENERICOS)
DIRLIST = Util GraphicsUtil XMLParser Audio OS OS/Script

# Build Debug and Release library
CC = emcc
RELEASEFLAGS = #-O2 
DEBUGFLAGS = #-g3 -O0 -fno-inline 

LIBS = #empty
LIBDIRS = #empty
DEBUGTARGET = ../../BuildScript/Debug/$(TARGET)
RELEASETARGET = ../../BuildScript/Release/$(TARGET)
 
all: debug release

debug: debugdirs $(DEBUGTARGET)

release: releasedirs $(RELEASETARGET)

DEBUGFILES = $(addprefix Debug/, $(FILES))
RELEASEFILES = $(addprefix Release/, $(FILES))

#directories
debugdirs:
	@mkdir -p ../../BuildScript/Debug
	@mkdir -p Debug $(addprefix Debug/, $(DIRLIST))

releasedirs:
	@mkdir -p ../../BuildScript/Release
	@mkdir -p Release $(addprefix Release/, $(DIRLIST))

# compilation rules
Debug/%.bc: $(SOURCE)/%.c
	@echo "$(CC) -o $@"
	@$(CC) $(DEBUGFLAGS) $(INCDIRS) $(DEFINE) -c $< -o $@

Debug/%.bc: $(SOURCE)/%.cpp
	@echo "$(CC) -o $@"
	@$(CC) $(DEBUGFLAGS) $(INCDIRS) $(DEFINE) -c $< -o $@

Release/%.bc: $(SOURCE)/%.c
	@echo "$(CC) -o $@"
	@$(CC) $(RELEASEFLAGS) $(INCDIRS) $(DEFINE) -c $< -o $@

Release/%.bc: $(SOURCE)/%.cpp
	@echo "$(CC) -o $@"
	@$(CC) $(RELEASEFLAGS) $(INCDIRS) $(DEFINE) -c $< -o $@

# build the library
$(DEBUGTARGET): $(DEBUGFILES)
	@echo "Link $@"
	@-rm -f $(DEBUGTARGET)
	@llvm-ar cvq $(DEBUGTARGET) $(DEBUGFILES)

$(RELEASETARGET): $(RELEASEFILES)
	@echo "Link $@"
	@-rm -f $(RELEASETARGET)
	@llvm-ar cvq $(RELEASETARGET) $(RELEASEFILES)

clean: 
	-rm -f -r Debug
	-rm -f -r Release


