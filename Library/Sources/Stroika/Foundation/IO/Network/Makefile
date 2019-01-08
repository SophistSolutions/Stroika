include ../../../../../../IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir					=	$(StroikaRoot)Library/Sources/Stroika/Foundation/IO/Network/
ObjDir					=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/IO/Network/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk



SubDirs	=				\
	HTTP				\
	InternetProtocol	\
	SOAP				\
	Transfer			\

.PHONY:			$(SubDirs)

vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)CIDR.o\
	$(ObjDir)ConnectionlessSocket.o\
	$(ObjDir)ConnectionOrientedMasterSocket.o\
	$(ObjDir)ConnectionOrientedStreamSocket.o\
	$(ObjDir)DNS.o\
	$(ObjDir)Interface.o\
	$(ObjDir)InternetAddress.o\
	$(ObjDir)InternetAddressRange.o\
	$(ObjDir)LinkMonitor.o\
	$(ObjDir)Listener.o\
	$(ObjDir)Socket.o\
	$(ObjDir)SocketAddress.o\
	$(ObjDir)SocketStream.o\
	$(ObjDir)URL.o\
	$(ObjDir)WaitForSocketIOReady.o\

all:	$(ObjDir) $(Objs) $(SubDirs)

$(SubDirs):
	@$(MAKE) --directory $@ -s
