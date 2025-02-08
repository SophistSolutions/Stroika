StroikaRoot=$(abspath ..)/

MAKE_INDENT_LEVEL?=$(MAKELEVEL)

INTERMEDIATE_FILES=$(abspath ../IntermediateFiles/Doxygen)

.PHONY:	all doxygen

all:
	@if [ ! -e $(StroikaRoot)/Builds/Stroika-Doxygen-Help.tar.gz ] ; then\
		if which doxygen 2>&1 > /dev/null; then\
			$(MAKE) --silent doxygen;\
		else\
			$(StroikaRoot)/ScriptsLib/PrintLevelLeader $(MAKE_INDENT_LEVEL) && echo "Skipping build of documentation since Doxygen not installed";\
		fi;\
	fi

clobber: clean
	@rm -f $(StroikaRoot)/Builds/Stroika-Doxygen-Help.tar.gz

clean:
	@rm -rf Doxygen/Stroika-Doxygen-Help.tar.gz ${INTERMEDIATE_FILES}

doxygen:
	@$(StroikaRoot)/ScriptsLib/PrintLevelLeader $(MAKE_INDENT_LEVEL) && echo -n "Building Doxygen Documentation (Builds/Stroika-Doxygen-Help.tar.gz; see IntermediateFiles/Doxygen/BUILD.OUT)..."
	@rm -rf ${INTERMEDIATE_FILES}
	@mkdir -p ${INTERMEDIATE_FILES}
	@cd Doxygen && doxygen Stroika-Library.cfg >${INTERMEDIATE_FILES}/BUILD.OUT 2>&1
	@cd ${INTERMEDIATE_FILES}; \
		mv html Stroika-Doxygen-Help; \
		tar cf Stroika-Doxygen-Help.tar Stroika-Doxygen-Help; \
		gzip --best Stroika-Doxygen-Help.tar; \
		mkdir -p $(StroikaRoot)/Builds/; \
		mv Stroika-Doxygen-Help.tar.gz $(StroikaRoot)/Builds/
	@echo done
