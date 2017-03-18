include ../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir					=       $(StroikaRoot)Library/Sources/Stroika/Frameworks/
ObjDir					=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk



VPATH		=	$(SrcDir) $(StroikaLibDir)


SubDirs=	\
		Modbus\
		NetworkMonitor\
		Service\
		SystemPerformance\
		WebServer\
		UPnP\

.PHONY:	$(SubDirs) all_objs_ all_objs_ all


ALL_OBJS := $(foreach dir, $(SubDirs), $(shell $(MAKE) --directory $(dir) -s list-objs CONFIGURATION=$(CONFIGURATION)))


all-objs:
	@echo $(ALL_OBJS)


$(ObjDir)%.o : %.cpp
	@#suppress normal rule so just all_objs_ fires


$(ALL_OBJS): all_objs_

all_objs_:
	@$(StroikaRoot)ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo "Building Stroika Frameworks Objs:";
	@(for i in $(SubDirs); do $(MAKE) --directory $$i --no-print-directory -s all MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) CONFIGURATION=$(CONFIGURATION); done;);


$(StroikaFrameworksLib):	$(ALL_OBJS)
	@mkdir -p $(StroikaLibDir)
	@$(StroikaRoot)ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Creating $(subst $(StroikaRoot),\$$StroikaRoot/, $@)..."
	@rm -f $(StroikaFrameworksLib)
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
		echo "      $(AR) cr $(StroikaFrameworksLib) $(ALL_OBJS)";\
	fi
	@$(AR) cr $(StroikaFrameworksLib) $(ALL_OBJS)
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
		echo "      $(RANLIB) $(StroikaFrameworksLib)";\
	fi
	@$(RANLIB) $(StroikaFrameworksLib)


all:	$(StroikaFrameworksLib)


clean::
	@for i in $(SubDirs);\
	do\
	     $(MAKE) --directory $$i --no-print-directory -s clean CONFIGURATION=$(CONFIGURATION);\
	done

clobber::
	do\
	    $(MAKE) --directory $$i --no-print-directory -s clobber CONFIGURATION=$(CONFIGURATION);\
	done
	@rm -f $(StroikaFrameworksLib)


Build:	all


