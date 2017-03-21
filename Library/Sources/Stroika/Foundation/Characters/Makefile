include ../../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir					=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Characters/
ObjDir					=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Characters/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk



vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)Character.o\
	$(ObjDir)CodePage.o\
	$(ObjDir)FloatConversion.o\
	$(ObjDir)Format.o\
	$(ObjDir)RegularExpression.o\
	$(ObjDir)SDKString.o\
	$(ObjDir)String.o\
	$(ObjDir)StringBuilder.o\
	$(ObjDir)String2Int.o\
	$(ObjDir)ToString.o\

SubDirs	=	\
	Concrete \
	CString \

.PHONY:	$(SubDirs)

all:	$(ObjDir) $(Objs) $(SubDirs)


list-objs::
	@for sd in $(SubDirs);\
	 do\
		$(MAKE) --directory $$sd -s list-objs CONFIGURATION=$(CONFIGURATION);\
	 done

$(SubDirs):
	@$(MAKE) --directory $@ -s CONFIGURATION=$(CONFIGURATION)
