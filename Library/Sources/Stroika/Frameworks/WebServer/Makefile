StroikaRoot	:=	$(abspath ../../../../../)/
SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Frameworks/WebServer/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/WebServer/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=-Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaLibDir)stroika-frameworks.pdb)
endif


vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)ConnectionManager${OBJ_SUFFIX}\
	$(ObjDir)Connection${OBJ_SUFFIX}\
	$(ObjDir)CORS${OBJ_SUFFIX}\
	$(ObjDir)DefaultFaultInterceptor${OBJ_SUFFIX}\
	$(ObjDir)FileSystemRequestHandler${OBJ_SUFFIX}\
	$(ObjDir)Interceptor${OBJ_SUFFIX}\
	$(ObjDir)InterceptorChain${OBJ_SUFFIX}\
	$(ObjDir)Message${OBJ_SUFFIX}\
	$(ObjDir)LoggingInterceptor${OBJ_SUFFIX}\
	$(ObjDir)Request${OBJ_SUFFIX}\
	$(ObjDir)RequestHandler${OBJ_SUFFIX}\
	$(ObjDir)Response${OBJ_SUFFIX}\
	$(ObjDir)Router${OBJ_SUFFIX}\


include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs)