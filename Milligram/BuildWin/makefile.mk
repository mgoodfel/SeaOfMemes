# Build Debug and Release versions of libraries

LIBS = mg2D mg3D mgGUI mgPlatform mgUtil JpegLib LibPNG Ogg Vorbis ZLib FreeType 

all: $(LIBS)

$(LIBS):
	msbuild ../$*/BuildWin/$*.vcxproj /p:configuration=Debug
	msbuild ../$*/BuildWin/$*.vcxproj /p:configuration=Release
