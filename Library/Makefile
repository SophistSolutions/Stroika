ACTIVE_CONFIGURATION 	?=	$(shell perl ../ScriptsLib/PrintConfigurationVariable.pl DefaultConfiguration)
ProjectPlatformSubdir	=	$(shell perl ../ScriptsLib/PrintConfigurationVariable.pl ProjectPlatformSubdir)

all:
ifeq (,$(findstring CYGWIN,$(shell uname)))
	@$(MAKE) --directory ../IntermediateFiles/$(ACTIVE_CONFIGURATION)/Library/Foundation/ --no-print-directory all
	@$(MAKE) --directory ../IntermediateFiles/$(ACTIVE_CONFIGURATION)/Library/Frameworks/ --no-print-directory all
else
	@cd Projects/$(ProjectPlatformSubdir) && perl buildall.pl Build
endif


check:
	@echo "Checking Library..."
	@cd Projects/$(ProjectPlatformSubdir) && perl checkall.pl
