-include ../IntermediateFiles/DefaultConfiguration/Library/Configuration.mk

CONFIGURATION 			?=	$(shell perl ../ScriptsLib/GetDefaultConfiguration.pl)
ProjectPlatformSubdir	=	$(shell perl ../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)

MAKE_INDENT_LEVEL?=$(MAKELEVEL)



.PHONY:	all

all:
	@if which doxygen 2>&1 > /dev/null; then\
		export MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL); cd Doxygen && perl ./RunDoxygen.pl;\
	else\
	    ../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo "Skipping build of documentation since Doxygen not installed";\
	fi
