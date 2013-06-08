TARGET = libmgGUI.a

FORM = $(addprefix Form/, mgBlockTag.o mgBreakTag.o mgButtonTag.o mgCheckboxTag.o \
	mgConsoleTag.o mgFieldTag.o mgFormPane.o mgFormParser.o mgFormTag.o \
	mgHeadingTag.o mgLabelTag.o mgListItemTag.o mgListTag.o mgParaTag.o \
	mgPreTag.o mgRuleTag.o mgTableColTag.o mgTableRowTag.o mgTableTag.o \
	mgTextTag.o )

GUI = $(addprefix GUI/, mgColumnLayout.o mgControl.o mgFrame.o mgRectFrame.o mgSolidPaint.o \
	mgStyle.o mgTableLayout.o mgTopControl.o )

TEXTFORM = $(addprefix TextFormatter/, mgTextBuffer.o mgTextDraw.o mgTextFormat.o \
	mgTextMeasure.o mgTextScan.o)

SimpleSTYLE = $(addprefix SimpleStyle/, mgSimpleButton.o mgSimpleCheckbox.o mgSimpleConsole.o \
	mgSimpleDesktop.o mgSimpleField.o mgSimpleLabel.o mgSimpleList.o mgSimpleMenuBar.o \
	mgSimpleScrollbar.o mgSimpleScrollPane.o mgSimpleSplit.o mgSimpleStack.o \
	mgSimpleStyle.o mgSimpleTabbed.o mgSimpleWindow.o )

UTIL = Util/mgDebugPane.o 

DIRLIST = Form GUI TextFormatter SimpleStyle Util
FILES = $(FORM) $(GUI) $(TEXTFORM) $(SimpleSTYLE) $(UTIL) 

SOURCE = ../Source
INCDIRS = -I/usr/include -I/usr/local/include -I/user/include/GL -I./ -I$(SOURCE) -I../Include -I../../
DEFINE = 


CC = g++
RELEASEFLAGS = -O3
DEBUGFLAGS = -g3 -O0 -fno-inline 

LIBS = -lm -lrt -lpthread
LIBDIRS = #empty
DEBUGTARGET = ../../BuildLinux/Debug/$(TARGET)
RELEASETARGET = ../../BuildLinux/Release/$(TARGET)
 
all: debug release

debug: debugdirs $(DEBUGTARGET)

release: releasedirs $(RELEASETARGET)

DEBUGFILES = $(addprefix Debug/, $(FILES))
RELEASEFILES = $(addprefix Release/, $(FILES))

#directories
debugdirs:
	@mkdir -p ../../BuildLinux/Debug
	@mkdir -p Debug $(addprefix Debug/, $(DIRLIST))

releasedirs:
	@mkdir -p ../../BuildLinux/Release
	@mkdir -p Release $(addprefix Release/, $(DIRLIST))

# compilation rules
Debug/%.o: $(SOURCE)/%.c
	@echo "$(CC) -o $@"
	@$(CC) $(DEBUGFLAGS) $(INCDIRS) $(DEFINE) -c $< -o $@

Debug/%.o: $(SOURCE)/%.cpp
	@echo "$(CC) -o $@"
	@$(CC) $(DEBUGFLAGS) $(INCDIRS) $(DEFINE) -c $< -o $@

Release/%.o: $(SOURCE)/%.c
	@echo "$(CC) -o $@"
	@$(CC) $(RELEASEFLAGS) $(INCDIRS) $(DEFINE) -c $< -o $@

Release/%.o: $(SOURCE)/%.cpp
	@echo "$(CC) -o $@"
	@$(CC) $(RELEASEFLAGS) $(INCDIRS) $(DEFINE) -c $< -o $@

# build the library
$(DEBUGTARGET): $(DEBUGFILES)
	@echo "Link $@"
	@-rm -f $(DEBUGTARGET)
	@ar -cvq $(DEBUGTARGET) $(DEBUGFILES)

$(RELEASETARGET): $(RELEASEFILES)
	@echo "Link $@"
	@-rm -f $(RELEASETARGET)
	@ar -cvq $(RELEASETARGET) $(RELEASEFILES)

clean: 
	-rm -f -r Debug
	-rm -f -r Release


