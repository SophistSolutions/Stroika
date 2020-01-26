StroikaRoot=$(abspath ../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=   $(StroikaRoot)Library/Sources/Stroika/Frameworks/WebServer/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/WebServer/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=/Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaRoot)Builds/$(CONFIGURATION)/Stroika-Frameworks.pdb)
endif


vpath %cpp $(SrcDir)


Objs	=	\
	$(ObjDir)ConnectionManager${OBJ_SUFFIX}\
	$(ObjDir)Connection${OBJ_SUFFIX}\
	$(ObjDir)DefaultFaultInterceptor${OBJ_SUFFIX}\
	$(ObjDir)FileSystemRouter${OBJ_SUFFIX}\
	$(ObjDir)Interceptor${OBJ_SUFFIX}\
	$(ObjDir)InterceptorChain${OBJ_SUFFIX}\
	$(ObjDir)Message${OBJ_SUFFIX}\
	$(ObjDir)LoggingInterceptor${OBJ_SUFFIX}\
	$(ObjDir)Request${OBJ_SUFFIX}\
	$(ObjDir)RequestHandler${OBJ_SUFFIX}\
	$(ObjDir)Response${OBJ_SUFFIX}\
	$(ObjDir)Router${OBJ_SUFFIX}\


include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk


all:	$(Objs)