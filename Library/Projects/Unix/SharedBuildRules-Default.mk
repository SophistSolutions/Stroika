$(info "This file deprecated (as of Stroika 2.1a2) - include ScriptsLib/SharedBuildRules-Default.mk instead")
ifndef StroikaRoot
$(error("StroikaRoot must be defined and included before this file"))
endif
include $(StroikaRoot)/ScriptsLib/SharedBuildRules-Default.mk
