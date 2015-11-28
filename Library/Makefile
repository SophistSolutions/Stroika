ProjectPlatformSubdir	=	$(shell perl ../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)


MAKE_INDENT_LEVEL?=$(MAKELEVEL)


all:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory CONFIGURATION=$$i all;\
	done
else
	@../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo "Building Stroika {$(CONFIGURATION)} Library:"
ifeq (Unix,$(ProjectPlatformSubdir))
	@$(MAKE) --directory ../IntermediateFiles/$(CONFIGURATION)/Library/Foundation/ --no-print-directory all
	@$(MAKE) --directory ../IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/ --no-print-directory all
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
	@../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo -n "Checking Stroika {$(CONFIGURATION)} Library:"
	@export CONFIGURATION=$(CONFIGURATION); export MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL); cd Projects/$(ProjectPlatformSubdir) && perl checkall.pl
endif
