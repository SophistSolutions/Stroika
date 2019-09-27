$(info "This file deprecated  (as of Stroika 2.1a2) - include ScriptsLib/SharedMakeVariables-Default.mk instead")
ifndef StroikaRoot
$(error("StroikaRoot must be defined and included before this file"))
endif
include $(StroikaRoot)/ScriptsLib/SharedMakeVariables-Default.mk
