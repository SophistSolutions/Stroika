-include ../IntermediateFiles/DefaultConfiguration/Library/Configuration.mk

CONFIGURATION 			?=	$(shell perl ../ScriptsLib/GetDefaultConfiguration.pl)
ProjectPlatformSubdir	=	$(shell perl ../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)


.PHONY:	all

all:
	@if which doxygen 2>&1 > /dev/null; then\
		cd Doxygen && perl ./RunDoxygen.pl;\
	else\
	    echo "   Skipping build of documentation since Doxygen not installed";\
	fi
