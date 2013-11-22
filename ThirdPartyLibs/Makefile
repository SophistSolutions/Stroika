include ../IntermediateFiles/DefaultConfiguration/Library/Configuration.mk

.PHONY:	all clean clobber check

all:
	@(cd doxygen; perl buildall.pl Build)
	make --directory openssl --no-print-directory all
        ifeq ($(qHasLibrary_Xerces), 1)
	@make --directory Xerces --no-print-directory all
        endif
	@make --directory zlib --no-print-directory all

clean:
	@(cd doxygen; perl buildall.pl clean)
	make --directory openssl --no-print-directory clean
	@make --directory Xerces --no-print-directory clean
	@make --directory zlib --no-print-directory clean

check:
	@./checkall.pl

clobber:
	@(cd doxygen; perl buildall.pl clobber)
	make --directory openssl --no-print-directory clobber
	@make --directory Xerces --no-print-directory clobber
	@make --directory zlib --no-print-directory clobber
