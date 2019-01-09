#	Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved
#
#	$Header: /cygdrive/k/CVSRoot/LedIt/Makefile,v 1.13 2003/06/24 14:09:28 lewis Exp $
#
#	$Log: Makefile,v $
#	Revision 1.13  2003/06/24 14:09:28  lewis
#	fix makefile to include SpelledIt\Dictionary folder (SPR#1538)
#	
#	Revision 1.12  2003/03/20 16:03:32  lewis
#	SPR#1360- add IdleManager
#	
#	Revision 1.11  2003/02/24 18:24:49  lewis
#	SPR#1303- must define qWideCharacters/qSingleByteCharacters instead of using qLed_CharacterSet
#	
#	Revision 1.10  2003/01/12 18:41:45  lewis
#	Added BiDiLayoutEngine.o
#	
#	Revision 1.9  2002/09/21 22:31:00  lewis
#	add SimpleTextStore (since depended on now by WordProcessor)
#	
#	Revision 1.8  2002/09/09 03:11:39  lewis
#	Added SpellCheckEngine and SpellCheckEngine_Basic
#	
#	Revision 1.7  2002/05/06 21:30:55  lewis
#	<========================================= Led 3.0.1 Released ========================================>
#	
#	Revision 1.6  2001/11/27 00:28:07  lewis
#	<=============== Led 3.0 Released ===============>
#	
#	Revision 1.5  2001/08/30 01:01:18  lewis
#	*** empty log message ***
#	
#	Revision 1.4  2001/08/27 16:09:48  lewis
#	-O4 instead of -O2 for RELEASE build
#	
#	Revision 1.3  2001/05/18 21:07:49  lewis
#	turn on OPT for NODEBUG build. Add SRCDIR to include path (resources). Added LedStdDialogs.o.
#	New app targets names and dirs for objs (sep for UNICODE and DEBUG/NODEBUG)
#	
#	Revision 1.2  2001/05/16 16:03:17  lewis
#	more massive changes. Make Options () stuff portable (though fake impl om mac/X). Got rid of
#	AppWindow class - and now X-WIndows fully using LedDoc/LedView. Much more common code between
#	implementations - but still a lot todo
#	
#	Revision 1.1  2001/05/14 20:54:42  lewis
#	New LedIt! CrossPlatform app - based on merging LedItPP and LedItMFC and parts of LedTextXWindows
#	
#
#
#
#	<<<***		Based on LedTestXWindows project from Led 3.0b6		***>>>
#
#
#
#
#
SHELL	=	/bin/sh
CC		=	g++
LD		=	g++
AR		=	ar


# By default build with DEBUG TRUE. Override with make DEBUG=0 to build no-debug builds...
ifeq ($(origin DEBUG),undefined)
DEBUG=1
endif

# By default build with DEBUG_SYMBOLS iff DEBUG. Override with make DEBUG_SYMBOLS=0 or 1
ifeq ($(origin DEBUG_SYMBOLS),undefined)
DEBUG_SYMBOLS=$(DEBUG)
endif

# By default build with UNICODE=0 (may change to true soon - once I've tested that way)
ifeq ($(origin UNICODE),undefined)
UNICODE=0
endif

# By default build with STATIC=0. STATIC=0 makes for bigger builds - but builds less dependent on OS version / version of gtk etc libs...
ifeq ($(origin STATIC),undefined)
STATIC=0
endif





#CXXFLAGS	=	-g -DqDebug=$(DEBUG) `gtk-config --cflags`
CXXFLAGS	=	`gtk-config --cflags` -DqXWindows=1
ifeq ($(DEBUG),1)
CXXFLAGS +=	-DqDebug=1
else
CXXFLAGS +=	-DqDebug=0 -DNDEBUG -O4
endif
ifeq ($(DEBUG_SYMBOLS),1)
CXXFLAGS +=	-g
endif
ifeq ($(UNICODE),1)
CXXFLAGS +=	-DqWideCharacters=1
else
CXXFLAGS +=	-DqSingleByteCharacters=1
endif

LDFLAGS	=	`gtk-config --libs`
ifeq ($(DEBUG_SYMBOLS),1)
LDFLAGS +=	-g
else
LDFLAGS +=	-s
endif
ifeq ($(STATIC),1)
LDFLAGS +=	-static
endif


INCLUDES=	-I../Led/Headers -I../SpelledIt/Dictionaries -IHeaders -ISources


ObjDir=Objs_D$(DEBUG)_U$(UNICODE)/


VPATH	=	Sources:Headers:../Led/Sources:../Led/Headers:$(ObjDir):.


.SUFFIXES:
.SUFFIXES: .cpp .o



OBJS=	\
	$(ObjDir)LedItApplication.o \
	$(ObjDir)LedItDocument.o \
	$(ObjDir)LedItView.o \
	$(ObjDir)Main.o \
	$(ObjDir)Options.o



LedOBJS=	\
	$(ObjDir)BiDiLayoutEngine.o\
	$(ObjDir)ChunkedArrayTextStore.o\
	$(ObjDir)CodePage.o\
	$(ObjDir)Command.o\
	$(ObjDir)FlavorPackage.o\
	$(ObjDir)HiddenText.o\
	$(ObjDir)IdleManager.o\
	$(ObjDir)Led_Gtk.o\
	$(ObjDir)Led_Gtk_WordProcessor.o\
	$(ObjDir)LedGDI.o\
	$(ObjDir)LedStdDialogs.o\
	$(ObjDir)LedSupport.o\
	$(ObjDir)LineBasedPartition.o\
	$(ObjDir)PartitioningTextImager.o\
	$(ObjDir)Marker.o\
	$(ObjDir)MarkerCover.o\
	$(ObjDir)MultiRowTextImager.o\
	$(ObjDir)PartitioningTextImager.o\
	$(ObjDir)SimpleTextStore.o\
	$(ObjDir)SpellCheckEngine.o\
	$(ObjDir)SpellCheckEngine_Basic.o\
	$(ObjDir)StandardStyledTextImager.o\
	$(ObjDir)StandardStyledTextInteractor.o\
	$(ObjDir)SimpleTextImager.o\
	$(ObjDir)SimpleTextStore.o\
	$(ObjDir)StyledTextEmbeddedObjects.o\
	$(ObjDir)StyledTextImager.o\
	$(ObjDir)StyledTextIO.o\
	$(ObjDir)StyledTextIO_HTML.o\
	$(ObjDir)StyledTextIO_LedNative.o\
	$(ObjDir)StyledTextIO_MIMETextEnriched.o\
	$(ObjDir)StyledTextIO_PlainText.o\
	$(ObjDir)StyledTextIO_RTF.o\
	$(ObjDir)StyledTextIO_STYLText.o\
	$(ObjDir)TextBreaks.o\
	$(ObjDir)TextImager.o\
	$(ObjDir)TextInteractor.o\
	$(ObjDir)TextInteractorMixins.o\
	$(ObjDir)TextStore.o\
	$(ObjDir)WordProcessor.o\
	$(ObjDir)WordWrappedTextImager.o\
	$(ObjDir)WordWrappedTextInteractor.o


ifeq ($(DEBUG),1)
LedItTargetTags1=-Debug
else
LedItTargetTags1=
endif
ifeq ($(UNICODE),1)
LedItTargetTags=$(LedItTargetTags1)-Unicode
else
LedItTargetTags=$(LedItTargetTags1)-NoUnicode
endif

LedItTarget	=	LedIt$(LedItTargetTags)



.PHONY:	all
all:	$(LedItTarget)


.PHONY:	clean
clean:
	-rm -f $(OBJS) $(LedOBJS)


.PHONY:	clobber
clobber:	clean
	-rm -f $(ObjDir)Led.a $(LedItTarget)
	-rmdir $(ObjDir)


$(LedItTarget):	 $(ObjDir) $(ObjDir)Led.a	$(OBJS)
	$(LD) $(StroikaLinkerPrefixArgs) -o $(LedItTarget) $(OBJS) $(ObjDir)Led.a $(LDFLAGS)



$(ObjDir)Led.a:	$(LedOBJS)
	@echo Making Led.a
	@rm -f $(ObjDir)Led.a
	@$(AR) cq $(ObjDir)Led.a $(LedOBJS)


$(ObjDir):
	@-mkdir $(ObjDir)


$(ObjDir)%.o: %.cpp
	@echo "Compiling $<"
	@$(CC) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

