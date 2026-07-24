$(warning ScriptsLib/SharedBuildRules-Default.mk is deprecated -- use Build/Lib/Make/SharedBuildRules-Default.mk instead)
include $(dir $(lastword $(MAKEFILE_LIST)))../Build/Lib/Make/SharedBuildRules-Default.mk
