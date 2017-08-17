include ../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir					=	$(StroikaRoot)Library/Sources/Stroika/Foundation/
ObjDir					=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk


.PHONY: all_objs_ all


VPATH		=	$(SrcDir) $(StroikaLibDir)

SubDirs=				\
	Cache				\
	Characters			\
	Common				\
	Configuration		\
	Containers			\
	Cryptography		\
	Database			\
	DataExchange		\
	Debug				\
	Execution			\
	IO					\
	Linguistics			\
	Math				\
	Memory				\
	Streams				\
	Time				\
	Traversal			\


ALL_OBJS := $(foreach dir, $(SubDirs), $(shell $(MAKE) --directory $(dir) -s list-objs CONFIGURATION=$(CONFIGURATION)))


all-objs:
	@echo $(ALL_OBJS)

$(ALL_OBJS):	all_objs_


$(ObjDir)%.o : %.cpp
	@#suppress normal rule so just all_objs_ fires


all_objs_:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Building Stroika Foundation Objs:";
	@(for i in $(SubDirs); do $(MAKE) --directory $$i --no-print-directory -s all MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) CONFIGURATION=$(CONFIGURATION); done;);


$(StroikaFoundationLib):	$(ALL_OBJS)
	@mkdir -p $(StroikaLibDir)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Creating $(subst $(StroikaRoot),\$$StroikaRoot/, $@)..."
	@rm -f $(StroikaFoundationLib)
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
		echo "      $(AR) cr $(StroikaFoundationLib) $(ALL_OBJS)";\
	fi
	@$(AR) cr $(StroikaFoundationLib) $(ALL_OBJS)
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
		echo "      $(RANLIB) $(StroikaFoundationLib)";\
	fi
	@$(RANLIB) $(StroikaFoundationLib)


all:	$(StroikaFoundationLib)


clean::
	@for i in $(SubDirs);\
	do\
	    $(MAKE) --directory $$i --no-print-directory -s clean CONFIGURATION=$(CONFIGURATION);\
	done


clobber::
	@rm -f $(StroikaFoundationLib)


Build:	all

