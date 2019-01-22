StroikaRoot=$(abspath ../)/

ifneq ($(CONFIGURATION),)
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk

.PHONY:	samples all check clobber



help:
	@echo "Help for making Stroika-Tools"
	@echo "Targets"
	@echo "    all:                    -    builds everything"
	@echo "    check:                  -    checks everything built properly"
	@echo "    clean:"
	@echo "    clobber:"



all:
ifeq ($(CONFIGURATION),)
	@for i in `$(StroikaRoot)GetConfigurations` ; do\
		$(MAKE) --no-print-directory $@ CONFIGURATION=$$i || exit $$?;\
	done
else
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Stroika Tools {$(CONFIGURATION)}:"
	@$(MAKE) --directory Sources/Stroika/Frameworks/ --no-print-directory $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif


check:
ifeq ($(CONFIGURATION),)
	@for i in `$(StroikaRoot)ScriptsLib/GetConfigurations` ; do\
		$(MAKE) --no-print-directory $@ CONFIGURATION=$$i || exit $$?;\
	done
else
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Checking Stroika Tools {$(CONFIGURATION)}:"
	@$(MAKE) --directory Sources/Stroika/Frameworks/ --no-print-directory $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif




clean clobber::
ifeq ($(CONFIGURATION),)
	@for i in `$(StroikaRoot)ScriptsLib/GetConfigurations` ; do\
		$(MAKE) --no-print-directory $@ CONFIGURATION=$$i;\
	done
else
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(MAKE) --directory Sources/Stroika/Frameworks/ CONFIGURATION=$(CONFIGURATION) --no-print-directory $@ 2> /dev/null || true
endif

