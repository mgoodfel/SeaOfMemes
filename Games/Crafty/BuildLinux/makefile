TARGET = Crafty

SOURCE = ../Source
INCDIRS = -I/usr/include -I/usr/local/include -I/user/include/GL -I./ -I$(SOURCE) -I../../../Milligram/

CRAFTY = Crafty.o StarrySky.o VertexSky.o MovementApp.o HelpUI.o

BRICKS = Bricks/BrickBlob.o Bricks/BrickBlobInt.o Bricks/BrickBlobFloat.o \
	Bricks/BrickSet.o  Bricks/BrickSetFile.o Bricks/ShapeFile.o \
	Bricks/VertexCubeInt.o Bricks/VertexShapeInt.o \
	Bricks/VertexCubeFloat.o Bricks/VertexShapeFloat.o

CHUNKS = Chunks/ChunkList.o Chunks/ChunkObj.o Chunks/ChunkWorld.o 

FILES = $(CRAFTY) $(BRICKS) $(CHUNKS) 
DIRLIST = Bricks Chunks

# Build Milligram Application
CC = g++
RELEASEFLAGS = -O3 -Wno-invalid-offsetof
DEBUGFLAGS = -g3 -O0 -fno-inline -Wno-invalid-offsetof

LIBS = -lX11 -lGL -lm -lrt -lpthread
DEBUGLIBDIRS = -L../../../Milligram/BuildLinux/Debug
RELEASELIBDIRS = -L../../../Milligram/BuildLinux/Release
FRAMEWORK = -lmg3D -lmgPlatform -lmg2D -lmgGUI -lmgUtil -lJpegLib -lLibPNG -lZLib -lFreeType


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
	@$(CC) $(DEBUGLIBDIRS) -o Debug/$(TARGET) $(DEBUGFILES) $(FRAMEWORK) $(LIBS)

Release/$(TARGET): $(RELEASEFILES)
	@echo "Link $@"
	@$(CC) $(RELEASELIBDIRS) -o Release/$(TARGET) $(RELEASEFILES) $(FRAMEWORK) $(LIBS)

clean: 
	-rm -r -f Debug
	-rm -r -f Release


