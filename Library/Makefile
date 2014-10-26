ACTIVE_CONFIGURATION=DefaultConfiguration
ProjectPlatformSubdir=VisualStudio.Net-2013

all:
ifeq (,$(findstring CYGWIN,$(shell uname)))
	$(MAKE) --directory ../IntermediateFiles/$(ACTIVE_CONFIGURATION)/Library/Foundation/ --no-print-directory all
	$(MAKE) --directory ../IntermediateFiles/$(ACTIVE_CONFIGURATION)/Library/Frameworks/ --no-print-directory all
else
	cd Projects/$(ProjectPlatformSubdir) && perl buildall.pl Build
endif
