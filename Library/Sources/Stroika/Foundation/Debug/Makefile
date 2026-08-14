StroikaRoot	:=	$(abspath ../../../../../)/
SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Foundation/Debug/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Debug/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)Assertions${OBJ_SUFFIX}\
	$(ObjDir)AssertExternallySynchronizedChecker${OBJ_SUFFIX}\
	$(ObjDir)BackTrace${OBJ_SUFFIX}\
	$(ObjDir)Debugger${OBJ_SUFFIX}\
	$(ObjDir)Demangle${OBJ_SUFFIX}\
	$(ObjDir)Fatal${OBJ_SUFFIX}\
	$(ObjDir)MallocGuard${OBJ_SUFFIX}\
	$(ObjDir)Main${OBJ_SUFFIX}\
	$(ObjDir)Trace${OBJ_SUFFIX}\
	$(ObjDir)Visualizations${OBJ_SUFFIX}\

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk

all:	$(Objs)
