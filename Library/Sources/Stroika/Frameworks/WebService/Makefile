include ../../../../../IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir						=   $(StroikaRoot)Library/Sources/Stroika/Frameworks/WebService/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/WebService/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk

SubDirs	=	\
	Client	\
	Server	\

.PHONY:			$(SubDirs)


vpath %cpp $(SrcDir)


Objs	=	\
	$(ObjDir)Basic${OBJ_SUFFIX}\


all:	$(ObjDir) $(Objs)  $(SubDirs)

$(SubDirs):
	@$(MAKE) --directory $@ -s
