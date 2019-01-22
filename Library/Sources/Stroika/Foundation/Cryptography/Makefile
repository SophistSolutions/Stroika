StroikaRoot=$(abspath ../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Cryptography/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Cryptography/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk


SubDirs	=		\
	Digest		\
	Encoding	\
	OpenSSL		\
	SSL			\


vpath %cpp $(SrcDir)


Objs	=	\
	$(ObjDir)Hash${OBJ_SUFFIX}\
	$(ObjDir)Format${OBJ_SUFFIX}\


all:	$(Objs) $(SubDirs)


.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
