#
#
#
ifndef StroikaRoot
$(error "StroikaRoot must be defined before is included SharedBuildRules-Default.mk")
endif

# Test some random variable defined in SharedMakeVariables-Default.mk to ensure it has been included
ifndef DETECTED_HOST_OS
$(warning "SharedMakeVariables-Default.mk must be included before SharedBuildRules-Default.mk")
endif


#
# See https://www.gnu.org/software/make/manual/html_node/Reading-Makefiles.html#:~:text=GNU%20make%20does%20its%20work,needed%20during%20the%20second%20phase.
#
#	- Make variables used in the RULES are evaluated at the time the rules are executed (deferred).
#   - But "Targets and Prerequisites: Variables used in targets and prerequisites are expanded during the parsing phase."
#
#	So this means that
#      - ObjDir
#      - Objs
#	   - OBJ_SUFFIX
#   all must be defined/finalized before the include of this file.
# 
ifndef ObjDir
$(warning "ObjDir must be defined before including SharedBuildRules-Default.mk")
endif
# DONT warn about Objs for now (may want to fix - but minor)
# ifndef Objs
# $(warning "Objs must be defined before including SharedBuildRules-Default.mk")
# endif
# DONT warn about OBJ_SUFFIX cuz typically just means Configuration.mk not built - probbaly due to doing a clobber)
# ifndef OBJ_SUFFIX
# $(warning "OBJ_SUFFIX must be defined before including SharedBuildRules-Default.mk")
# endif


.SUFFIXES:	${OBJ_SUFFIX} .cpp .i .h .swsp .a

.PHONY:			all clean clobber check


# 
# Though it might make sense to spread this around to just the makefiles that need it, that would be onerous
# and involve patching many files, so just apply it here where its likely included by all relevant
# makefiles (and those not including this includefile can replate/call this directly themselves)
#
$(call PATCH_PATH_FOR_TOOLPATH_ADDITION_IF_NEEDED)


$(ObjDir):
	@mkdir -p $(ObjDir)


ifdef Objs
$(Objs):	| $(ObjDir)
endif

ifdef OBJ_SUFFIX
$(ObjDir)%${OBJ_SUFFIX} : %.cpp
	@$(StroikaRoot)Build/ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Compiling $(shell $(StroikaRoot)Build/ScriptsLib/SubstituteBackVariables $(abspath $<)) ... "
	@mkdir -p `dirname $@`
	@if [ $(WRITE_PREPROCESSOR_OUTPUT) -eq 1 ]; then\
		if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
			$(StroikaRoot)Build/ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_CPP_LINE,$<,$@)';\
		fi;\
	   $(call DEFAULT_CPP_LINE,$<,$@); \
	fi
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)Build/ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_CXX_LINE,$<,$@)';\
	fi
	@$(call DEFAULT_CXX_LINE,$<,$@)
endif
	


%.i : %.swsp
	@$(StroikaRoot)Build/ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Compiling $(shell $(StroikaRoot)Build/ScriptsLib/SubstituteBackVariables $(abspath $<)) ... "
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
