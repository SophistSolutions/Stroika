ProjectPlatformSubdir	=	$(shell ../ScriptsLib/GetConfigurationParameter $(CONFIGURATION) ProjectPlatformSubdir)

SHELL=/bin/bash

MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=	$(shell ../ScriptsLib/GetDefaultShellVariable.sh ECHO)


all:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory CONFIGURATION=$$i all || exit $$?;\
	done
else
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Building Stroika Library {$(CONFIGURATION)}:"
	@$(MAKE) --directory Sources/Stroika/Foundation/ --no-print-directory --silent all MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Sources/Stroika/Frameworks/ --no-print-directory --silent all MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif


check:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory CONFIGURATION=$$i check || exit $$?;\
	done
else
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Checking Stroika Library {$(CONFIGURATION)}:      "
ifeq (Unix,$(ProjectPlatformSubdir))
	@../ScriptsLib/CheckFileExists.sh "../Builds/$(CONFIGURATION)/Stroika-Foundation.a";
	@../ScriptsLib/CheckFileExists.sh "../Builds/$(CONFIGURATION)/Stroika-Frameworks.a";
endif
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	@## COULD also check PDB and IDB files but not needed
	@../ScriptsLib/CheckFileExists.sh "../Builds/$(CONFIGURATION)/Library/Stroika-Foundation.lib";
	@../ScriptsLib/CheckFileExists.sh "../Builds/$(CONFIGURATION)/Library/Stroika-Frameworks-Led.lib";
	@../ScriptsLib/CheckFileExists.sh "../Builds/$(CONFIGURATION)/Library/Stroika-Frameworks-Service.lib";
	@../ScriptsLib/CheckFileExists.sh "../Builds/$(CONFIGURATION)/Library/Stroika-Frameworks-SystemPerformance.lib";
	@../ScriptsLib/CheckFileExists.sh "../Builds/$(CONFIGURATION)/Library/Stroika-Frameworks-UPnP.lib";
	@../ScriptsLib/CheckFileExists.sh "../Builds/$(CONFIGURATION)/Library/Stroika-Frameworks-WebServer.lib";
endif
	@$(ECHO) "[SUCCEEDED]";
endif



clean:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
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
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory CONFIGURATION=$$i clobber;\
	done
else
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Clobbering Stroika Library {$(CONFIGURATION)}:"
	@$(MAKE) --directory Sources/Stroika/Foundation/ --no-print-directory --silent clobber MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) 2> /dev/null || true
	@$(MAKE) --directory Sources/Stroika/Frameworks/ --no-print-directory --silent clobber MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) 2> /dev/null || true
endif
