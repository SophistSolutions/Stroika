ACTIVE_CONFIGURATION 	?=	$(shell perl ../ScriptsLib/GetDefaultConfiguration.pl)
ProjectPlatformSubdir	=	$(shell perl ../ScriptsLib/PrintConfigurationVariable.pl $(ACTIVE_CONFIGURATION) ProjectPlatformSubdir)

all:
	@../ScriptsLib/PrintLevelLeader.sh $(MAKELEVEL) && echo "Building Stroika Library..."
ifeq (,$(findstring CYGWIN,$(shell uname)))
	@$(MAKE) --directory ../IntermediateFiles/$(ACTIVE_CONFIGURATION)/Library/Foundation/ --no-print-directory all
	@$(MAKE) --directory ../IntermediateFiles/$(ACTIVE_CONFIGURATION)/Library/Frameworks/ --no-print-directory all
else
	@cd Projects/$(ProjectPlatformSubdir) && perl buildall.pl Build
endif
	@../ScriptsLib/PrintLevelLeader.sh $(MAKELEVEL) && echo "Building Stroika Library...done"


check:
	@../ScriptsLib/PrintLevelLeader.sh $(MAKELEVEL) && echo "Checking Stroika Library..."
	@cd Projects/$(ProjectPlatformSubdir) && perl checkall.pl
