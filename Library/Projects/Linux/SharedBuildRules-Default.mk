.cpp.o:
	@echo "Compiling $<"
	@$(CPlusPlus) $(CFLAGS) $< -o $@


.swsp.i:
	@echo "Compiling $<"
	@$(HTMLViewCompiler) $< $@


.DEFAULT_GOAL := all


clean::
	@rm -f $(Objs)


clobber::	clean



list-objs::
	@echo $(Objs)


# default build rule - map (windowsish) top-level build arg to unixish name 'all'
Build:	all

