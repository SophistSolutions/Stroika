$(warning ScriptsLib/Makefile-Common.mk is deprecated -- use Build/Lib/Make/Makefile-Common.mk instead)
include $(dir $(lastword $(MAKEFILE_LIST)))../Build/Lib/Make/Makefile-Common.mk
