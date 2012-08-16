# Build Debug and Release versions of all the games

GAMES = Crafty McrView SeaOfMemes DontHitMe

all: $(GAMES)

$(GAMES):
	@xcodebuild -project ../$@/BuildOSX/$@.xcodeproj -configuration Debug
	@xcodebuild -project ../$@/BuildOSX/$@.xcodeproj -configuration Release

