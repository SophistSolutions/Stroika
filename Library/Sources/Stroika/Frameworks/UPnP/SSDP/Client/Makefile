StroikaSourceRoot	:=	$(abspath ../../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Library/Sources/Stroika/Frameworks/UPnP/SSDP/Client/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Frameworks/UPnP/SSDP/Client/

vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)CachingListener${OBJ_SUFFIX}\
	$(ObjDir)Listener${OBJ_SUFFIX}\
	$(ObjDir)Search${OBJ_SUFFIX}\

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs)
