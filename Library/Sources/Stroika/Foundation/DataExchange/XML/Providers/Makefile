StroikaRoot	:=	$(abspath ../../../../../../../)/
SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Foundation/DataExchange/XML/Providers/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/DataExchange/XML/Providers/

include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)

Objs	:=	\
		$(ObjDir)IProvider${OBJ_SUFFIX}

ifneq ($(qFeatureFlag_Xerces), no)
Objs +=	$(ObjDir)Xerces${OBJ_SUFFIX}
endif
ifneq ($(qFeatureFlag_libxml2), no)
Objs +=	$(ObjDir)LibXML2${OBJ_SUFFIX}
endif

all:	$(Objs)
