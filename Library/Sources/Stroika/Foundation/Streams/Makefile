include ../../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir					=       $(StroikaRoot)Library/Sources/Stroika/Foundation/Streams/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Streams/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)

SubDirs	=	\
	iostream	\

.PHONY:			$(SubDirs)


Objs	=	\
	$(ObjDir)EOFException.o\
	$(ObjDir)InputStream.o\
	$(ObjDir)OutputStream.o\
	$(ObjDir)MemoryStream.o\
	$(ObjDir)TextReader.o\
	$(ObjDir)TextWriter.o\

all:	$(Objs)  $(SubDirs)


list-objs::
	@echo $(shell $(MAKE) --directory iostream -s list-objs CONFIGURATION=$(CONFIGURATION))


$(SubDirs):
	@$(MAKE) --directory $@ -s CONFIGURATION=$(CONFIGURATION)
