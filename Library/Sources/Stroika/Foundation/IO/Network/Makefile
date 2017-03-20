include ../../../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir					=       $(StroikaRoot)Library/Sources/Stroika/Foundation/IO/Network/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/IO/Network/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk



SubDirs	=				\
	HTTP				\
	InternetProtocol	\
	SOAP				\
	Transfer			\

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

all:	$(ObjDir) $(Objs) $(SubDirs)


list-objs::
	@echo \
		$(shell $(MAKE) --directory HTTP -s list-objs CONFIGURATION=$(CONFIGURATION))\
		$(shell $(MAKE) --directory SOAP -s list-objs CONFIGURATION=$(CONFIGURATION))\
		$(shell $(MAKE) --directory Transfer -s list-objs CONFIGURATION=$(CONFIGURATION))\


$(SubDirs):
	@$(MAKE) --directory $@ -s CONFIGURATION=$(CONFIGURATION)
