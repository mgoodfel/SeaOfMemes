TARGET = libFreeType.a

BASE = $(addprefix base/, ftbase.bc ftbdf.bc ftbitmap.bc \
	ftcid.bc ftdebug.bc ftfstype.bc \
	ftgasp.bc ftglyph.bc ftgxval.bc \
	ftinit.bc ftlcdfil.bc ftmm.bc \
	ftotval.bc ftpatent.bc ftpfr.bc \
	ftstroke.bc ftsynth.bc ftsystem.bc \
	fttype1.bc ftwinfnt.bc ftxf86.bc )

RASTER = raster/raster.bc

SMOOTH = smooth/smooth.bc

TRUETYPE = truetype/truetype.bc

SFNT = sfnt/sfnt.bc

FILES = $(BASE) $(RASTER) $(SMOOTH) $(TRUETYPE) $(SFNT)
DIRLIST = base raster smooth truetype sfnt

SOURCE = ../Source
INCDIRS = -I./ -I../../ -I../Include -I$(SOURCE) 
DEFINE = -DEMSCRIPTEN -U__unix__ -DFT2_BUILD_LIBRARY

CC = emcc
RELEASEFLAGS = #-O3
DEBUGFLAGS = #-g3 -O0 -fno-inline 

LIBS = #-lm -lrt -lpthread
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


