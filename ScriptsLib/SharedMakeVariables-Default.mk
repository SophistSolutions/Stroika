$(warning ScriptsLib/SharedMakeVariables-Default.mk is deprecated -- use Build/Lib/Make/SharedMakeVariables-Default.mk instead)
include $(dir $(lastword $(MAKEFILE_LIST)))../Build/Lib/Make/SharedMakeVariables-Default.mk
