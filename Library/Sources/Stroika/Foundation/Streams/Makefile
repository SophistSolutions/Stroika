StroikaRoot	:=	$(abspath ../../../../../)/
SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Streams/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Streams/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=-Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaLibDir)stroika-foundation.pdb)
endif

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


include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
