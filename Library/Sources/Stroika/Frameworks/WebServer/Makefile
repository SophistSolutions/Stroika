include ../../../../../IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir						=   $(StroikaRoot)Library/Sources/Stroika/Frameworks/WebServer/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/WebServer/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk



vpath %cpp $(SrcDir)


Objs	=	\
	$(ObjDir)ClientErrorException.o\
	$(ObjDir)ConnectionManager.o\
	$(ObjDir)Connection.o\
	$(ObjDir)DefaultFaultInterceptor.o\
	$(ObjDir)FileSystemRouter.o\
	$(ObjDir)Interceptor.o\
	$(ObjDir)InterceptorChain.o\
	$(ObjDir)Message.o\
	$(ObjDir)LoggingInterceptor.o\
	$(ObjDir)Request.o\
	$(ObjDir)RequestHandler.o\
	$(ObjDir)Response.o\
	$(ObjDir)Router.o\


all:	$(ObjDir) $(Objs)