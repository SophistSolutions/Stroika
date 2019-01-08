include ../../../../../IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir					=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Streams/
ObjDir					=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Streams/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)

SubDirs	=	\
	iostream	\

.PHONY:			$(SubDirs)


Objs	=	\
	$(ObjDir)EOFException.o\
	$(ObjDir)InputStream.o\
	$(ObjDir)OutputStream.o\
	$(ObjDir)MemoryStream.o\
	$(ObjDir)SharedMemoryStream.o\
	$(ObjDir)TextReader.o\
	$(ObjDir)TextWriter.o\

all:	$(ObjDir) $(Objs)  $(SubDirs)


$(SubDirs):
	@$(MAKE) --directory $@ -s
