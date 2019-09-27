StroikaRoot=$(abspath ..)/

ifneq ($(CONFIGURATION),)
	#no error if missing cuz could be doing make clobber
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk



all:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations` ; do\
		$(MAKE) --no-print-directory CONFIGURATION=$$i all || exit $$?;\
	done
else
	@../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Stroika Library {$(CONFIGURATION)}:"
	@$(MAKE) --directory Sources/Stroika/Foundation/ --no-print-directory --silent all MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Sources/Stroika/Frameworks/ --no-print-directory --silent all MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif


check:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations` ; do\
		$(MAKE) --no-print-directory CONFIGURATION=$$i check || exit $$?;\
	done
else
	@../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Checking Stroika Library {$(CONFIGURATION)}:      "
	@../ScriptsLib/CheckFileExists $(StroikaFoundationLib) $(StroikaFrameworksLib)
	@$(ECHO) "[SUCCEEDED]"
endif



clean clobber:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations` ; do\
		$(MAKE) --no-print-directory CONFIGURATION=$$i $@;\
	done
else
	@../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Stroika/Library $(call FUNCTION_CAPITALIZE_WORD,$@) {$(CONFIGURATION)} ... "
	@$(MAKE) --directory Sources/Stroika/Foundation --no-print-directory $@
	@$(MAKE) --directory Sources/Stroika/Frameworks --no-print-directory $@
	@$(ECHO) done
endif

