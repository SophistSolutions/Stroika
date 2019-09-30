StroikaRoot=$(abspath ../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=   $(StroikaRoot)Library/Sources/Stroika/Frameworks/Led/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/Led/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=/Fd$(call FUNCTION_CONVERT_FILES_TO_COMPILER_NATIVE,$(StroikaRoot)Builds/$(CONFIGURATION)/Stroika-Frameworks.pdb)
endif

vpath %cpp $(SrcDir)

SubDirs	=	\
	Platform	\
	StyledTextIO	\


Objs	=	\
	$(ObjDir)BiDiLayoutEngine${OBJ_SUFFIX}\
	$(ObjDir)ChunkedArrayTextStore${OBJ_SUFFIX}\
	$(ObjDir)Command${OBJ_SUFFIX}\
	$(ObjDir)FlavorPackage${OBJ_SUFFIX}\
	$(ObjDir)GDI${OBJ_SUFFIX}\
	$(ObjDir)HandySimple${OBJ_SUFFIX}\
	$(ObjDir)HiddenText${OBJ_SUFFIX}\
	$(ObjDir)IdleManager${OBJ_SUFFIX}\
	$(ObjDir)LineBasedPartition${OBJ_SUFFIX}\
	$(ObjDir)Marker${OBJ_SUFFIX}\
	$(ObjDir)MultiRowTextImager${OBJ_SUFFIX}\
	$(ObjDir)PartitioningTextImager${OBJ_SUFFIX}\
	$(ObjDir)SimpleLed${OBJ_SUFFIX}\
	$(ObjDir)SimpleTextImager${OBJ_SUFFIX}\
	$(ObjDir)SimpleTextInteractor${OBJ_SUFFIX}\
	$(ObjDir)SimpleTextStore${OBJ_SUFFIX}\
	$(ObjDir)SpellCheckEngine${OBJ_SUFFIX}\
	$(ObjDir)SpellCheckEngine_Basic${OBJ_SUFFIX}\
	$(ObjDir)StandardStyledTextImager${OBJ_SUFFIX}\
	$(ObjDir)StandardStyledTextInteractor${OBJ_SUFFIX}\
	$(ObjDir)StdDialogs${OBJ_SUFFIX}\
	$(ObjDir)StyledTextEmbeddedObjects${OBJ_SUFFIX}\
	$(ObjDir)StyledTextImager${OBJ_SUFFIX}\
	$(ObjDir)Support${OBJ_SUFFIX}\
	$(ObjDir)SyntaxColoring${OBJ_SUFFIX}\
	$(ObjDir)TextBreaks${OBJ_SUFFIX}\
	$(ObjDir)TextImager${OBJ_SUFFIX}\
	$(ObjDir)TextInteractor${OBJ_SUFFIX}\
	$(ObjDir)TextInteractorMixins${OBJ_SUFFIX}\
	$(ObjDir)TextStore${OBJ_SUFFIX}\
	$(ObjDir)WordProcessor${OBJ_SUFFIX}\
	$(ObjDir)WordWrappedTextImager${OBJ_SUFFIX}\
	$(ObjDir)WordWrappedTextInteractor${OBJ_SUFFIX}\


include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk






all:	$(Objs) $(SubDirs)


.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
