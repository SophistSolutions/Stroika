export StroikaRoot?=$(shell realpath ../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir					=	$(StroikaRoot)Library/Sources/Stroika/Foundation/IO/
ObjDir					=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/IO/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk



SubDirs	=	\
	FileSystem	\
	Network		\

.PHONY:			$(SubDirs)


vpath %cpp $(SrcDir)


Objs	=	\
	$(ObjDir)FileAccessException${OBJ_SUFFIX}\
	$(ObjDir)FileAccessMode${OBJ_SUFFIX}\
	$(ObjDir)FileBusyException${OBJ_SUFFIX}\
	$(ObjDir)FileFormatException${OBJ_SUFFIX}\


all:	$(Objs) $(SubDirs)


$(SubDirs):
	@$(MAKE) --directory $@ -s

