StroikaSourceRoot	:=	$(abspath ../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Library/Sources/Stroika/Foundation/Database/SQL/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/Database/SQL/

vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)Connection${OBJ_SUFFIX}\
	$(ObjDir)ODBC${OBJ_SUFFIX}\
	$(ObjDir)SQLite${OBJ_SUFFIX}\
	$(ObjDir)Statement${OBJ_SUFFIX}\
	$(ObjDir)Utils${OBJ_SUFFIX}\

SubDirs	:=	\
	ORM \

.PHONY:				$(SubDirs)

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk

all:	$(Objs)  $(SubDirs)


$(SubDirs):
	@$(MAKE) --directory $@ -s

