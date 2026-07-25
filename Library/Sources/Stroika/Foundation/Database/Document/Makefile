StroikaRoot	:=	$(abspath ../../../../../../)/
SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Database/Document/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Database/Document/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk



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

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk

all:	$(Objs)  $(SubDirs)


$(SubDirs):
	@$(MAKE) --directory $@ -s

