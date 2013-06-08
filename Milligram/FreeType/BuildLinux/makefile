TARGET = libFreeType.a

BASE = base/ftbase.o base/ftbdf.o base/ftbitmap.o \
	base/ftcid.o base/ftdebug.o base/ftfstype.o \
	base/ftgasp.o base/ftglyph.o base/ftgxval.o \
	base/ftinit.o base/ftlcdfil.o base/ftmm.o \
	base/ftotval.o base/ftpatent.o base/ftpfr.o \
	base/ftstroke.o base/ftsynth.o base/ftsystem.o \
	base/fttype1.o base/ftwinfnt.o base/ftxf86.o

RASTER = raster/raster.o

SMOOTH = smooth/smooth.o

TRUETYPE = truetype/truetype.o

SFNT = sfnt/sfnt.o

FILES = $(BASE) $(RASTER) $(SMOOTH) $(TRUETYPE) $(SFNT)
DIRLIST = base raster smooth truetype sfnt

SOURCE = ../Source
INCDIRS = -I/usr/include -I/usr/local/include -I./ -I$(SOURCE) -I../Include
DEFINE = -DFT2_BUILD_LIBRARY

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


