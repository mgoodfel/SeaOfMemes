DEMOS = GuiForms GuiTestAll GuiTestGL GuiTestSDL GuiTestWin Landscape SaucerMovie TestCube Trees
GAMES = Crafty McrView SeaOfMemes DontHitMe newCrafty WorldClient WorldServer
MILLIGRAM = JpegLib FreeType LibPNG Ogg Vorbis ZLib \
	mg2D mg3D mgGUI mgPlatform mgUtil \
	mgSendLog mgLogServer

all:   libs $(MILLIGRAM) $(DEMOS) $(GAMES)

#
# erase existing libraries
#
libs:
	-rmdir /s /q ..\Milligram\BuildWin\Debug
	-rmdir /s /q ..\Milligram\BuildWin\DebugMemory
	-rmdir /s /q ..\Milligram\BuildWin\Release
	-rmdir /s /q ..\Milligram\BuildWin\Distrib
#
# Rebuild the Milligram libraries
#
$(MILLIGRAM): 
	-rmdir /s /q ..\Milligram\$*\BuildWin\Debug
	-rmdir /s /q ..\Milligram\$*\BuildWin\DebugMemory
	-rmdir /s /q ..\Milligram\$*\BuildWin\Release
	-rmdir /s /q ..\Milligram\$*\BuildWin\Distrib
	-rmdir /s /q ..\Milligram\$*\BuildWin\ipch
	-erase ..\Milligram\$*\BuildWin\*.sdf

#
# Rebuild the Demos
#
$(DEMOS): 
	-rmdir /s /q ..\Demos\$*\BuildWin\Debug
	-rmdir /s /q ..\Demos\$*\BuildWin\DebugMemory
	-rmdir /s /q ..\Demos\$*\BuildWin\Release
	-rmdir /s /q ..\Demos\$*\BuildWin\Distrib
	-rmdir /s /q ..\Demos\$*\BuildWin\ipch
	-erase ..\Demos\$*\BuildWin\*.sdf
#
# Rebuild the Distrib version of the games
#
$(GAMES):
	-rmdir /s /q ..\Games\$*\BuildWin\Debug
	-rmdir /s /q ..\Games\$*\BuildWin\DebugMemory
	-rmdir /s /q ..\Games\$*\BuildWin\Release
	-rmdir /s /q ..\Games\$*\BuildWin\Distrib
	-rmdir /s /q ..\Games\$*\BuildWin\ipch
	-erase ..\Games\$*\BuildWin\*.sdf
