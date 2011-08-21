.cpp.o:
	@echo "Compiling $<"
	@gcc $(CFLAGS) $< -o $@


.DEFAULT_GOAL := all


clean::	$(Objs)
	@rm -f $(Objs)



# default build rule - map (windowsish) top-level build arg to unixish name 'all'
Build:	all

