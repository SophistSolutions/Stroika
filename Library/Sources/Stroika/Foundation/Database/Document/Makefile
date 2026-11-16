StroikaSourceRoot	:=	$(abspath ../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Library/Sources/Stroika/Foundation/Database/Document/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/Database/Document/



vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)Collection${OBJ_SUFFIX}\
	$(ObjDir)Connection${OBJ_SUFFIX}\
	$(ObjDir)Filter${OBJ_SUFFIX}\
	$(ObjDir)MongoDBClient${OBJ_SUFFIX}\
	$(ObjDir)Projection${OBJ_SUFFIX}\
	$(ObjDir)SQLite${OBJ_SUFFIX}\
	$(ObjDir)LocalDocumentDB${OBJ_SUFFIX}\

SubDirs	:=	\

.PHONY:				$(SubDirs)

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk

all:	$(Objs)  $(SubDirs)


$(SubDirs):
	@$(MAKE) --directory $@ -s

