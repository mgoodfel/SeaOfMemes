TARGET = McrView

BRICKS = $(addprefix Bricks/, BrickBlob.bc BrickBlobFloat.bc BrickSet.bc BrickSetFile.bc \
	ShapeFile.bc VertexCubeFloat.bc VertexShapeFloat.bc)
CHUNKS = $(addprefix Chunks/, ChunkList.bc ChunkObj.bc ChunkWorld.bc)
MINECRAFT = $(addprefix Minecraft/, MinecraftChunk.bc MinecraftFile.bc MinecraftRegion.bc NBTFile.bc)
ROOT = McrView.bc HelpUI.bc MovementApp.bc StarrySky.bc VertexSky.bc
FILES = $(ROOT) $(BRICKS) $(CHUNKS) $(MINECRAFT)

DIRLIST = Bricks Chunks Minecraft
 
CC = emcc
LIBS = 

DEBUGLIBDIRS = -L../../../Milligram/BuildScript/Debug
RELEASELIBDIRS = -L../../../Milligram/BuildScript/Release
FRAMEWORK = -lmg3D -lmgPlatform -lmg2D -lmgGUI -lmgUtil -lFreeType 

SOURCE = ../Source
INCDIRS = -I./ -I../../../Milligram/ -I$(SOURCE) 
DEFINE= -DEMSCRIPTEN -U__unix__ -Wno-invalid-offsetof


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

Debug/$(TARGET): $(DEBUGFILES)
	@echo "Link $@"
	@rm -rf docs options.xml
	@cp -r ../docs docs
	@rm -r docs/shaders/GL21 
	@rm -r docs/shaders/GL33
	@cp ../options.xml options.xml
	@cp ../../../Milligram/mgPlatform/BuildScript/mgAppScript.js $(TARGET).html Debug
	@$(CC) $(DEBUGLIBDIRS) $(DEBUGFLAGS) --bind -s ALLOW_MEMORY_GROWTH=1 \
	-o Debug/$(TARGET).js $(DEBUGFILES) $(FRAMEWORK) $(LIBS) \
	../../../Milligram/mgPlatform/Source/Script/linkage.cpp  \
	--preload-file docs --preload-file options.xml
	@rm -rf docs options.xml

Release/$(TARGET): $(RELEASEFILES)
	@echo "Link $@"
	@rm -rf docs options.xml
	@cp -r ../docs docs 
	@rm -r docs/shaders/GL21 
	@rm -r docs/shaders/GL33
	@cp ../options.xml options.xml
	@cp ../../../Milligram/mgPlatform/BuildScript/mgAppScript.js $(TARGET).html Release
	@$(CC) $(RELEASELIBDIRS) $(RELEASEFLAGS) --bind -O2 -s ALLOW_MEMORY_GROWTH=1 \
	-o Release/$(TARGET).js $(RELEASEFILES) $(FRAMEWORK) $(LIBS) \
	../../../Milligram/mgPlatform/Source/Script/linkage.cpp  \
	--preload-file docs --preload-file options.xml
	@rm -rf docs options.xml

clean: 
	-rm -r -f Debug
	-rm -r -f Release


