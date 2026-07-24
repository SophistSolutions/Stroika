StroikaRoot	:=	$(abspath ../../../../../../)/
SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Database/SQL/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Database/SQL/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=-Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaLibDir)stroika-foundation.pdb)
endif


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

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk

all:	$(Objs)  $(SubDirs)


$(SubDirs):
	@$(MAKE) --directory $@ -s

