StroikaRoot	:=	$(abspath ../../../../../../)/

SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Cryptography/Providers/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Cryptography/Providers/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk



SubDirs	:=	\
	OpenSSL

vpath %cpp $(SrcDir)

Objs	:=	\


all:	$(Objs)  $(SubDirs)


.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
