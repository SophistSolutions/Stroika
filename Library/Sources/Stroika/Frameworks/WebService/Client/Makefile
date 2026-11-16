StroikaSourceRoot	:=	$(abspath ../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Library/Sources/Stroika/Frameworks/WebService/Client/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Frameworks/WebService/Client/

vpath %cpp $(SrcDir)


Objs	:=	\


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)


$(SubDirs):
	@$(MAKE) --directory $@ -s
