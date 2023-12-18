export StroikaRoot?=$(abspath ../../)/

ifneq ($(CONFIGURATION),)
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

TEST_NUM=$(shell basename `pwd`)
include $(StroikaRoot)/Tests/Makefile-Test-Template.mk

$(ObjDir): $(SrcDir)

$(ObjDir)ReferenceContent-2012-03.xsd.embed:
	xxd -i ReferenceContent-2012-03.xsd $@

$(ObjDir)HealthFrameWorks_v3.xml.embed:
	xxd -i HealthFrameWorks_v3.xml $@

$(ObjDir)personal.xml.embed:
	xxd -i personal.xml $@

$(ObjDir)personal.xsd.embed:
	xxd -i personal.xsd $@

$(ObjDir)Test${OBJ_SUFFIX}:	$(ObjDir)personal.xml.embed  $(ObjDir)personal.xsd.embed $(ObjDir)HealthFrameWorks_v3.xml.embed $(ObjDir)ReferenceContent-2012-03.xsd.embed
