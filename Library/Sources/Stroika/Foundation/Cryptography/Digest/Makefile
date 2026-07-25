StroikaRoot	:=	$(abspath ../../../../../../)/
SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Cryptography/Digest/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Cryptography/Digest/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk

SubDirs	:=	\
	Algorithm	\


vpath %cpp $(SrcDir)


Objs	:=	\



all:	$(Objs)  $(SubDirs)


.PHONY:	$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
