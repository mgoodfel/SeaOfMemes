# Build Debug and Release versions of all the games

GAMES = Crafty McrView SeaOfMemes DontHitMe

all: $(GAMES)

$(GAMES):
	msbuild ../$*/BuildWin/$*.vcxproj /p:configuration=Debug
	msbuild ../$*/BuildWin/$*.vcxproj /p:configuration=Release
