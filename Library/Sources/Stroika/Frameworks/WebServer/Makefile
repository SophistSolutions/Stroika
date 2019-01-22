export StroikaRoot?=$(shell realpath ../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=   $(StroikaRoot)Library/Sources/Stroika/Frameworks/WebServer/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/WebServer/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk



vpath %cpp $(SrcDir)


Objs	=	\
	$(ObjDir)ClientErrorException${OBJ_SUFFIX}\
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


all:	$(Objs)