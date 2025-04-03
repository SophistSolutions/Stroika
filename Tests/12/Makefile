export StroikaRoot?=$(abspath ../../)/

ifneq ($(CONFIGURATION),)
	#no error if missing cuz could be doing make clobber
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

TEST_NUM=$(shell basename `pwd`)
include $(StroikaRoot)/Tests/Makefile-Test-Template.mk
