TARGET = libmgGUI.a

FORM = Form/mgBlockTag.o Form/mgBreakTag.o Form/mgButtonTag.o Form/mgCheckboxTag.o \
	Form/mgConsoleTag.o Form/mgFieldTag.o Form/mgFormPane.o Form/mgFormParser.o Form/mgFormTag.o \
	Form/mgHeadingTag.o Form/mgLabelTag.o Form/mgListItemTag.o Form/mgListTag.o Form/mgParaTag.o \
	Form/mgPreTag.o Form/mgRuleTag.o Form/mgTableColTag.o Form/mgTableRowTag.o Form/mgTableTag.o \
	Form/mgTextTag.o 

GUI = GUI/mgColumnLayout.o GUI/mgControl.o GUI/mgFrame.o GUI/mgRectFrame.o GUI/mgSolidPaint.o \
	GUI/mgStyle.o GUI/mgTableLayout.o GUI/mgTopControl.o

TEXTFORM = TextFormatter/mgTextBuffer.o TextFormatter/mgTextDraw.o TextFormatter/mgTextFormat.o \
	TextFormatter/mgTextMeasure.o TextFormatter/mgTextScan.o

UGLYSTYLE = UglyStyle/mgUglyButton.o UglyStyle/mgUglyCheckbox.o UglyStyle/mgUglyConsole.o \
	UglyStyle/mgUglyField.o UglyStyle/mgUglyLabel.o UglyStyle/mgUglyList.o \
	UglyStyle/mgUglyScrollbar.o UglyStyle/mgUglyStack.o UglyStyle/mgUglyStyle.o

UTIL = Util/mgDebugPane.o 

DIRLIST = Form GUI TextFormatter UglyStyle Util
FILES = $(FORM) $(GUI) $(TEXTFORM) $(UGLYSTYLE) $(UTIL) 

SOURCE = ../Source
INCDIRS = -I/usr/include -I/usr/local/include -I/user/include/GL -I./ -I$(SOURCE) -I../Include -I../../
DEFINE = 


CC = g++
RELEASEFLAGS = -O3
DEBUGFLAGS = -g3 -O0 -fno-inline 

LIBS = -LX11 -lGL -lm -lrt -lpthread
LIBDIRS = -L/usr/local/lib
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


