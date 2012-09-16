DEMOS = GuiForms GuiTestAll GuiTestGL GuiTestSDL GuiTestWin \
	Landscape SaucerMovie TestCube Trees BVHView

GAMES = Crafty McrView SeaOfMemes DontHitMe NewCrafty WorldClient WorldServer

MILLIGRAM = JpegLib FreeType LibPNG Ogg Vorbis ZLib \
	mg2D mg3D mgGUI mgPlatform mgUtil \
	mgSendLog mgLogServer

all:   $(MILLIGRAM) $(DEMOS) $(GAMES)

#
# clean the Milligram libraries
#
$(MILLIGRAM): 
	-rmdir /s /q ..\Milligram\$*\BuildWin\Debug
	-rmdir /s /q ..\Milligram\$*\BuildWin\DebugMemory
	-rmdir /s /q ..\Milligram\$*\BuildWin\Release
	-rmdir /s /q ..\Milligram\$*\BuildWin\Distrib
	-rmdir /s /q ..\Milligram\$*\BuildWin\ipch
	-erase ..\Milligram\$*\BuildWin\*.sdf

#
# clean the Demos
#
$(DEMOS): 
	-rmdir /s /q ..\Demos\$*\BuildWin\Debug
	-rmdir /s /q ..\Demos\$*\BuildWin\DebugMemory
	-rmdir /s /q ..\Demos\$*\BuildWin\Release
	-rmdir /s /q ..\Demos\$*\BuildWin\Distrib
	-rmdir /s /q ..\Demos\$*\BuildWin\ipch
	-erase ..\Demos\$*\BuildWin\*.sdf
#
# clean the Games
#
$(GAMES):
	-rmdir /s /q ..\Games\$*\BuildWin\Debug
	-rmdir /s /q ..\Games\$*\BuildWin\DebugMemory
	-rmdir /s /q ..\Games\$*\BuildWin\Release
	-rmdir /s /q ..\Games\$*\BuildWin\Distrib
	-rmdir /s /q ..\Games\$*\BuildWin\ipch
	-erase ..\Games\$*\BuildWin\*.sdf
