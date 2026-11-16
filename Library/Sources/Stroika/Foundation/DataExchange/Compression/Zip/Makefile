StroikaSourceRoot	:=	$(abspath ../../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Library/Sources/Stroika/Foundation/DataExchange/Compression/Zip/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/DataExchange/Compression/Zip/

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk



vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)Reader${OBJ_SUFFIX}\


all:	$(Objs)
