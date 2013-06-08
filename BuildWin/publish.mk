PARTXX = part83
DEMOS = GuiTestAll Landscape SaucerMovie TestCube Trees 
GAMES = Crafty McrView SeaOfMemes DontHitMe
MILLIGRAM = JpegLib FreeType LibPNG Ogg Vorbis ZLib mg2D mg3D mgGUI mgPlatform mgUtil mgSendLog

all:   MakeDirs $(MILLIGRAM) $(DEMOS) $(GAMES) MakeZips

#
# Create the distribution directories
#
MakeDirs: 
	-rmdir /S /Q Publish\Milligram
	mkdir Publish\Milligram
	-rmdir /S /Q Publish\Demos
	mkdir Publish\Demos
	-rmdir /S /Q Publish\Games
	mkdir Publish\Games
	-rmdir /S /Q Publish\Source
	mkdir Publish\Source

#
# Rebuild the Milligram libraries
#
$(MILLIGRAM): 
	msbuild ..\Milligram\$*/BuildWin/$*.vcxproj /t:rebuild /p:configuration=Distrib
#
# Extract the Milligram source code and project files
#
	mkdir Publish\Source\Milligram\$*
	-xcopy /q ..\Milligram\$*\*.txt Publish\Source\Milligram\$*
	-erase Source\Milligram\$*\*Todo.txt
	-xcopy /q /e /i ..\Milligram\$*\Source Publish\Source\Milligram\$*\Source
	-xcopy /q /e /i ..\Milligram\$*\Include Publish\Source\Milligram\$*\Include
	-xcopy /q /e /i ..\Milligram\$*\BuildLinux Publish\Source\Milligram\$*\BuildLinux
	-xcopy /q /e /i ..\Milligram\$*\BuildScript Publish\Source\Milligram\$*\BuildScript
	-xcopy /q /e /i ..\Milligram\$*\BuildOSX Publish\Source\Milligram\$*\BuildOSX
	-rmdir /s /q Publish\Source\Milligram\$*\BuildOSX\xcuserdata
	-xcopy /q /h /i ..\Milligram\$*\BuildWin\$*.* Publish\Source\Milligram\$*\BuildWin
	-erase Publish\Source\Milligram\$*\BuildWin\*.sdf
#
# Rebuild the Demos
#
$(DEMOS): 
	msbuild ..\Demos\$*\BuildWin\$*.vcxproj /t:rebuild /p:configuration=Distrib
	mkdir Publish\Demos\$*
	-xcopy ..\Demos\$*\BuildWin\Distrib\$*.exe Publish\Demos\$*
	-xcopy ..\Demos\$*\options.xml Publish\Demos\$*
	-xcopy /q /e /i ..\Demos\$*\docs Publish\Demos\$*\docs
#
# Extract the Demo source code and project files
#
	mkdir Publish\Source\Demos\$*
	-xcopy /q ..\Demos\$*\*.txt Publish\Source\Demos\$*
	-erase Publish\Source\Demos\$*\*Todo.txt
	-erase Publish\Source\Demos\$*\errors.txt
	-xcopy ..\Demos\$*\options.xml Publish\Source\Demos\$*
	-xcopy /q /e /i ..\Demos\$*\docs Publish\Source\Demos\$*\docs
	-xcopy /q /e /i ..\Demos\$*\Source Publish\Source\Demos\$*\Source
	-xcopy /q /e /i ..\Demos\$*\BuildLinux Publish\Source\Demos\$*\BuildLinux
	-xcopy /q /e /i ..\Demos\$*\BuildScript Publish\Source\Demos\$*\BuildScript
	-xcopy /q /e /i ..\Demos\$*\BuildOSX Publish\Source\Demos\$*\BuildOSX
	-rmdir /s /q Publish\Source\Demos\$*\BuildOSX\xcuserdata
	-xcopy /q /h /i ..\Demos\$*\BuildWin\$*.* Publish\Source\Demos\$*\BuildWin
	-erase Publish\Source\Demos\$*\BuildWin\*.sdf

#
# Rebuild the Distrib version of the games
#
$(GAMES):
	msbuild ..\Games\$*\BuildWin\$*.vcxproj /t:rebuild /p:configuration=Distrib
	mkdir Publish\Games\$*
	-xcopy ..\Games\$*\BuildWin\Distrib\$*.exe Publish\Games\$*
	-xcopy ..\Games\$*\options.xml Publish\Games\$*
	-xcopy /q /e /i ..\Games\$*\docs Publish\Games\$*\docs
#
# Create the game zip file
#
        -xcopy ..\Milligram\mgSendLog\BuildWin\Distrib\mgSendLog.exe Publish\Games\$*
        -xcopy ..\logoptions.xml Publish\Games\$*
	chdir Publish\Games\$*
	-erase ..\..\LetsCode_$(PARTXX)_$*_win.zip
	7z a ..\..\LetsCode_$(PARTXX)_$*_win.zip *
	chdir ..\..\..
	-erase Publish\Games\$*\mgSendLog.exe
	-erase Publish\Games\$*\logoptions.xml
#
# Extract the Game source code and project files
#
	mkdir Publish\Source\Games\$*
	-xcopy /q ..\Games\$*\*.txt Publish\Source\Games\$*
	-erase Publish\Source\Games\$*\*Todo.txt
	-erase Publish\Source\Games\$*\errors.txt
#
	-xcopy ..\Games\$*\options.xml Publish\Source\Games\$*
	-xcopy /q /e /i ..\Games\$*\docs Publish\Source\Games\$*\docs
	-xcopy /q /e /i ..\Games\$*\Source Publish\Source\Games\$*\Source
	-xcopy /q /e /i ..\Games\$*\BuildLinux Publish\Source\Games\$*\BuildLinux
	-xcopy /q /e /i ..\Games\$*\BuildScript Publish\Source\Games\$*\BuildScript
	-xcopy /q /e /i ..\Games\$*\BuildOSX Publish\Source\Games\$*\BuildOSX
	-rmdir /s /q Publish\Source\Games\$*\BuildOSX\xcuserdata
	-xcopy /q /h /i ..\Games\$*\BuildWin\$*.* Publish\Source\Games\$*\BuildWin
	-erase Publish\Source\Games\$*\BuildWin\*.sdf
#
# Create single zip file for all demos 
#
MakeZips:
        -xcopy ..\Milligram\mgSendLog\BuildWin\Distrib\mgSendLog.exe Publish\Demos
        -xcopy ..\logoptions.xml Publish\Demos
	chdir Publish\Demos
	-erase ..\LetsCode_$(PARTXX)_demos_win.zip
	7z a ..\LetsCode_$(PARTXX)_demos_win.zip *
	chdir ..\..
	-erase Publish\Demos\mgSendLog.exe
	-erase Publish\Demos\logoptions.xml
#
# Create zip file for all executables
#
        -xcopy /q /y ..\Milligram\mgSendLog\BuildWin\Distrib\mgSendLog.exe Publish
        -xcopy /q /y ..\logoptions.xml Publish
	chdir Publish
	-erase LetsCode_$(PARTXX)_all_win.zip
	7z a LetsCode_$(PARTXX)_all_win.zip Demos Games mgSendLog.exe logoptions.xml
	chdir ..
#
# Create zip file for all source
#
# Copy the publish makefiles
	-xcopy /q /e /i ..\BuildLinux Publish\Source\BuildLinux
	-xcopy /q /e /i ..\BuildScript Publish\Source\BuildScript
	-xcopy /q /e /i ..\BuildOSX Publish\Source\BuildOSX
	-mkdir Publish\Source\BuildWin
	-xcopy /q ..\BuildWin\*.mk Publish\Source\BuildWin
	-xcopy /q ..\BuildWin\*.txt Publish\Source\BuildWin
#
# Copy the section makefiles
#
	-xcopy /q /e /i ..\Demos\BuildLinux Publish\Source\Demos\BuildLinux
	-xcopy /q /e /i ..\Demos\BuildScript Publish\Source\Demos\BuildScript
	-xcopy /q /e /i ..\Demos\BuildOSX Publish\Source\Demos\BuildOSX
	-xcopy /q /e /i ..\Demos\BuildWin Publish\Source\Demos\BuildWin
#
	-xcopy /q /e /i ..\Games\BuildLinux Publish\Source\Games\BuildLinux
	-xcopy /q /e /i ..\Games\BuildScript Publish\Source\Games\BuildScript
	-xcopy /q /e /i ..\Games\BuildOSX Publish\Source\Games\BuildOSX
	-xcopy /q /e /i ..\Games\BuildWin Publish\Source\Games\BuildWin
#
	-mkdir Publish\Source\Milligram\BuildLinux
	-xcopy /q /e /i ..\Milligram\BuildLinux\makefile Publish\Source\Milligram\BuildLinux
	-mkdir Publish\Source\Milligram\BuildScript
	-xcopy /q /e /i ..\Milligram\BuildScript\makefile Publish\Source\Milligram\BuildScript
	-mkdir Publish\Source\Milligram\BuildOSX
	-xcopy /q /e /i ..\Milligram\BuildOSX\makefile Publish\Source\Milligram\BuildOSX
	-mkdir Publish\Source\Milligram\BuildWin
	-xcopy /q /e /i ..\Milligram\BuildWin\makefile.mk Publish\Source\Milligram\BuildWin
#
# create the source zip
#
        -xcopy /q /y ..\logoptions.xml Publish\Source
	chdir Publish\Source
	-erase ..\LetsCode_$(PARTXX)_all_source.zip
	7z a ..\LetsCode_$(PARTXX)_all_source.zip *
	chdir ..\..

