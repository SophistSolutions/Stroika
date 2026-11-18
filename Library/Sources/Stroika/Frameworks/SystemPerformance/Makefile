# DEPRECATED: make's own build (run by Makefile-Legacy), kept until Stroika v3.0d26 ships - Stroika builds with CMake (CMakeLists.txt)
StroikaSourceRoot	:=	$(abspath ../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Library/Sources/Stroika/Frameworks/SystemPerformance/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Frameworks/SystemPerformance/

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


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)


.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
