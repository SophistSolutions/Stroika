CONFIGURATION 			?=	$(shell perl ../ScriptsLib/GetDefaultConfiguration.pl)
ProjectPlatformSubdir	=	$(shell perl ../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)

INDENT_LEVEL=0

all:
	@../ScriptsLib/PrintLevelLeader.sh $(INDENT_LEVEL) && echo "Building Stroika Library..."
ifeq (,$(findstring CYGWIN,$(shell uname)))
	@$(MAKE) --directory ../IntermediateFiles/$(CONFIGURATION)/Library/Foundation/ --no-print-directory all
	@$(MAKE) --directory ../IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/ --no-print-directory all
else
	@cd Projects/$(ProjectPlatformSubdir) && perl buildall.pl Build
endif
	@../ScriptsLib/PrintLevelLeader.sh $(INDENT_LEVEL) && echo "Building Stroika Library...done"


check:
	@../ScriptsLib/PrintLevelLeader.sh $(INDENT_LEVEL) && echo -n "Checking Stroika Library..."
	@cd Projects/$(ProjectPlatformSubdir) && perl checkall.pl
