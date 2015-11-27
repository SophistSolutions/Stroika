ifneq ($(CONFIGURATION),)
	ProjectPlatformSubdir	=	$(shell perl ../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)
endif


.PHONY:	samples all check clobber skel



help:
	@echo "Help for making Stroika-Tools"
	@echo "Targets"
	@echo "    all:                    -    builds everything"
	@echo "    check:                  -    checks everything built properly"
	@echo "    clean:"
	@echo "    clobber:"



all:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory all CONFIGURATION=$$i;\
	done
else
	@../ScriptsLib/PrintLevelLeader.sh $(MAKELEVEL) && echo "Building Stroika {$(CONFIGURATION)} Tools..."
ifeq (Unix,$(ProjectPlatformSubdir))
	@$(MAKE) --directory ../IntermediateFiles/$(CONFIGURATION)/Tools/Frameworks/ --no-print-directory all
else
	@ECHO_BUILD_LINES=$(ECHO_BUILD_LINES) CONFIGURATION=$(CONFIGURATION) cd "Projects/$(ProjectPlatformSubdir)" && perl buildall.pl
endif
	@../ScriptsLib/PrintLevelLeader.sh $(MAKELEVEL) && echo "Building Stroika {$(CONFIGURATION)} Tools...done"
endif


check:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory check CONFIGURATION=$$i;\
	done
else
	@../ScriptsLib/PrintLevelLeader.sh $(MAKELEVEL) && echo -n "Checking Stroika Tools for $(CONFIGURATION)"
	@CONFIGURATION=$(CONFIGURATION) cd "Projects/$(ProjectPlatformSubdir)" && perl checkall.pl
endif


clean:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory clean CONFIGURATION=$$i;\
	done
else
ifeq (Unix,$(ProjectPlatformSubdir))
	@$(MAKE) --directory ../IntermediateFiles/$(CONFIGURATION)/Tools/Frameworks/ --no-print-directory clean
else
	@CONFIGURATION=$(CONFIGURATION) cd "Projects/$(ProjectPlatformSubdir)" && perl buildall.pl clean
endif
endif


clobber:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory clobber CONFIGURATION=$$i;\
	done
else
ifeq (Unix,$(ProjectPlatformSubdir))
	@$(MAKE) --directory ../IntermediateFiles/$(CONFIGURATION)/Tools/Frameworks/ --no-print-directory clobber
else
	@CONFIGURATION=$(CONFIGURATION) cd "Projects/$(ProjectPlatformSubdir)" && perl buildall.pl clobber
endif
endif
