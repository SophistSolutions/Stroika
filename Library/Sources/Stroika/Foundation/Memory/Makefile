StroikaRoot=$(abspath ../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Memory/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Memory/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk



VPATH = $(SrcDir)

Objs	=	\
	$(ObjDir)BLOB${OBJ_SUFFIX}\
	$(ObjDir)BlockAllocator${OBJ_SUFFIX}\
	$(ObjDir)Common${OBJ_SUFFIX}\
	$(ObjDir)MemoryAllocator${OBJ_SUFFIX}\
	$(ObjDir)SharedPtr${OBJ_SUFFIX}\


all:	$(Objs)
	
