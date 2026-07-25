StroikaRoot	:=	$(abspath ../../../../../../../)/
SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Frameworks/UPnP/SSDP/Client/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/UPnP/SSDP/Client/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)CachingListener${OBJ_SUFFIX}\
	$(ObjDir)Listener${OBJ_SUFFIX}\
	$(ObjDir)Search${OBJ_SUFFIX}\

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs)
