CONFIGURATION 			?=	$(shell perl ../ScriptsLib/GetDefaultConfiguration.pl)
ProjectPlatformSubdir	=	$(shell perl ../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)

-include ../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

.PHONY:	samples all check clobber skel




help:
	@echo "Help for making Stroika-Tools"
	@echo "Targets"
	@echo "    all:                    -    builds everything"
	@echo "    check:                  -    checks everything built properly"
	@echo "    clean:"
	@echo "    clobber:"



all:
ifeq (Unix,$(ProjectPlatformSubdir))
	@make --directory ../IntermediateFiles/$(CONFIGURATION)/Tools/Frameworks/ --no-print-directory all
else
	@cd "Projects/$(ProjectPlatformSubdir)" && perl buildall.pl
endif

check:
	@echo "Checking Tools..."
	@cd "Projects/$(ProjectPlatformSubdir)" && perl checkall.pl

clean:
	@make --directory ../IntermediateFiles/$(CONFIGURATION)/Tools/Frameworks/ --no-print-directory clean

clobber:
	@make --directory ../IntermediateFiles/$(CONFIGURATION)/Tools/Frameworks/ --no-print-directory clobber

