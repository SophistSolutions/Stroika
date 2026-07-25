StroikaRoot	:=	$(abspath ../../../../../)/
SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Memory/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Memory/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

VPATH = $(SrcDir)

Objs	:=	\
	$(ObjDir)BLOB${OBJ_SUFFIX}\
	$(ObjDir)BlockAllocator${OBJ_SUFFIX}\
	$(ObjDir)Common${OBJ_SUFFIX}\
	$(ObjDir)MemoryAllocator${OBJ_SUFFIX}\


include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs)
	
