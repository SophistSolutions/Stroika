ifneq ($(CONFIGURATION),)
	ProjectPlatformSubdir	=	$(shell perl ../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)
endif

StroikaRoot=$(shell realpath ../)/

MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=$(shell $(StroikaRoot)ScriptsLib/GetDefaultShellVariable.sh ECHO)

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
	@for i in `$(StroikaRoot)GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory $@ CONFIGURATION=$$i || exit $$?;\
	done
else
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Building Stroika Tools {$(CONFIGURATION)}:"
	@$(MAKE) --directory Sources/Stroika/Frameworks/ --no-print-directory $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif


check:
ifeq ($(CONFIGURATION),)
	@for i in `$(StroikaRoot)ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory $@ CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) || exit $$?;\
	done
else
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Checking Stroika Tools {$(CONFIGURATION)}:"
	@$(MAKE) --directory Sources/Stroika/Frameworks/ --no-print-directory $@ CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif




clean clobber::
ifeq ($(CONFIGURATION),)
	@for i in `$(StroikaRoot)ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory $@ CONFIGURATION=$$i;\
	done
else
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@$(MAKE) --directory Sources/Stroika/Frameworks/ CONFIGURATION=$(CONFIGURATION) --no-print-directory $@ 2> /dev/null || true
endif

