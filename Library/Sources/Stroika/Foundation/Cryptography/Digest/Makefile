StroikaSourceRoot	:=	$(abspath ../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Library/Sources/Stroika/Foundation/Cryptography/Digest/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/Cryptography/Digest/

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk

SubDirs	:=	\
	Algorithm	\


vpath %cpp $(SrcDir)


Objs	:=	\



all:	$(Objs)  $(SubDirs)


.PHONY:	$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
