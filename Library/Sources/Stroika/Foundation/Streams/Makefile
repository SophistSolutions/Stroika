StroikaSourceRoot	:=	$(abspath ../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Library/Sources/Stroika/Foundation/Streams/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/Streams/

vpath %cpp $(SrcDir)

SubDirs	:=	\
	iostream	\



Objs	:=	\
	$(ObjDir)BufferedInputStream${OBJ_SUFFIX}\
	$(ObjDir)EOFException${OBJ_SUFFIX}\
	$(ObjDir)InputStream${OBJ_SUFFIX}\
	$(ObjDir)TextToBinary${OBJ_SUFFIX}\
	$(ObjDir)MemoryStream${OBJ_SUFFIX}\
	$(ObjDir)BinaryToText${OBJ_SUFFIX}\

all:	$(Objs)  $(SubDirs)


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
