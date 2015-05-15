#
#
#

.PHONY:			all clean clobber list-objs Build

.DEFAULT_GOAL := all

.SUFFIXES:	.o .cpp .i .h .swsp .a



%.o : %.cpp
	@echo "      Compiling $<"
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    echo "      $(CPlusPlus) $(CFLAGS) -c $< -o $@";\
	fi
	@$(CPlusPlus) $(CFLAGS) -c $< -o $@


%.i : %.swsp
	@echo "Compiling $<"
	@$(HTMLViewCompiler) $< $@


clean::
	@rm -f $(Objs)


clobber::	clean



list-objs::
	@echo $(Objs)


# default build rule - map (windowsish) top-level build arg to unixish name 'all'
Build:	all

