MAKE_INDENT_LEVEL?=$(MAKELEVEL)


.PHONY:	all

all:
	@if [ ! -e ../Builds/Stroika-Doxygen-Help.tar.gz ] ; then\
		if which doxygen 2>&1 > /dev/null; then\
			export MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL); cd Doxygen && perl ./RunDoxygen.pl;\
		else\
			../ScriptsLib/PrintLevelLeader $(MAKE_INDENT_LEVEL) && echo "Skipping build of documentation since Doxygen not installed";\
		fi;\
	fi

clobber:
	@rm -f Doxygen/Stroika-Doxygen-Help.tar.gz
