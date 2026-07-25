StroikaRoot	:=	$(abspath ../../../../../)/

SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Cryptography/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Cryptography/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SubDirs	:=		\
	Digest		\
	Encoding	\
	Providers	\
	PKI			\
	SSL			\


vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)Format${OBJ_SUFFIX}\

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)


.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
