StroikaRoot=$(abspath ../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Streams/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Streams/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)

SubDirs	=	\
	iostream	\



Objs	=	\
	$(ObjDir)EOFException${OBJ_SUFFIX}\
	$(ObjDir)InputStream${OBJ_SUFFIX}\
	$(ObjDir)OutputStream${OBJ_SUFFIX}\
	$(ObjDir)MemoryStream${OBJ_SUFFIX}\
	$(ObjDir)SharedMemoryStream${OBJ_SUFFIX}\
	$(ObjDir)TextReader${OBJ_SUFFIX}\
	$(ObjDir)TextWriter${OBJ_SUFFIX}\

all:	$(Objs)  $(SubDirs)


.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
