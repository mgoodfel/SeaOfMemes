PARTXX = part83
DEMOS = Landscape SaucerMovie TestCube Trees GuiTestAll #GuiTestGL #GuiTestSDL
GAMES = Crafty McrView SeaOfMemes DontHitMe
MILLIGRAM = JpegLib LibPNG FreeType Ogg Vorbis ZLib mg2D mg3D mgGUI mgUtil mgPlatform mgSendLog

all:   MakeDirs $(MILLIGRAM) $(DEMOS) $(GAMES) MakeZips

#
# Create the distribution directories
#
MakeDirs: 
	@-rm -rf Publish/Demos
	@mkdir -p Publish/Demos
	@-rm -rf Publish/Games
	@mkdir -p Publish/Games

#
# Rebuild the Milligram libraries
#
$(MILLIGRAM): 
	@$(MAKE) clean --directory=../Milligram/$@/BuildLinux
	@$(MAKE) --directory=../Milligram/$@/BuildLinux
#
# Rebuild the Demos
#
$(DEMOS): 
	@$(MAKE) clean --directory=../Demos/$@/BuildLinux
	@$(MAKE) --directory=../Demos/$@/BuildLinux
	mkdir Publish/Demos/$@
	-cp ../Demos/$@/BuildLinux/Release/$@ Publish/Demos/$@
	-cp ../Demos/$@/options.xml Publish/Demos/$@
	-cp -r ../Demos/$@/docs Publish/Demos/$@/docs

#
# Rebuild the Distrib version of the games
#
$(GAMES):
	@$(MAKE) clean --directory=../Games/$@/BuildLinux
	@$(MAKE) --directory=../Games/$@/BuildLinux
	mkdir Publish/Games/$@
	-cp ReadMe.txt Publish/Games/$@
	-cp ../Games/$@/BuildLinux/Release/$@ Publish/Games/$@
	-cp ../Games/$@/options.xml Publish/Games/$@
	-cp -r ../Games/$@/docs Publish/Games/$@/docs
#
# Create the game zip file
#
	-rm Publish/LetsCode_$(PARTXX)_$@_linux.zip
	-cp ../Milligram/mgSendLog/BuildLinux/Release/mgSendLog Publish/Games/$@
	-cp ../logoptions.xml Publish/Games/$@
	-cd Publish/Games/$@ ; zip -r ../../LetsCode_$(PARTXX)_$@_linux.zip *
	-rm Publish/Games/$@/mgSendLog
	-rm Publish/Games/$@/logoptions.xml
	-rm Publish/Games/$@/ReadMe.txt
#
# Create single zip file for all demos 
#
MakeZips:
	-rm Publish/LetsCode_$(PARTXX)_demos_linux.zip
	-cp ReadMe.txt Publish/Demos
	-cp ../Milligram/mgSendLog/BuildLinux/Release/mgSendLog Publish/Demos
	-cp ../logoptions.xml Publish/Demos
	-cd Publish/Demos ; zip -r ../LetsCode_$(PARTXX)_demos_linux.zip *
	-rm Publish/Demos/mgSendLog
	-rm Publish/Demos/logoptions.xml
	-rm Publish/Demos/ReadMe.txt
#
# Create zip file for all executables
#
	-cp ReadMe.txt Publish
	-cp ../Milligram/mgSendLog/BuildLinux/Release/mgSendLog Publish
	-cp ../logoptions.xml Publish
	-cd Publish; rm LetsCode_$(PARTXX)_All_linux.zip ; \
	zip -r LetsCode_$(PARTXX)_all_linux.zip Demos Games mgSendLog logoptions.xml ReadMe.txt

clean:
	-rm -rf ../Milligram/BuildLinux/Debug
	-rm -rf ../Milligram/BuildLinux/Release
	$(MAKE) clean --directory=../Milligram/FreeType/BuildLinux
	$(MAKE) clean --directory=../Milligram/JpegLib/BuildLinux
#	$(MAKE) clean --directory=../Milligram/LibPNG/BuildLinux
	$(MAKE) clean --directory=../Milligram/Ogg/BuildLinux
	$(MAKE) clean --directory=../Milligram/Vorbis/BuildLinux
	$(MAKE) clean --directory=../Milligram/ZLib/BuildLinux
	$(MAKE) clean --directory=../Milligram/mgUtil/BuildLinux
	$(MAKE) clean --directory=../Milligram/mgPlatform/BuildLinux
	$(MAKE) clean --directory=../Milligram/mg3D/BuildLinux
	$(MAKE) clean --directory=../Milligram/mg2D/BuildLinux
	$(MAKE) clean --directory=../Milligram/mgGUI/BuildLinux
	$(MAKE) clean --directory=../Milligram/mgSendLog/BuildLinux
#
	$(MAKE) clean --directory=../Demos/TestCube/BuildLinux
	$(MAKE) clean --directory=../Demos/Trees/BuildLinux
	$(MAKE) clean --directory=../Demos/Landscape/BuildLinux
	$(MAKE) clean --directory=../Demos/SaucerMovie/BuildLinux
	$(MAKE) clean --directory=../Demos/GuiTestAll/BuildLinux
	$(MAKE) clean --directory=../Demos/GuiTestGL/BuildLinux
#
	$(MAKE) clean --directory=../Games/Crafty/BuildLinux
	$(MAKE) clean --directory=../Games/McrView/BuildLinux
	$(MAKE) clean --directory=../Games/SeaOfMemes/BuildLinux
	$(MAKE) clean --directory=../Games/DontHitMe/BuildLinux
	
