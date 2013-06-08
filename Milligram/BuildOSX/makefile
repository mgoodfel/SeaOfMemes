# Build Debug and Release versions of libraries

LIBS = JpegLib LibPNG Ogg Vorbis ZLib FreeType mgUtil mg2D mg3D mgGUI mgPlatform 

all: dirs $(LIBS)

dirs: 
	-mkdir Debug
	-mkdir Release
  
$(LIBS):
	@xcodebuild -project ../$@/BuildOSX/$@.xcodeproj -configuration Debug
	@xcodebuild -project ../$@/BuildOSX/$@.xcodeproj -configuration Release
