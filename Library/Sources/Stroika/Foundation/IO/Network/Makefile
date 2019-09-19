StroikaRoot=$(abspath ../../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=	$(StroikaRoot)Library/Sources/Stroika/Foundation/IO/Network/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/IO/Network/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=/Fd$(shell cygpath --mixed $(StroikaRoot)Builds/$(CONFIGURATION)/Stroika-Foundation.pdb)
endif

SubDirs	=				\
	HTTP				\
	InternetProtocol	\
	SOAP				\
	Transfer			\


vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)CIDR${OBJ_SUFFIX}\
	$(ObjDir)ConnectionlessSocket${OBJ_SUFFIX}\
	$(ObjDir)ConnectionOrientedMasterSocket${OBJ_SUFFIX}\
	$(ObjDir)ConnectionOrientedStreamSocket${OBJ_SUFFIX}\
	$(ObjDir)DNS${OBJ_SUFFIX}\
	$(ObjDir)Interface${OBJ_SUFFIX}\
	$(ObjDir)InternetAddress${OBJ_SUFFIX}\
	$(ObjDir)LinkMonitor${OBJ_SUFFIX}\
	$(ObjDir)Listener${OBJ_SUFFIX}\
	$(ObjDir)Neighbors${OBJ_SUFFIX}\
	$(ObjDir)Socket${OBJ_SUFFIX}\
	$(ObjDir)SocketAddress${OBJ_SUFFIX}\
	$(ObjDir)SocketStream${OBJ_SUFFIX}\
	$(ObjDir)UniformResourceIdentification${OBJ_SUFFIX}\
	$(ObjDir)URI${OBJ_SUFFIX}\
	$(ObjDir)WaitForSocketIOReady${OBJ_SUFFIX}\

all:	$(Objs) $(SubDirs)

.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
