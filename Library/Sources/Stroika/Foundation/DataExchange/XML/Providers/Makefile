# DEPRECATED: make's own build (run by Makefile-Legacy), kept until Stroika v3.0d26 ships - Stroika builds with CMake (CMakeLists.txt)
StroikaSourceRoot	:=	$(abspath ../../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Library/Sources/Stroika/Foundation/DataExchange/XML/Providers/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/DataExchange/XML/Providers/

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk



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
