#
#
#

.PHONY:			all clean clobber list-objs Build


.SUFFIXES:	.o .cpp .i .h .swsp .a


$(ObjDir):
	@mkdir -p $(ObjDir)


# Because of this rule (and others) - its important that Objs 'recursively expanded'
$(Objs):	$(ObjDir)



### As of Stroika v2.1d15 disabled
#%.o : %.cpp
#	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Compiling $(shell $(StroikaRoot)ScriptsLib/SubstituteBackVariables `realpath $<`) ... "
#	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
#	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "$(CXX) $(CXXFLAGS) -c $< -o $@";\
#	fi
#	@mkdir -p `dirname $@`
#	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(ObjDir)%.o : %.cpp
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Compiling $(shell $(StroikaRoot)ScriptsLib/SubstituteBackVariables `realpath $<`) ... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "$(CXX) $(CXXFLAGS) -c $< -o $@";\
	fi
	@mkdir -p `dirname $@`
	@$(CXX) $(CXXFLAGS) -c $< -o $@


%.i : %.swsp
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Compiling $(shell $(StroikaRoot)ScriptsLib/SubstituteBackVariables `realpath $<`) ... "
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
