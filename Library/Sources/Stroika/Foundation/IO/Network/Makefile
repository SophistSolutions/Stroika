include ../../../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir					=       $(StroikaRoot)Library/Sources/Stroika/Foundation/IO/Network/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/IO/Network/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk



SubDirs	=		\
	HTTP		\
	SOAP		\
	Transfer	\

.PHONY:			$(SubDirs)

vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)DNS.o\
	$(ObjDir)Interface.o\
	$(ObjDir)InternetAddress.o\
	$(ObjDir)LinkMonitor.o\
	$(ObjDir)Listener.o\
	$(ObjDir)Socket.o\
	$(ObjDir)SocketAddress.o\
	$(ObjDir)SocketStream.o\
	$(ObjDir)URL.o\

all:	$(Objs) $(SubDirs)


HTTP_OBJS		:= $(shell $(MAKE) --directory HTTP -s list-objs CONFIGURATION=$(CONFIGURATION))
SOAP_OBJS		:= $(shell $(MAKE) --directory SOAP -s list-objs CONFIGURATION=$(CONFIGURATION))
Transfer_OBJS	:= $(shell $(MAKE) --directory Transfer -s list-objs CONFIGURATION=$(CONFIGURATION))


list-objs::
	@echo $(HTTP_OBJS) $(SOAP_OBJS) $(Transfer_OBJS)


$(SubDirs):
	@$(MAKE) --directory $@ -s CONFIGURATION=$(CONFIGURATION)
