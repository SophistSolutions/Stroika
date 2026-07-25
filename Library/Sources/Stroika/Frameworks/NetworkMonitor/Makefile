StroikaRoot	:=	$(abspath ../../../../../)/
SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Frameworks/NetworkMonitor/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/NetworkMonitor/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)Ping${OBJ_SUFFIX}\
	$(ObjDir)Traceroute${OBJ_SUFFIX}\


include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs)
