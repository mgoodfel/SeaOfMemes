PARTXX = part83
DEMOS = TestCube Trees Landscape SaucerMovie GuiTestAll 
GAMES = Crafty McrView SeaOfMemes DontHitMe
MILLIGRAM = mg2D mg3D mgGUI mgUtil mgPlatform FreeType

all:   MakeDirs $(MILLIGRAM) $(DEMOS) $(GAMES)

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
	@$(MAKE) clean --directory=../Milligram/$@/BuildScript
	@$(MAKE) --directory=../Milligram/$@/BuildScript
#
# Rebuild the Demos
#
$(DEMOS): 
	@$(MAKE) clean --directory=../Demos/$@/BuildScript
	@$(MAKE) --directory=../Demos/$@/BuildScript
	mkdir Publish/Demos/$@
	-cp ../Demos/$@/BuildScript/Release/*.js Publish/Demos/$@
	-cp ../Demos/$@/BuildScript/Release/$@.html Publish/Demos/$@
	-cp ../Demos/$@/BuildScript/Release/$@.data Publish/Demos/$@

#
# Rebuild the Distrib version of the games
#
$(GAMES):
	@$(MAKE) clean --directory=../Games/$@/BuildScript
	@$(MAKE) --directory=../Games/$@/BuildScript
	mkdir Publish/Games/$@
	-cp ../Games/$@/BuildScript/Release/*.js Publish/Games/$@
	-cp ../Games/$@/BuildScript/Release/$@.html Publish/Games/$@
	-cp ../Games/$@/BuildScript/Release/$@.data Publish/Games/$@

clean:
	-rm -rf ../Milligram/BuildScript/Debug
	-rm -rf ../Milligram/BuildScript/Release
	$(MAKE) clean --directory=../Milligram/mgUtil/BuildScript
	$(MAKE) clean --directory=../Milligram/mgPlatform/BuildScript
	$(MAKE) clean --directory=../Milligram/mg3D/BuildScript
	$(MAKE) clean --directory=../Milligram/mg2D/BuildScript
	$(MAKE) clean --directory=../Milligram/mgGUI/BuildScript
#
	$(MAKE) clean --directory=../Demos/TestCube/BuildScript
	$(MAKE) clean --directory=../Demos/Trees/BuildScript
	$(MAKE) clean --directory=../Demos/Landscape/BuildScript
	$(MAKE) clean --directory=../Demos/SaucerMovie/BuildScript
#	$(MAKE) clean --directory=../Demos/GuiTestAll/BuildScript
#	$(MAKE) clean --directory=../Demos/GuiTestGL/BuildScript
#
	$(MAKE) clean --directory=../Games/Crafty/BuildScript
	$(MAKE) clean --directory=../Games/McrView/BuildScript
	$(MAKE) clean --directory=../Games/SeaOfMemes/BuildScript
	$(MAKE) clean --directory=../Games/DontHitMe/BuildScript
	
