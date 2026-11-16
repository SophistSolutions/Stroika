StroikaSourceRoot	:=	$(abspath ../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:= 	$(StroikaSourceRoot)Library/Sources/Stroika/Frameworks/UPnP/SSDP/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Frameworks/UPnP/SSDP/

vpath %cpp $(SrcDir)

SubDirs	:=	\
	Client	\
	Server	\


Objs	:=	\
	$(ObjDir)Advertisement${OBJ_SUFFIX}\
	$(ObjDir)Common${OBJ_SUFFIX}\


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)


.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
