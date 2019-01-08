include ../../../../../IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir					=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Memory/
ObjDir					=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Memory/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk



VPATH = $(SrcDir)

Objs	=	\
	$(ObjDir)BLOB.o\
	$(ObjDir)BlockAllocator.o\
	$(ObjDir)Common.o\
	$(ObjDir)MemoryAllocator.o\
	$(ObjDir)SharedPtr.o\


all:	$(ObjDir) $(Objs)
	
