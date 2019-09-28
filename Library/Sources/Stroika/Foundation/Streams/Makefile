StroikaRoot=$(abspath ../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Streams/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Streams/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=/Fd$(call FUNCTION_CONVERT_FILES_TO_COMPILER_NATIVE,$(StroikaRoot)Builds/$(CONFIGURATION)/Stroika-Foundation.pdb)
endif

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


include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk


.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
