StroikaRoot	:=	$(abspath ../../../../../../)/
SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Frameworks/Led/StyledTextIO/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/Led/StyledTextIO/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

vpath %cpp $(SrcDir)

Objs	:=	

# Led's GUI code is Windows only - there is no unix or macos GUI support
Objs+=	$(ObjDir)StyledTextIO${OBJ_SUFFIX}
Objs+=	$(ObjDir)StyledTextIO_HTML${OBJ_SUFFIX}
Objs+=	$(ObjDir)StyledTextIO_LedNative${OBJ_SUFFIX}
Objs+=	$(ObjDir)StyledTextIO_MIMETextEnriched${OBJ_SUFFIX}
Objs+=	$(ObjDir)StyledTextIO_PlainText${OBJ_SUFFIX}
Objs+=	$(ObjDir)StyledTextIO_RTF${OBJ_SUFFIX}

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs)
