TARGET = libJpegLib.a

SOURCE = ../Source
INCDIRS = -I/usr/include -I/usr/local/include -I$(SOURCE) -I../Include

FILES = jcapimin.o jcapistd.o jccoefct.o jccolor.o \
	jcdctmgr.o jchuff.o   jcinit.o   jcmainct.o \
	jcmarker.o jcmaster.o jcomapi.o  jcparam.o \
	jcphuff.o  jcprepct.o jcsample.o jctrans.o \
	jdapimin.o jdapistd.o jdatadst.o jdatasrc.o \
	jdcoefct.o jdcolor.o  jddctmgr.o jdhuff.o \
	jdinput.o  jdmainct.o jdmarker.o jdmaster.o \
	jdmerge.o  jdphuff.o  jdpostct.o jdsample.o \
	jdtrans.o  jerror.o   jfdctflt.o jfdctfst.o \
	jfdctint.o jidctflt.o jidctfst.o jidctint.o \
	jidctred.o jmemmgr.o  jmemnobs.o jquant1.o \
	jquant2.o  jutils.o   rdbmp.o    rdgif.o

DIRLIST = #empty

# Build Debug and Release library
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


