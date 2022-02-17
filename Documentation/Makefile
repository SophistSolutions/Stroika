StroikaRoot=$(abspath ..)/

MAKE_INDENT_LEVEL?=$(MAKELEVEL)

.PHONY:	all doxygen

all:
	@if [ ! -e $(StroikaRoot)/Builds/Stroika-Doxygen-Help.tar.gz ] ; then\
		if which doxygen 2>&1 > /dev/null; then\
			make --silent doxygen;\
		else\
			$(StroikaRoot)/ScriptsLib/PrintLevelLeader $(MAKE_INDENT_LEVEL) && echo "Skipping build of documentation since Doxygen not installed";\
		fi;\
	fi

clobber: clean
	@rm -f $(StroikaRoot)/Builds/Stroika-Doxygen-Help.tar.gz

clean:
	@rm -rf Doxygen/Stroika-Doxygen-Help.tar.gz Doxygen/Output Doxygen/IntermediateFiles

doxygen:
	@$(StroikaRoot)/ScriptsLib/PrintLevelLeader $(MAKE_INDENT_LEVEL) && echo -n "Building Doxygen Documentation (Builds/Stroika-Doxygen-Help.tar.gz; see Documentation/Doxygen/BUILD.OUT)..."
	@rm -rf Doxygen/Output Doxygen/IntermediateFiles
	@cd Doxygen && doxygen Stroika-Library.cfg >BUILD.OUT 2>&1
	@cd Doxygen/IntermediateFiles; \
		mv html Stroika-Doxygen-Help; \
		tar cf Stroika-Doxygen-Help.tar Stroika-Doxygen-Help; \
		gzip --best Stroika-Doxygen-Help.tar; \
		mv Stroika-Doxygen-Help.tar.gz $(StroikaRoot)/Builds/
	@echo done
