# DEPRECATED: make's own build (run by Makefile-Legacy), kept until Stroika v3.0d26 ships - Stroika builds with CMake (CMakeLists.txt)
export StroikaSourceRoot	:=	$(abspath ../../)/
TEST_NUM			:=	$(shell basename $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
include $(StroikaSourceRoot)/Tests/Makefile-Test-Template.mk
