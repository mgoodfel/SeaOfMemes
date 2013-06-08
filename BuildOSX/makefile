PARTXX = part83
DEMOS = Landscape SaucerMovie TestCube Trees GuiTestAll 
GAMES = Crafty McrView SeaOfMemes DontHitMe
MILLIGRAM = JpegLib LibPNG FreeType Ogg Vorbis ZLib mg2D mg3D mgGUI mgUtil mgPlatform mgSendLog 

all: MakeDirs $(MILLIGRAM) $(DEMOS) $(GAMES) MakeZips

#
# Create the distribution directories
#
MakeDirs: 
	@-rm -rf Publish
	@mkdir -p Publish/Demos
	@mkdir -p Publish/Games

#
# Rebuild the Milligram libraries
#
$(MILLIGRAM): 
	@xcodebuild -project ../Milligram/$@/BuildOSX/$@.xcodeproj -configuration Release clean
	@xcodebuild -project ../Milligram/$@/BuildOSX/$@.xcodeproj -configuration Release
	@xcodebuild -project ../Milligram/$@/BuildOSX/$@.xcodeproj -configuration Debug clean
	@xcodebuild -project ../Milligram/$@/BuildOSX/$@.xcodeproj -configuration Debug
#
# Rebuild the Demos
#
$(DEMOS): 
	@xcodebuild -project ../Demos/$@/BuildOSX/$@.xcodeproj -configuration Release clean
	@xcodebuild -project ../Demos/$@/BuildOSX/$@.xcodeproj -configuration Release
	@xcodebuild -project ../Demos/$@/BuildOSX/$@.xcodeproj -configuration Debug clean
	@xcodebuild -project ../Demos/$@/BuildOSX/$@.xcodeproj -configuration Debug
	mkdir Publish/Demos/$@
	-cp -r ../Demos/$@/BuildOSX/Release/$@.app Publish/Demos/$@
	-cp ../Demos/$@/options.xml Publish/Demos/$@
	-cp -r ../Demos/$@/docs Publish/Demos/$@/docs

#
# Rebuild the Distrib version of the games
#
$(GAMES):
	@xcodebuild -project ../Games/$@/BuildOSX/$@.xcodeproj -configuration Release clean
	@xcodebuild -project ../Games/$@/BuildOSX/$@.xcodeproj -configuration Release
	@xcodebuild -project ../Games/$@/BuildOSX/$@.xcodeproj -configuration Debug clean
	@xcodebuild -project ../Games/$@/BuildOSX/$@.xcodeproj -configuration Debug
	mkdir Publish/Games/$@
	-cp -r ../Games/$@/BuildOSX/Release/$@.app Publish/Games/$@
	-cp ../Games/$@/options.xml Publish/Games/$@
	-cp -r ../Games/$@/docs Publish/Games/$@/docs
#
# Create the game zip file
#
	-cp ../Milligram/mgSendLog/BuildOSX/Release/mgSendLog Publish/Games/$@
	-cp ../logoptions.xml Publish/Games/$@ 
	-cd Publish/Games/$@ ; rm ../../LetsCode_$(PARTXX)_$@_mac.zip ; zip -r ../../LetsCode_$(PARTXX)_$@_mac.zip *
	-rm Publish/Games/$@/logoptions.xml
	-rm Publish/Games/$@/mgSendLog
#
# Create single zip file for all demos 
#
MakeZips:
	-cp ../Milligram/mgSendLog/BuildOSX/Release/mgSendLog Publish/Demos
	-cp ../logoptions.xml Publish/Demos 
	-cd Publish/Demos ; rm ../LetsCode_$(PARTXX)_Demos_mac.zip ; zip -r ../LetsCode_$(PARTXX)_demos_mac.zip *
	-rm Publish/Demos/logoptions.xml
	-rm Publish/Demos/mgSendLog
#
# Create zip file for all executables
#
	-cp ../Milligram/mgSendLog/BuildOSX/Release/mgSendLog Publish
	-cp ../logoptions.xml Publish
	-cd Publish; rm LetsCode_$(PARTXX)_All_mac.zip ; zip -r LetsCode_$(PARTXX)_all_mac.zip Demos Games mgSendLog logoptions.xml

clean:
	-rm -rf Publish
	-rm -rf ../Milligram/BuildOSX/Debug
	-rm -rf ../Milligram/BuildOSX/Release
	-rm -rf $(addprefix ../Milligram/FreeType/BuildOSX/,build Debug Release DerivedData) 
	-rm -rf $(addprefix ../Milligram/JpegLib/BuildOSX/,build Debug Release DerivedData) 
	-rm -rf $(addprefix ../Milligram/LibPNG/BuildOSX/,build Debug Release DerivedData) 
	-rm -rf $(addprefix ../Milligram/Ogg/BuildOSX/,build Debug Release DerivedData) 
	-rm -rf $(addprefix ../Milligram/Vorbis/BuildOSX/,build Debug Release DerivedData) 
	-rm -rf $(addprefix ../Milligram/ZLib/BuildOSX/,build Debug Release DerivedData) 
	-rm -rf $(addprefix ../Milligram/FreeType/BuildOSX/,build Debug Release DerivedData) 
#
	-rm -rf $(addprefix ../Milligram/mgSendLog/BuildOSX/,build Debug Release DerivedData) 
#
	-rm -rf $(addprefix ../Milligram/mgUtil/BuildOSX/,build Debug Release DerivedData) 
	-rm -rf $(addprefix ../Milligram/mgPlatform/BuildOSX/,build Debug Release DerivedData) 
	-rm -rf $(addprefix ../Milligram/mg2D/BuildOSX/,build Debug Release DerivedData) 
	-rm -rf $(addprefix ../Milligram/mg3D/BuildOSX/,build Debug Release DerivedData) 
	-rm -rf $(addprefix ../Milligram/mgGUI/BuildOSX/,build Debug Release DerivedData) 
#
	-rm -rf $(addprefix ../Demos/TestCube/BuildOSX/,build Debug Release DerivedData) 
	-rm -rf $(addprefix ../Demos/Trees/BuildOSX/,build Debug Release DerivedData) 
	-rm -rf $(addprefix ../Demos/Landscape/BuildOSX/,build Debug Release DerivedData) 
	-rm -rf $(addprefix ../Demos/SaucerMovie/BuildOSX/,build Debug Release DerivedData) 
	-rm -rf $(addprefix ../Demos/GuiTestAll/BuildOSX/,build Debug Release DerivedData) 
	-rm -rf $(addprefix ../Demos/GuiTestGL/BuildOSX/,build Debug Release DerivedData) 
#
	-rm -rf $(addprefix ../Games/Crafty/BuildOSX/,build Debug Release DerivedData) 
	-rm -rf $(addprefix ../Games/McrView/BuildOSX/,build Debug Release DerivedData) 
	-rm -rf $(addprefix ../Games/SeaOfMemes/BuildOSX/,build Debug Release DerivedData) 
	-rm -rf $(addprefix ../Games/DontHitMe/BuildOSX/,build Debug Release DerivedData) 
	
