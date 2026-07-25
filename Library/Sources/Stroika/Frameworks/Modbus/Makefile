StroikaRoot	:=	$(abspath ../../../../../)/
SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Frameworks/Modbus/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/Modbus/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)IModbusService${OBJ_SUFFIX}\
	$(ObjDir)Server${OBJ_SUFFIX}\


include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs)
