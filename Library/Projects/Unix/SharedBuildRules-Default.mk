#
#
#

.PHONY:			all clean clobber list-objs Build

.DEFAULT_GOAL := all

.SUFFIXES:	.o .cpp .i .h .swsp .a



$(ObjDir):
	@mkdir -p $(ObjDir)

### not sure we need this one anymore...
%.o : %.cpp
	@$(StroikaRoot)ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Compiling $(subst $(StroikaRoot),\$$StroikaRoot/, $(shell realpath $<))..."
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && $(ECHO) "$(CPlusPlus) $(CFLAGS) -c $< -o $@";\
	fi
	@mkdir -p `dirname $@`
	@$(CPlusPlus) $(CFLAGS) -c $< -o $@

$(ObjDir)%.o : %.cpp
	@$(StroikaRoot)ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Compiling $(subst $(StroikaRoot),\$$StroikaRoot/, $(shell realpath $<))..."
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && $(ECHO) "$(CPlusPlus) $(CFLAGS) -c $< -o $@";\
	fi
	@mkdir -p `dirname $@`
	@$(CPlusPlus) $(CFLAGS) -c $< -o $@


%.i : %.swsp
	@$(StroikaRoot)ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Compiling $(subst $(StroikaRoot),\$$StroikaRoot/, $<)..."
	@$(HTMLViewCompiler) $< $@


clean::
	@rm -f $(Objs)


clobber::	clean


list-objs::
	@echo $(Objs)
	@for sd in $(SubDirs);\
	do\
		$(MAKE) --directory $$sd -s list-objs CONFIGURATION=$(CONFIGURATION);\
	done


# default build rule - map (windowsish) top-level build arg to unixish name 'all'
#Build:	all

