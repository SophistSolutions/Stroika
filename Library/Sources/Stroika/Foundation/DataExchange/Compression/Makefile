StroikaSourceRoot	:=	$(abspath ../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Library/Sources/Stroika/Foundation/DataExchange/Compression/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/DataExchange/Compression/


vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)Deflate${OBJ_SUFFIX}\
	$(ObjDir)GZip${OBJ_SUFFIX}\
	$(ObjDir)ZStd${OBJ_SUFFIX}\

SubDirs	:=	\
	Zip \


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)


.PHONY:				$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
