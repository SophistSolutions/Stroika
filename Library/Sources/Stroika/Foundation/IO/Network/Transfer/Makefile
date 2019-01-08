include ../../../../../../../IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir					=	$(StroikaRoot)Library/Sources/Stroika/Foundation/IO/Network/Transfer/
ObjDir					=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/IO/Network/Transfer/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk




vpath %cpp $(SrcDir)


Objs	=	\
	$(ObjDir)Client.o\
	$(ObjDir)Client_libcurl.o\
	$(ObjDir)Client_WinHTTP.o\
	$(ObjDir)Connection.o\
	$(ObjDir)Exception.o\
	$(ObjDir)Request.o\
	$(ObjDir)Response.o\


all:	$(ObjDir) $(Objs)

