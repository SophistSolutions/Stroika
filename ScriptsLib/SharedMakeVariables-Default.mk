$(warning ScriptsLib/SharedMakeVariables-Default.mk is deprecated -- use Build/ScriptsLib/SharedMakeVariables-Default.mk instead)
include $(dir $(lastword $(MAKEFILE_LIST)))../Build/ScriptsLib/SharedMakeVariables-Default.mk
