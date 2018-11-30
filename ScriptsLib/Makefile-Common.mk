#Common bits of script / functions that maybe used in Stroika makefiles.
#This has no dependencies (besides gnu make)

FUNCTION_CAPITALIZE_WORD=$(shell echo $1 | tr '[:lower:]' '[:upper:]' | cut -c 1-1)$(shell echo $1 | cut -c 2-99)
