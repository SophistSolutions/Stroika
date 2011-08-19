.cpp.o:
	@echo "Compiling $<"
	@gcc $(CFLAGS) $< -o $@


clean::	$(Objs)
	@rm -f $(Objs)



