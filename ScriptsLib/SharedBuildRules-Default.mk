$(warning ScriptsLib/SharedBuildRules-Default.mk is deprecated -- use Build/ScriptsLib/SharedBuildRules-Default.mk instead)
include $(dir $(lastword $(MAKEFILE_LIST)))../Build/ScriptsLib/SharedBuildRules-Default.mk
