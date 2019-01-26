#
#
#

.SUFFIXES:	${OBJ_SUFFIX} .cpp .i .h .swsp .a


$(ObjDir):
	@mkdir -p $(ObjDir)


$(Objs):	| $(ObjDir)


$(ObjDir)%${OBJ_SUFFIX} : %.cpp
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Compiling $(shell $(StroikaRoot)ScriptsLib/SubstituteBackVariables $(abspath  $<)) ... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "$(CXX) $(CXXFLAGS) -c $< -o $@";\
	fi
	@mkdir -p `dirname $@`
	@$(CXX) $(CXXFLAGS) -c $< -o $@


%.i : %.swsp
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Compiling $(shell $(StroikaRoot)ScriptsLib/SubstituteBackVariables $(abspath  $<)) ... "
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
