StroikaRoot	:=	$(abspath ..)/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk


all:
ifeq ($(CONFIGURATION),)
	@for i in `../Build/Scripts/GetConfigurations` ; do\
		$(MAKE) --no-print-directory CONFIGURATION=$$i all || exit $$?;\
	done
else
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Stroika Library {$(CONFIGURATION)}:"
	@$(MAKE) --directory Sources/Stroika/Foundation/ --no-print-directory --silent all MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Sources/Stroika/Frameworks/ --no-print-directory --silent all MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif


check:
ifeq ($(CONFIGURATION),)
	@for i in `../Build/Scripts/GetConfigurations` ; do\
		$(MAKE) --no-print-directory CONFIGURATION=$$i check || exit $$?;\
	done
else
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Checking Stroika Library {$(CONFIGURATION)}:      "
	@$(StroikaRoot)Build/Scripts/CheckFileExists $(StroikaFoundationLib) $(StroikaFrameworksLib)
	@$(ECHO) "[SUCCEEDED]"
endif



clean clobber:
ifeq ($(CONFIGURATION),)
	@for i in `../Build/Scripts/GetConfigurations` ; do\
		$(MAKE) --no-print-directory CONFIGURATION=$$i $@;\
	done
else
	@../Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Stroika/Library $(call FUNCTION_CAPITALIZE_WORD,$@) {$(CONFIGURATION)} ... "
	@$(MAKE) --directory Sources/Stroika/Foundation --no-print-directory $@
	@$(MAKE) --directory Sources/Stroika/Frameworks --no-print-directory $@
	@$(ECHO) done
endif

