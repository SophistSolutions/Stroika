StroikaSourceRoot	:=	$(abspath ../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Library/Sources/Stroika/Foundation/Memory/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/Memory/

VPATH = $(SrcDir)

Objs	:=	\
	$(ObjDir)BLOB${OBJ_SUFFIX}\
	$(ObjDir)BlockAllocator${OBJ_SUFFIX}\
	$(ObjDir)Common${OBJ_SUFFIX}\
	$(ObjDir)MemoryAllocator${OBJ_SUFFIX}\


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs)
	
