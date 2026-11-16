StroikaSourceRoot	:=	$(abspath ../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Library/Sources/Stroika/Frameworks/WebService/Server/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Frameworks/WebService/Server/

vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)Basic${OBJ_SUFFIX}\
	$(ObjDir)ObjectRequestHandler${OBJ_SUFFIX}\
	$(ObjDir)VariantValue${OBJ_SUFFIX}\


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)


$(SubDirs):
	@$(MAKE) --directory $@ -s
