StroikaRoot	:=	$(abspath ../../../../../../../)/
SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Cryptography/Providers/OpenSSL/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Cryptography/Providers/OpenSSL/

include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk


SubDirs	:=	\

vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)Certificate${OBJ_SUFFIX}\
	$(ObjDir)CipherAlgorithm${OBJ_SUFFIX}\
	$(ObjDir)ClientContext${OBJ_SUFFIX}\
	$(ObjDir)DigestAlgorithm${OBJ_SUFFIX}\
	$(ObjDir)DerivedKey${OBJ_SUFFIX}\
	$(ObjDir)Exception${OBJ_SUFFIX}\
	$(ObjDir)LibraryContext${OBJ_SUFFIX}\
	$(ObjDir)PEMFile${OBJ_SUFFIX}\
	$(ObjDir)PrivateKey${OBJ_SUFFIX}\
	$(ObjDir)ServerContext${OBJ_SUFFIX}\
	$(ObjDir)SocketStream${OBJ_SUFFIX}\

include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk

all:	$(Objs)  $(SubDirs)


