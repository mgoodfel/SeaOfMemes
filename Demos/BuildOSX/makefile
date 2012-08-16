# Build Debug and Release versions of all the demos

DEMOS = Landscape SaucerMovie TestCube Trees GuiTestAll GuiTestGL 

all: $(DEMOS)

$(DEMOS):
	@xcodebuild -project ../$@/BuildOSX/$@.xcodeproj -configuration Debug
	@xcodebuild -project ../$@/BuildOSX/$@.xcodeproj -configuration Release

