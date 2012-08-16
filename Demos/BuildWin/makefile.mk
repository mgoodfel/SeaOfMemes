# Build Debug and Release versions of all the demos

DEMOS = GuiTestAll GuiTestGL GuiTestWin Landscape SaucerMovie TestCube Trees

all: $(DEMOS)

$(DEMOS):
	msbuild ../$*/BuildWin/$*.vcxproj /p:configuration=Debug
	msbuild ../$*/BuildWin/$*.vcxproj /p:configuration=Release
