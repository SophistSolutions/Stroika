#Common bits of script / functions that maybe used in Stroika makefiles.
#This has no dependencies (besides gnu make)

#NB: we export variables like ECHO since they dont change from makefile to makefile and pass the values to submakes and avoid being recomputed/re-fetched(performance tweek on make)

ifndef StroikaRoot
$(error("StroikaRoot must be defined and included before this file"))
endif

FUNCTION_CAPITALIZE_WORD=$(shell echo $1 | tr '[:lower:]' '[:upper:]' | cut -c 1-1)$(shell echo $1 | cut -c 2-99)

# intentionally export just as a performance hack (avoid call to getdefaultshellvariable)
export ECHO?=	$(shell $(StroikaRoot)ScriptsLib/GetDefaultShellVariable.sh ECHO)

# intentionally export cuz sometimes MAKE_INDENT_LEVEL doesnt go up as fast as MAKELEVEL, and so submakes work out
# looking better with less explicit setting
export MAKE_INDENT_LEVEL?=$(MAKELEVEL)

# intentionally DONT set export for this cuz it could vary from makefile to makefile
SHELL?=/bin/bash