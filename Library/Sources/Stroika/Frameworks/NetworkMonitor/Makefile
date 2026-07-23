StroikaRoot	:=	$(abspath ../../../../../)/
SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Frameworks/NetworkMonitor/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/NetworkMonitor/

include $(StroikaRoot)Build/ScriptsLib/SharedMakeVariables-Default.mk


ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=-Fd$(StroikaLibDir)stroika-frameworks.pdb
endif


vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)Ping${OBJ_SUFFIX}\
	$(ObjDir)Traceroute${OBJ_SUFFIX}\


include $(StroikaRoot)Build/ScriptsLib/SharedBuildRules-Default.mk


all:	$(Objs)