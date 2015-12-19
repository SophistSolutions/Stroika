ifneq ($(CONFIGURATION),)
	ProjectPlatformSubdir	=	$(shell perl ../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)
endif

StroikaRoot=$(shell realpath ../)

MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=$(shell ../ScriptsLib/GetDefaultShellVariable.sh ECHO)

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
	@$(StroikaRoot)/ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Building Stroika {$(CONFIGURATION)} Tools:"
ifeq (Unix,$(ProjectPlatformSubdir))
	@$(MAKE) --directory ../IntermediateFiles/$(CONFIGURATION)/Tools/Frameworks/ --no-print-directory all MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
else
	@export CONFIGURATION=$(CONFIGURATION); export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)); export ECHO_BUILD_LINES=$(ECHO_BUILD_LINES); cd "Projects/$(ProjectPlatformSubdir)" && perl buildall.pl
endif
endif


check:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory check CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done
else
	@$(StroikaRoot)/ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) -n "Checking Stroika {$(CONFIGURATION)} Tools:"
	@export CONFIGURATION=$(CONFIGURATION); export MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL); cd "Projects/$(ProjectPlatformSubdir)" && perl checkall.pl
endif


clean:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory clean CONFIGURATION=$$i;\
	done
else
ifeq (Unix,$(ProjectPlatformSubdir))
	@$(MAKE) --directory ../IntermediateFiles/$(CONFIGURATION)/Tools/Frameworks/ --no-print-directory clean 2> /dev/null || true
else
	@export CONFIGURATION=$(CONFIGURATION); export MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL); cd "Projects/$(ProjectPlatformSubdir)" && perl buildall.pl clean
endif
endif


clobber:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory clobber CONFIGURATION=$$i;\
	done
else
ifeq (Unix,$(ProjectPlatformSubdir))
	@$(MAKE) --directory ../IntermediateFiles/$(CONFIGURATION)/Tools/Frameworks/ --no-print-directory clobber 2> /dev/null || true
else
	@export CONFIGURATION=$(CONFIGURATION); export MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL); cd "Projects/$(ProjectPlatformSubdir)" && perl buildall.pl clobber
endif
endif
