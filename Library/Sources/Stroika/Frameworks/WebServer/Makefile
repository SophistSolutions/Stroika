StroikaSourceRoot	:=	$(abspath ../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Library/Sources/Stroika/Frameworks/WebServer/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Frameworks/WebServer/

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


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs)
