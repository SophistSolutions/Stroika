StroikaRoot	:=	$(abspath ../../../../../../)/
SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Cryptography/Encoding/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Cryptography/Encoding/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SubDirs	:=	\
	Algorithm	\

vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)OpenSSLCryptoStream${OBJ_SUFFIX}\

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs)  $(SubDirs)

.PHONY:	$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ --no-print-directory -s
