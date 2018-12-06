#Common bits of script / functions that maybe used in Stroika makefiles.
#This has no dependencies (besides gnu make)

#NB: we export variables like ECHO since they dont change from makefile to makefile and pass the values to submakes and avoid being recomputed/re-fetched(performance tweek on make)

ifndef StroikaRoot
$(error("StroikaRoot must be defined and included before this file"))
endif

FUNCTION_CAPITALIZE_WORD=$(shell echo $1 | tr '[:lower:]' '[:upper:]' | cut -c 1-1)$(shell echo $1 | cut -c 2-99)

export ECHO?=	$(shell $(StroikaRoot)ScriptsLib/GetDefaultShellVariable.sh ECHO)
