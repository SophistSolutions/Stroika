include ../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir					=	$(StroikaRoot)Library/Sources/Stroika/Foundation/
ObjDir					=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk



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


# In order to get the make dependency rule for $(StroikaFoundationLib) to work for all objs,
# and yet to not re-run the make subdir objs rule too many times, this was the best trick
# I could come up with to just run the submakefiles once...
ALL_OBJS_DONE_HACK=0
$(ALL_OBJS):
	@if [ $(ALL_OBJS_DONE_HACK) -eq 0 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Building Stroika Foundation Objs...";\
	    (for i in $(SubDirs); do $(MAKE) --directory $$i --no-print-directory -s all MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) CONFIGURATION=$(CONFIGURATION); done;);\
	fi
	@$(eval ALL_OBJS_DONE_HACK := 1)


$(StroikaFoundationLib):	$(ALL_OBJS)
	@mkdir -p $(StroikaLibDir)
	@$(StroikaRoot)ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo "Creating $(subst $(StroikaRoot),\$$StroikaRoot/, $@)..."
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
	    $(MAKE) --directory $$i --no-print-directory -s clean;\
	done


clobber::
	@rm -f $(StroikaFoundationLib)


Build:	all

