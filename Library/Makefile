ProjectPlatformSubdir	=	$(shell perl ../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)


MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=	$(shell ../ScriptsLib/GetDefaultShellVariable.sh ECHO)


all:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory CONFIGURATION=$$i all;\
	done
else
	@../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Building Stroika {$(CONFIGURATION)} Library:"
ifeq (Unix,$(ProjectPlatformSubdir))
	@$(MAKE) --directory ../IntermediateFiles/$(CONFIGURATION)/Library/Foundation/ --no-print-directory --silent all MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory ../IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/ --no-print-directory --silent all MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
else
	@export CONFIGURATION=$(CONFIGURATION); export MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL); cd Projects/$(ProjectPlatformSubdir) && perl buildall.pl Build
endif
endif


check:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory CONFIGURATION=$$i check;\
	done
else
	@../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) -n "Checking Stroika {$(CONFIGURATION)} Library:"
	@export CONFIGURATION=$(CONFIGURATION); export MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL); cd Projects/$(ProjectPlatformSubdir) && perl checkall.pl
endif



clean:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory CONFIGURATION=$$i clean;\
	done
else
ifeq (Unix,$(ProjectPlatformSubdir))
	@$(MAKE) --directory ../IntermediateFiles/$(CONFIGURATION)/Library/Foundation/ --no-print-directory --silent clean MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) 2> /dev/null || true
	@$(MAKE) --directory ../IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/ --no-print-directory --silent clean MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) 2> /dev/null || true
else
	@export CONFIGURATION=$(CONFIGURATION); export MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL); cd Projects/$(ProjectPlatformSubdir) && perl buildall.pl Clean
endif
endif
