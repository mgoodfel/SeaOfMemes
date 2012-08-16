# Build Debug and Release versions of libraries

LIBS = JpegLib Ogg Vorbis ZLib FreeType mgUtil mg2D mg3D mgGUI mgPlatform  #LibPNG

all: dirs $(LIBS)

dirs: 
	-mkdir Debug
	-mkdir Release
  
$(LIBS):
	@xcodebuild -project ../$@/BuildOSX/$@.xcodeproj -configuration Debug
	@xcodebuild -project ../$@/BuildOSX/$@.xcodeproj -configuration Release
