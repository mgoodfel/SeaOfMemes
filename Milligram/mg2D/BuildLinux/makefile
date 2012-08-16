TARGET = libmg2D.a 

GRAPHICS2D = mgColor.o

GENERIC2D = Generic2D/mgGenBrush.o Generic2D/mgGenContext.o Generic2D/mgGenFont.o \
	Generic2D/mgGenIcon.o Generic2D/mgGenImage.o Generic2D/mgGenPen.o Generic2D/mgGenCharCache.o \
	Generic2D/mgGenSurface.o

FILES = $(GRAPHICS2D) $(GENERIC2D)
DIRLIST = Generic2D

DEFINE = 
SOURCE = ../Source
INCDIRS = -I/usr/include -I/usr/local/include -I/user/include/GL -I./ -I$(SOURCE) -I../Include -I../../ \
	-I../../FreeType/Include

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


