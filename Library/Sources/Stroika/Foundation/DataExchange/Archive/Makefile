include ../../../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir					=       $(StroikaRoot)Library/Sources/Stroika/Foundation/DataExchange/Archive/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/DataExchange/Archive/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk




vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)Reader.o\

SubDirs	=	\
	7z \
	Zip \

.PHONY:				$(SubDirs)


all:	$(ObjDir) $(Objs) $(SubDirs)


$(SubDirs):
	@$(MAKE) --directory $@ -s
