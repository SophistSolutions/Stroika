StroikaSourceRoot	:=	$(abspath ../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Library/Sources/Stroika/Frameworks/Auth/OAuth/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Frameworks/Auth/OAuth/

vpath %cpp $(SrcDir)

SubDirs	:=	\


Objs	:=	\
	$(ObjDir)Client${OBJ_SUFFIX}\
	$(ObjDir)Configuration${OBJ_SUFFIX}\


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)


.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
