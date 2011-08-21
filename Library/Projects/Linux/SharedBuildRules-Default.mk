.cpp.o:
	@echo "Compiling $<"
	@gcc $(CFLAGS) $< -o $@


.DEFAULT_GOAL := all


clean::	$(Objs)
	@rm -f $(Objs)



