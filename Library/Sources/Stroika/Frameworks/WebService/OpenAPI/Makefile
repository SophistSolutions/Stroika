StroikaSourceRoot	:=	$(abspath ../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Library/Sources/Stroika/Frameworks/WebService/Server/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Frameworks/WebService/OpenAPI/

vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)Specification${OBJ_SUFFIX}\


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)


$(SubDirs):
	@$(MAKE) --directory $@ -s
