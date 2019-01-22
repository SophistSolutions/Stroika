#Common bits of script / functions that maybe used in Stroika makefiles.
#This has no dependencies (besides gnu make)

#NB: we export variables like ECHO since they dont change from makefile to makefile and pass the values to submakes and avoid being recomputed/re-fetched(performance tweek on make)

ifndef StroikaRoot
$(error("StroikaRoot must be defined and included before this file"))
endif
#nb: StroikaRoot exported for use in some included scripts with minimal scripting effort in makefiles that call them
export StroikaRoot

# intentionally export just as a performance hack (avoid call to getdefaultshellvariable)
# nb we need to define ECHO at all (instead of using echo) because of some braindead default echo implementations, like AIX
export ECHO?=	$(shell $(StroikaRoot)ScriptsLib/GetDefaultShellVariable ECHO)

FUNCTION_CAPITALIZE_WORD=$(shell $(ECHO) $1 | tr '[:lower:]' '[:upper:]' | cut -c 1-1)$(shell $(ECHO) $1 | cut -c 2-99)

# intentionally export cuz sometimes MAKE_INDENT_LEVEL doesnt go up as fast as MAKELEVEL, and so submakes work out
# looking better with less explicit setting
export MAKE_INDENT_LEVEL?=$(MAKELEVEL)

# intentionally DONT set export for this cuz it could vary from makefile to makefile
SHELL?=/bin/bash

.DEFAULT_GOAL := all

.PHONY:			all clean clobber check
