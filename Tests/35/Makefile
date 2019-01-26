export StroikaRoot?=$(abspath ../../)/

ifneq ($(CONFIGURATION),)
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

TEST_NUM=$(shell basename $(dir $(abspath $(lastword $(MAKEFILE_LIST))))))
include $(StroikaRoot)/Tests/Makefile-Test-Template.mk
