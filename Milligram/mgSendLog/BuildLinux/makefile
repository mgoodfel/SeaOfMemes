TARGET = mgSendLog

SOURCE = ../Source
INCDIRS = -I/usr/include -I/usr/local/include -I/user/include/GL -I./ -I$(SOURCE) -I../../../Milligram/

FILES = mgSendLog.o
DIRLIST = #empty

# Build Milligram Application
CC = g++
RELEASEFLAGS = -O3
DEBUGFLAGS = -g3 -O0 -fno-inline 

LIBS = -LX11 -lGL -lm -lrt -lpthread
DEBUGLIBDIRS = -L/usr/local/lib -L../../../Milligram/BuildLinux/Debug
RELEASELIBDIRS = -L/usr/local/lib -L../../../Milligram/BuildLinux/Release
FRAMEWORK = -lmgUtil


all: debug release

debug: debugdirs Debug/$(TARGET)

release: releasedirs Release/$(TARGET)

DEBUGFILES = $(addprefix Debug/, $(FILES))
RELEASEFILES = $(addprefix Release/, $(FILES))

#directories
debugdirs:
	@mkdir -p Debug $(addprefix Debug/, $(DIRLIST))

releasedirs:
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

Debug/$(TARGET): $(DEBUGFILES)
	@echo "Link $@"
	@$(CC) $(DEBUGLIBDIRS) $(LIBS) -o Debug/$(TARGET) $(DEBUGFILES) $(FRAMEWORK)

Release/$(TARGET): $(RELEASEFILES)
	@echo "Link $@"
	@$(CC) $(RELEASELIBDIRS) $(LIBS) -o Release/$(TARGET) $(RELEASEFILES) $(FRAMEWORK)

clean: 
	-rm -r -f Debug
	-rm -r -f Release


