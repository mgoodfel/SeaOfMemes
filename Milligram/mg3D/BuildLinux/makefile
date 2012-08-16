TARGET = libmg3D.a
SOURCE = ../Source
INCDIRS = -I/usr/include -I/usr/local/include -I/user/include/GL -I./ -I$(SOURCE) -I../Include -I../../
DEFINE = -DGLEW_STATIC

ROOT = mgDisplayServices.o

UTIL3D = Util3D/mgCursorDefn.o Util3D/mg3DErrorTable.o Util3D/mgFontList.o

GRAPHICS3D = Graphics3D/mgTextureSurface.o Graphics3D/mgIndexBuffer.o \
	Graphics3D/mgVertexBuffer.o Graphics3D/mgVertexTypes.o

GL21 = Graphics3D/GL21/mgGL21IndexBuffer.o Graphics3D/GL21/mgGL21Misc.o Graphics3D/GL21/mgGL21Services.o \
	Graphics3D/GL21/mgGL21VertexBuffer.o

GL33 = Graphics3D/GL33/mgGL33IndexBuffer.o Graphics3D/GL33/mgGL33Misc.o Graphics3D/GL33/mgGL33Services.o \
	Graphics3D/GL33/mgGL33VertexBuffer.o

FILES = $(ROOT) $(UTIL3D) $(GRAPHICS3D) $(GL21) $(GL33)
DIRLIST = Util3D Graphics3D Graphics3D/GL21 Graphics3D/GL33

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


