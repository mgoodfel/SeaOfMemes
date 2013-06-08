TARGET = libmgGUI.a

SOURCE = ../Source
INCDIRS = -I./ -I../../ -I../Include -I$(SOURCE)
DEFINE = -DEMSCRIPTEN -U__unix__

FORM = $(addprefix Form/, mgBlockTag.bc mgBreakTag.bc mgButtonTag.bc mgCheckboxTag.bc \
	mgConsoleTag.bc mgFieldTag.bc mgFormPane.bc mgFormParser.bc mgFormTag.bc \
	mgHeadingTag.bc mgLabelTag.bc mgListItemTag.bc mgListTag.bc mgParaTag.bc \
	mgPreTag.bc mgRuleTag.bc mgTableColTag.bc mgTableRowTag.bc mgTableTag.bc \
	mgTextTag.bc )

GUI = $(addprefix GUI/, mgColumnLayout.bc mgControl.bc mgFrame.bc mgRectFrame.bc mgSolidPaint.bc \
	mgStyle.bc mgTableLayout.bc mgTopControl.bc )

TEXTFORM = $(addprefix TextFormatter/, mgTextBuffer.bc mgTextDraw.bc mgTextFormat.bc \
	mgTextMeasure.bc mgTextScan.bc)

SimpleSTYLE = $(addprefix SimpleStyle/, mgSimpleButton.bc mgSimpleCheckbox.bc mgSimpleConsole.bc \
	mgSimpleDesktop.bc mgSimpleField.bc mgSimpleLabel.bc mgSimpleList.bc mgSimpleMenuBar.bc \
	mgSimpleScrollbar.bc mgSimpleScrollPane.bc mgSimpleSplit.bc mgSimpleStack.bc \
	mgSimpleStyle.bc mgSimpleTabbed.bc mgSimpleWindow.bc )

UTIL = Util/mgDebugPane.bc 

DIRLIST = Form GUI TextFormatter SimpleStyle Util
FILES = $(FORM) $(GUI) $(TEXTFORM) $(SimpleSTYLE) $(UTIL) 


CC = emcc
RELEASEFLAGS = #-O3
DEBUGFLAGS = #-g3 -O0 -fno-inline 

LIBS = #-lm -lrt -lpthread
LIBDIRS = #empty
DEBUGTARGET = ../../BuildScript/Debug/$(TARGET)
RELEASETARGET = ../../BuildScript/Release/$(TARGET)
 
all: debug release

debug: debugdirs $(DEBUGTARGET)

release: releasedirs $(RELEASETARGET)

DEBUGFILES = $(addprefix Debug/, $(FILES))
RELEASEFILES = $(addprefix Release/, $(FILES))

#directories
debugdirs:
	@mkdir -p ../../BuildScript/Debug
	@mkdir -p Debug $(addprefix Debug/, $(DIRLIST))

releasedirs:
	@mkdir -p ../../BuildScript/Release
	@mkdir -p Release $(addprefix Release/, $(DIRLIST))

# compilation rules
Debug/%.bc: $(SOURCE)/%.c
	@echo "$(CC) -o $@"
	@$(CC) $(DEBUGFLAGS) $(INCDIRS) $(DEFINE) -c $< -o $@

Debug/%.bc: $(SOURCE)/%.cpp
	@echo "$(CC) -o $@"
	@$(CC) $(DEBUGFLAGS) $(INCDIRS) $(DEFINE) -c $< -o $@

Release/%.bc: $(SOURCE)/%.c
	@echo "$(CC) -o $@"
	@$(CC) $(RELEASEFLAGS) $(INCDIRS) $(DEFINE) -c $< -o $@

Release/%.bc: $(SOURCE)/%.cpp
	@echo "$(CC) -o $@"
	@$(CC) $(RELEASEFLAGS) $(INCDIRS) $(DEFINE) -c $< -o $@

# build the library
$(DEBUGTARGET): $(DEBUGFILES)
	@echo "Link $@"
	@-rm -f $(DEBUGTARGET)
	@llvm-ar cvq $(DEBUGTARGET) $(DEBUGFILES)

$(RELEASETARGET): $(RELEASEFILES)
	@echo "Link $@"
	@-rm -f $(RELEASETARGET)
	@llvm-ar cvq $(RELEASETARGET) $(RELEASEFILES)

clean: 
	-rm -f -r Debug
	-rm -f -r Release


