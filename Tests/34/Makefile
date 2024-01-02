export StroikaRoot?=$(abspath ../../)/

ifneq ($(CONFIGURATION),)
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

TEST_NUM=$(shell basename `pwd`)
include $(StroikaRoot)/Tests/Makefile-Test-Template.mk

$(ObjDir): $(SrcDir)

$(ObjDir)ASTM_CCR_V1.xsd.embed:
	xxd -i TestFiles/ASTM_CCR_V1.xsd $@

$(ObjDir)SampleCCR.ccr.embed:
	xxd -i TestFiles/SampleCCR.ccr $@

$(ObjDir)ReferenceContent-2012-03.xsd.embed:
	xxd -i TestFiles/ReferenceContent-2012-03.xsd $@

$(ObjDir)HealthFrameWorks_v3.xml.embed:
	xxd -i TestFiles/HealthFrameWorks_v3.xml $@

$(ObjDir)personal.xml.embed:
	xxd -i TestFiles/personal.xml $@

$(ObjDir)personal.xsd.embed:
	xxd -i TestFiles/personal.xsd $@

$(ObjDir)Test${OBJ_SUFFIX}:	$(ObjDir)personal.xml.embed $(ObjDir)personal.xsd.embed $(ObjDir)HealthFrameWorks_v3.xml.embed $(ObjDir)ReferenceContent-2012-03.xsd.embed $(ObjDir)ASTM_CCR_V1.xsd.embed $(ObjDir)SampleCCR.ccr.embed
