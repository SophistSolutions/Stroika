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


Objs	=

# for now - this only builds on windows, cuz unix requires x-windows, and macos only written for very old macos gdi (carbon)
ifneq ($(findstring Windows,$(TARGET_PLATFORMS)),)
Objs+=	$(ObjDir)BiDiLayoutEngine${OBJ_SUFFIX}
Objs+=	$(ObjDir)ChunkedArrayTextStore${OBJ_SUFFIX}
Objs+=	$(ObjDir)Command${OBJ_SUFFIX}
Objs+=	$(ObjDir)FlavorPackage${OBJ_SUFFIX}
Objs+=	$(ObjDir)GDI${OBJ_SUFFIX}
Objs+=	$(ObjDir)HandySimple${OBJ_SUFFIX}
Objs+=	$(ObjDir)HiddenText${OBJ_SUFFIX}
Objs+=	$(ObjDir)IdleManager${OBJ_SUFFIX}
Objs+=	$(ObjDir)LineBasedPartition${OBJ_SUFFIX}
Objs+=	$(ObjDir)Marker${OBJ_SUFFIX}
Objs+=	$(ObjDir)MultiRowTextImager${OBJ_SUFFIX}
Objs+=	$(ObjDir)PartitioningTextImager${OBJ_SUFFIX}
Objs+=	$(ObjDir)SimpleLed${OBJ_SUFFIX}
Objs+=	$(ObjDir)SimpleTextImager${OBJ_SUFFIX}
Objs+=	$(ObjDir)SimpleTextInteractor${OBJ_SUFFIX}
Objs+=	$(ObjDir)SimpleTextStore${OBJ_SUFFIX}
Objs+=	$(ObjDir)SpellCheckEngine${OBJ_SUFFIX}
Objs+=	$(ObjDir)SpellCheckEngine_Basic${OBJ_SUFFIX}
Objs+=	$(ObjDir)StandardStyledTextImager${OBJ_SUFFIX}
Objs+=	$(ObjDir)StandardStyledTextInteractor${OBJ_SUFFIX}
Objs+=	$(ObjDir)StdDialogs${OBJ_SUFFIX}
Objs+=	$(ObjDir)StyledTextEmbeddedObjects${OBJ_SUFFIX}
Objs+=	$(ObjDir)StyledTextImager${OBJ_SUFFIX}
Objs+=	$(ObjDir)Support${OBJ_SUFFIX}
Objs+=	$(ObjDir)SyntaxColoring${OBJ_SUFFIX}
Objs+=	$(ObjDir)TextBreaks${OBJ_SUFFIX}
Objs+=	$(ObjDir)TextImager${OBJ_SUFFIX}
Objs+=	$(ObjDir)TextInteractor${OBJ_SUFFIX}
Objs+=	$(ObjDir)TextInteractorMixins${OBJ_SUFFIX}
Objs+=	$(ObjDir)TextStore${OBJ_SUFFIX}
Objs+=	$(ObjDir)WordProcessor${OBJ_SUFFIX}
Objs+=	$(ObjDir)WordWrappedTextImager${OBJ_SUFFIX}
Objs+=	$(ObjDir)WordWrappedTextInteractor${OBJ_SUFFIX}
endif


include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk






all:	$(Objs) $(SubDirs)


.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
