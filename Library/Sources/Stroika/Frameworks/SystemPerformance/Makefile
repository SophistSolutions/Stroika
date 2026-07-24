StroikaRoot	:=	$(abspath ../../../../../)/
SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Frameworks/SystemPerformance/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/SystemPerformance/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=-Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaLibDir)stroika-frameworks.pdb)
endif


SubDirs	:=	\
	Instruments	\
	Support	\


vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)AllInstruments${OBJ_SUFFIX}\
	$(ObjDir)Capturer${OBJ_SUFFIX}\
	$(ObjDir)CaptureSet${OBJ_SUFFIX}\
	$(ObjDir)Instrument${OBJ_SUFFIX}\
	$(ObjDir)Measurement${OBJ_SUFFIX}\


include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)


.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
