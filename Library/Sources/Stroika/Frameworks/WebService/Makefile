StroikaRoot	:=	$(abspath ../../../../../)/
SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Frameworks/WebService/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/WebService/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SubDirs	:=	\
	Client	\
	OpenAPI	\
	Server	\


vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)JSONRPC${OBJ_SUFFIX}	\


include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk

all:	$(Objs)  $(SubDirs)

.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
