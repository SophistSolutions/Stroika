#	Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved
#
#	$Header: /cygdrive/k/CVSRoot/LedTestXWindows/Makefile,v 1.22 2003/03/20 16:03:13 lewis Exp $
#
#	$Log: Makefile,v $
#	Revision 1.22  2003/03/20 16:03:13  lewis
#	SPR#1360- add IdleManager
#	
#	Revision 1.21  2003/02/24 22:56:38  lewis
#	SPR#1303 - use qSingleByteCharacters/WideChar... instead of qLed_CharacterSet
#	
#	Revision 1.20  2003/01/23 22:35:12  lewis
#	add BiDiLayoutEngine.o
#	
#	Revision 1.19  2002/05/06 21:36:20  lewis
#	<=============================== Led 3.0.1 Released ==============================>
#	
#	Revision 1.18  2001/11/27 00:34:27  lewis
#	<=============== Led 3.0 Released ===============>
#	
#	Revision 1.17  2001/08/30 01:01:27  lewis
#	*** empty log message ***
#	
#	Revision 1.16  2001/05/25 13:15:14  lewis
#	fix qXWindows - NOT qWIndows Makefile
#	
#	Revision 1.15  2001/05/15 16:43:58  lewis
#	*** empty log message ***
#	
#	Revision 1.14  2001/05/05 09:37:08  lewis
#	*** empty log message ***
#	
#	Revision 1.13  2001/05/01 21:34:31  lewis
#	*** empty log message ***
#	
#	Revision 1.12  2001/05/01 21:31:54  lewis
#	SPR#0896- add new makefile config vars - DEBUG/UNICODE/STATIC/DEBUG_SYMBOLS - so you specific that var=0 or 1 on make CMD line - and you get those buildflags set. NB: when you change these flags - you must rebuild all (make clean)
#	
#	Revision 1.11  2001/05/01 20:51:08  lewis
#	static link
#	
#	Revision 1.10  2001/04/27 15:26:07  lewis
#	*** empty log message ***
#	
#	Revision 1.9  2000/09/05 22:36:44  lewis
#	Led_Gtk support
#	
#	Revision 1.8  2000/09/05 14:32:15  lewis
#	Support for building using GTK
#	
#	Revision 1.7  2000/06/15 20:14:35  lewis
#	added TextBreaks.o
#	
#	Revision 1.6  2000/04/29 22:51:19  lewis
#	Added CodePage and HiddenText modules
#	
#	Revision 1.5  2000/04/16 14:12:30  lewis
#	small tweeks to get compiling with new namespac and other changes
#	
#	Revision 1.4  2000/01/15 16:58:31  lewis
#	Finally got working with VPATH and multiple directories - builds lib and EXE neatly in OBJS dir etc
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





#CFLAGS	=	-g -DqDebug=$(DEBUG) `gtk-config --cflags`
CFLAGS	=	`gtk-config --cflags` -DqXWindows=1
ifeq ($(DEBUG),1)
CFLAGS +=	-DqDebug=1
else
CFLAGS +=	-DqDebug=0 -DNDEBUG
endif
ifeq ($(DEBUG_SYMBOLS),1)
CFLAGS +=	-g
endif
ifeq ($(UNICODE),1)
CFLAGS +=	-DqWideCharacters=1
else
CFLAGS +=	-DqSingleByteCharacters=1
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


INCLUDES=	-I../Led/Headers


ObjDir=Objs/


VPATH	=	Sources:Headers:../Led/Sources:../Led/Headers:$(ObjDir)


.SUFFIXES:
.SUFFIXES: .cpp .o



OBJS=	\
	$(ObjDir)Main.o



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
	$(ObjDir)LedSupport.o\
	$(ObjDir)LineBasedPartition.o\
	$(ObjDir)PartitioningTextImager.o\
	$(ObjDir)Marker.o\
	$(ObjDir)MarkerCover.o\
	$(ObjDir)MultiRowTextImager.o\
	$(ObjDir)PartitioningTextImager.o\
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




.PHONY:	all
all:	LedTestXWindows


.PHONY:	clean
clean:
	-rm -f $(OBJS) $(LedOBJS)


.PHONY:	clobber $(ObjDir)Led.a LedTestXWindows
clobber:	clean
	-rm -f $(ObjDir)Led.a LedTestXWindows
	-rmdir $(ObjDir)


LedTestXWindows:	 $(ObjDir) $(ObjDir)Led.a	$(OBJS)
	$(LD) -o LedTestXWindows $(OBJS) $(ObjDir)Led.a $(LDFLAGS)



$(ObjDir)Led.a:	$(LedOBJS)
	@echo Making Led.a
	@rm -f $(ObjDir)Led.a
	@$(AR) cq $(ObjDir)Led.a $(LedOBJS)


$(ObjDir):
	@-mkdir $(ObjDir)


$(ObjDir)%.o: %.cpp
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

