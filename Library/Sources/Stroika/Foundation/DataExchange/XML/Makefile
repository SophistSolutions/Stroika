StroikaSourceRoot	:=	$(abspath ../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Library/Sources/Stroika/Foundation/DataExchange/XML/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/DataExchange/XML/

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk



vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)DOM${OBJ_SUFFIX}\
	$(ObjDir)Resource${OBJ_SUFFIX}\
	$(ObjDir)SAXReader${OBJ_SUFFIX}\
	$(ObjDir)Schema${OBJ_SUFFIX}\
	$(ObjDir)WriterUtils${OBJ_SUFFIX}\


	
SubDirs	:=	\
	Providers \
	

all:	$(Objs) $(SubDirs)


.PHONY:				$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
