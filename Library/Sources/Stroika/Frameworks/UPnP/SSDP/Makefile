StroikaRoot	:=	$(abspath ../../../../../../)/
SrcDir		:= 	$(StroikaRoot)Library/Sources/Stroika/Frameworks/UPnP/SSDP/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/UPnP/SSDP/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

vpath %cpp $(SrcDir)

SubDirs	:=	\
	Client	\
	Server	\


Objs	:=	\
	$(ObjDir)Advertisement${OBJ_SUFFIX}\
	$(ObjDir)Common${OBJ_SUFFIX}\


include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)


.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
