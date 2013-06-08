TARGET = libmg3D.a

SOURCE = ../Source
INCDIRS = -I./ -I../../ -I../Include -I$(SOURCE) 
DEFINE = -DEMSCRIPTEN -U__unix__

ROOT = mgDisplayServices.bc

UTIL3D = $(addprefix Util3D/, mgCursorDefn.bc mg3DErrorTable.bc mgFontList.bc)

GRAPHICS3D = $(addprefix Graphics3D/, mgIndexBuffer.bc mgVertexBuffer.bc mgVertexTypes.bc)

GLSURFACE = $(addprefix Graphics3D/GLTextureSurface/, mgGLTextureSurface.bc) 

WEBGL = $(addprefix Graphics3D/WebGL/, mgWebGLIndexBuffer.bc mgWebGLMisc.bc mgWebGLDisplay.bc \
	mgWebGLVertexBuffer.bc)

FILES = $(ROOT) $(UTIL3D) $(GRAPHICS3D) $(WEBGL) $(GLSURFACE)
DIRLIST = Util3D Graphics3D Graphics3D/WebGL Graphics3D/GLTextureSurface

# Build Debug and Release library
CC = emcc
RELEASEFLAGS = #-O3
DEBUGFLAGS = #-g3 -O0 -fno-inline 

LIBS = #-lX11 -lGL -lm -lrt -lpthread
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


