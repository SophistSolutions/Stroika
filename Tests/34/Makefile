export StroikaRoot?=$(realpath ../../)/

ifneq ($(CONFIGURATION),)
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

TEST_NUM=$(shell basename `pwd`)
include $(StroikaRoot)/Tests/Makefile-Test-Template.mk
