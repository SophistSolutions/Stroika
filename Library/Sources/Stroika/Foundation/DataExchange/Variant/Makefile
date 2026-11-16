StroikaSourceRoot	:=	$(abspath ../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Library/Sources/Stroika/Foundation/DataExchange/Variant/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/DataExchange/Variant/

vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)Reader${OBJ_SUFFIX}\
	$(ObjDir)Writer${OBJ_SUFFIX}\

SubDirs	:=	\
	CharacterDelimitedLines \
	FormURLEncoded \
	INI \
	JSON \
	XML \


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)


.PHONY:				$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
