#
#
#

.SUFFIXES:	${OBJ_SUFFIX} .cpp .i .h .swsp .a


$(ObjDir):
	@mkdir -p $(ObjDir)


$(Objs):	| $(ObjDir)


ifeq (Unix,$(ProjectPlatformSubdir))
DASH_O_PARAMS_= -o $1
OUTPUT_FILTER_=
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
DASH_O_PARAMS_=/Fo$(shell cygpath -m $1)
OUTPUT_FILTER_=|sed -n '1!p'
endif


$(ObjDir)%${OBJ_SUFFIX} : %.cpp
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Compiling $(shell $(StroikaRoot)ScriptsLib/SubstituteBackVariables $(abspath  $<)) ... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) $(CXX) $(CXXFLAGS) -c $< $(call DASH_O_PARAMS_, $@);\
	fi
	@mkdir -p `dirname $@`
	@$(CXX) $(CXXFLAGS) -c $< $(call DASH_O_PARAMS_, $@) $(OUTPUT_FILTER_)


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
