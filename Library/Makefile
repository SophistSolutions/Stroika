export StroikaRoot?=$(shell realpath ..)/

ifneq ($(CONFIGURATION),)
	#no error if missing cuz could be doing make clobber
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk
endif

include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk


SHELL=/bin/bash

MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=	$(shell ../ScriptsLib/GetDefaultShellVariable.sh ECHO)


all:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations` ; do\
		$(MAKE) --no-print-directory CONFIGURATION=$$i all || exit $$?;\
	done
else
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Building Stroika Library {$(CONFIGURATION)}:"
	@$(MAKE) --directory Sources/Stroika/Foundation/ --no-print-directory --silent all MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Sources/Stroika/Frameworks/ --no-print-directory --silent all MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif


check:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations` ; do\
		$(MAKE) --no-print-directory CONFIGURATION=$$i check || exit $$?;\
	done
else
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Checking Stroika Library {$(CONFIGURATION)}:      "
	@../ScriptsLib/CheckFileExists.sh $(StroikaFoundationLib) $(StroikaFrameworksLib)
	@$(ECHO) "[SUCCEEDED]"
endif



clean:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations` ; do\
		$(MAKE) --no-print-directory CONFIGURATION=$$i clean;\
	done
else
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Cleaning Stroika Library {$(CONFIGURATION)} ... "
ifeq (Unix,$(ProjectPlatformSubdir))
	@test -d ../IntermediateFiles/$(CONFIGURATION)/ && find ../IntermediateFiles/$(CONFIGURATION)/Library/ -name '*.o' -exec rm -f {} \; || true
else
	@test -d ../IntermediateFiles/$(CONFIGURATION)/ && /bin/find ../IntermediateFiles/$(CONFIGURATION)/Library/ -name '*.obj' -exec rm -f {} \; || true
	@test -d ../IntermediateFiles/$(CONFIGURATION)/ && /bin/find ../IntermediateFiles/$(CONFIGURATION)/Library/ -name '*.log' -exec rm -f {} \; || true
	@test -d ../IntermediateFiles/$(CONFIGURATION)/ && /bin/find ../IntermediateFiles/$(CONFIGURATION)/Library/ -depth -name '*.tlog' -type d -exec rm -rf {} \; || true
endif
	@$(ECHO) done
endif





clobber:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations` ; do\
		$(MAKE) --no-print-directory CONFIGURATION=$$i clobber;\
	done
else
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Clobbering Stroika Library {$(CONFIGURATION)}:"
	@$(MAKE) --directory Sources/Stroika/Foundation/ --no-print-directory --silent clobber MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) 2> /dev/null || true
	@$(MAKE) --directory Sources/Stroika/Frameworks/ --no-print-directory --silent clobber MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) 2> /dev/null || true
endif
