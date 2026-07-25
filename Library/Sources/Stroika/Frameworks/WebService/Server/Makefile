StroikaRoot	:=	$(abspath ../../../../../../)/
SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Frameworks/WebService/Server/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/WebService/Server/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)Basic${OBJ_SUFFIX}\
	$(ObjDir)ObjectRequestHandler${OBJ_SUFFIX}\
	$(ObjDir)VariantValue${OBJ_SUFFIX}\


include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)


$(SubDirs):
	@$(MAKE) --directory $@ -s
