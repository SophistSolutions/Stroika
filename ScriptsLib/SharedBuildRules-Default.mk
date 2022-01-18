#
#
#
ifndef StroikaRoot
$(error "StroikaRoot must be defined and included before this file")
endif

.SUFFIXES:	${OBJ_SUFFIX} .cpp .i .h .swsp .a


# 
# Though it might make sense to spread this around to just the makefiles that need it, that would be onerous
# and involve patching many files, so just apply it here where its likely included by all relevant
# makefiles (and those not including this includefile can replate/call this directly themselves)
#
$(call PATCH_PATH_FOR_TOOLPATH_ADDITION_IF_NEEDED)


$(ObjDir):
	@mkdir -p $(ObjDir)


$(Objs):	| $(ObjDir)


$(ObjDir)%${OBJ_SUFFIX} : %.cpp
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Compiling $(shell $(StroikaRoot)ScriptsLib/SubstituteBackVariables $(abspath $<)) ... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_CXX_LINE,$<,$@)';\
	fi
	@mkdir -p `dirname $@`
	@$(call DEFAULT_CXX_LINE,$<,$@)


%.i : %.swsp
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Compiling $(shell $(StroikaRoot)ScriptsLib/SubstituteBackVariables $(abspath $<)) ... "
	@$(HTMLViewCompiler) $< $@


clean::
	@rm -f $(Objs)


clobber::	clean
	@rm -rf $(TARGETDIR)


list-objs::
	@echo $(Objs)
	@for sd in $(SubDirs);\
	do\
		$(MAKE) --directory $$sd -s list-objs;\
	done
