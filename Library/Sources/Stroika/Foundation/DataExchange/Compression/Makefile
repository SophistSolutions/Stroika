StroikaRoot	:=	$(abspath ../../../../../../)/
SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/DataExchange/Compression/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/DataExchange/Compression/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)Deflate${OBJ_SUFFIX}\
	$(ObjDir)GZip${OBJ_SUFFIX}\
	$(ObjDir)ZStd${OBJ_SUFFIX}\

SubDirs	:=	\
	Zip \


include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)


.PHONY:				$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
