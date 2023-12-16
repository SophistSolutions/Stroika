export StroikaRoot?=$(abspath ../../)/

ifneq ($(CONFIGURATION),)
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

TEST_NUM=$(shell basename `pwd`)
include $(StroikaRoot)/Tests/Makefile-Test-Template.mk

$(ObjDir): $(SrcDir)

$(ObjDir)personal.xml.embed:
	xxd -i personal.xml $(ObjDir)personal.xml.embed

$(ObjDir)personal.xsd.embed:
	xxd -i personal.xsd $(ObjDir)personal.xsd.embed

$(ObjDir)Test${OBJ_SUFFIX}:	$(ObjDir)personal.xml.embed  $(ObjDir)personal.xsd.embed
