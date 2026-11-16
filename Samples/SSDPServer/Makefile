StroikaSourceRoot	:=	$(abspath ../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Samples/SSDPServer/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Samples-SSDPServer/


TARGETDIR		:=	${StroikaPlatformTargetBuildDir}Samples-SSDPServer/
TARGETEXE		:=	$(TARGETDIR)SSDPServer${EXE_SUFFIX}


Objs	:=	\
	$(ObjDir)SSDPServer${OBJ_SUFFIX}\


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


VPATH	=			$(SrcDir)Sources/


all:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/SSDPServer:"
	@mkdir -p $(TARGETDIR)
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))


check:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Samples/SSDPServer...               "
	@$(StroikaSourceRoot)Build/Scripts/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]";


# Cannot be done generically through configuration cuz a given build of Stroika might contain EXEs targetting multiple subsystems
ifeq (Windows,$(findstring Windows,$(TargetPlatforms)))
EXTRALINKARGS_	+= -SUBSYSTEM:CONSOLE
endif


$(TARGETEXE):	$(ObjDir) $(Objs) $(StroikaLibs)
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Linking `$(StroikaSourceRoot)Build/Scripts/SubstituteBackVariables $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_LINK_LINE, $@, stroika-frameworks, ${EXTRALINKARGS_})';\
	fi
	@$(call DEFAULT_LINK_LINE, $@, stroika-frameworks, ${EXTRALINKARGS_})


