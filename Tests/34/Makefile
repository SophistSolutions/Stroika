export StroikaRoot	:=	$(abspath ../../)/
TEST_NUM			:=	$(shell basename $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
include $(StroikaRoot)/Tests/Makefile-Test-Template.mk

$(ObjDir): $(SrcDir)

# order-only $(ObjDir): with -j, nothing else guarantees the directory exists before xxd writes into it
$(ObjDir)%.embed:	TestFiles/% | $(ObjDir)
	xxd -i TestFiles/$* $@

$(ObjDir)Test${OBJ_SUFFIX}:	$(ObjDir)personal.xml.embed $(ObjDir)personal.xsd.embed $(ObjDir)HealthFrameWorks_v3.xml.embed $(ObjDir)ReferenceContent-2012-03.xsd.embed $(ObjDir)ASTM_CCR_V1.xsd.embed $(ObjDir)SampleCCR.ccr.embed
