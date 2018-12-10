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
	@$(StroikaRoot)ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Compiling $(shell $(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh `realpath $<`) ... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) "$(CXX) $(CXXFLAGS) -c $< -o $@";\
	fi
	@mkdir -p `dirname $@`
	@$(CXX) $(CFLAGS) -c $< -o $@

$(ObjDir)%.o : %.cpp
	@$(StroikaRoot)ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Compiling $(shell $(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh `realpath $<`) ... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) "$(CXX) $(CXXFLAGS) -c $< -o $@";\
	fi
	@mkdir -p `dirname $@`
	@$(CXX) $(CFLAGS) -c $< -o $@


%.i : %.swsp
	@$(StroikaRoot)ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Compiling $(shell $(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh `realpath $<`) ... "
	@$(HTMLViewCompiler) $< $@


clean::
	@rm -f $(Objs)


clobber::	clean


list-objs::
	@echo $(Objs)
	@for sd in $(SubDirs);\
	do\
		$(MAKE) --directory $$sd -s list-objs;\
	done


# default build rule - map (windowsish) top-level build arg to unixish name 'all'
#Build:	all

