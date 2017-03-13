include ../../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir					=       $(StroikaRoot)Library/Sources/Stroika/Foundation/Memory/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Memory/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk



VPATH = $(SrcDir)

Objs	=	\
	$(ObjDir)AnyVariantValue.o\
	$(ObjDir)Bits.o\
	$(ObjDir)BLOB.o\
	$(ObjDir)BlockAllocated.o\
	$(ObjDir)BlockAllocator.o\
	$(ObjDir)Common.o\
	$(ObjDir)MemoryAllocator.o\
	$(ObjDir)SharedPtr.o\


all:	$(Objs)
	
