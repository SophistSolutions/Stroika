#
#
#

.PHONY:			all clean clobber list-objs Build

.DEFAULT_GOAL := all

.SUFFIXES:	.o .cpp .i .h .swsp .a



%.o : %.cpp
	@$(StroikaRoot)ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Compiling $(subst $(StroikaRoot),\$$StroikaRoot/, $<)..."
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && $(ECHO) "$(CPlusPlus) $(CFLAGS) -c $< -o $@";\
	fi
	@$(CPlusPlus) $(CFLAGS) -c $< -o $@


%.i : %.swsp
	@$(StroikaRoot)ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Compiling $(subst $(StroikaRoot),\$$StroikaRoot/, $<)..."
	@$(HTMLViewCompiler) $< $@


clean::
	@rm -f $(Objs)


clobber::	clean



list-objs::
	@echo $(Objs)


# default build rule - map (windowsish) top-level build arg to unixish name 'all'
#Build:	all

