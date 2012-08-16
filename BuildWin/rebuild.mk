DEMOS = GuiTestAll GuiTestGL GuiTestWin Landscape SaucerMovie TestCube Trees
GAMES = Crafty McrView SeaOfMemes DontHitMe
MILLIGRAM = JpegLib FreeType LibPNG Ogg Vorbis ZLib mg2D mg3D mgGUI mgPlatform mgUtil mgSendLog

all: $(MILLIGRAM) $(DEMOS) $(GAMES)

#
# Rebuild the Milligram libraries
#
$(MILLIGRAM): 
	msbuild ..\Milligram\$*/BuildWin/$*.vcxproj /p:configuration=Debug
	msbuild ..\Milligram\$*/BuildWin/$*.vcxproj /p:configuration=Release
	msbuild ..\Milligram\$*/BuildWin/$*.vcxproj /p:configuration=DebugMemory
	msbuild ..\Milligram\$*/BuildWin/$*.vcxproj /p:configuration=Distrib
#
# Rebuild the Demos
#
$(DEMOS): 
	msbuild ..\Demos\$*\BuildWin\$*.vcxproj /p:configuration=Debug
	msbuild ..\Demos\$*\BuildWin\$*.vcxproj /p:configuration=Release
	msbuild ..\Demos\$*\BuildWin\$*.vcxproj /p:configuration=DebugMemory
	msbuild ..\Demos\$*\BuildWin\$*.vcxproj /p:configuration=Distrib
#
# Rebuild the Distrib version of the games
#
$(GAMES):
	msbuild ..\Games\$*\BuildWin\$*.vcxproj /p:configuration=Debug
	msbuild ..\Games\$*\BuildWin\$*.vcxproj /p:configuration=Release
	msbuild ..\Games\$*\BuildWin\$*.vcxproj /p:configuration=DebugMemory
	msbuild ..\Games\$*\BuildWin\$*.vcxproj /p:configuration=Distrib
