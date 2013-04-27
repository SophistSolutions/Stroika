all:
	./buildall.pl build

clean:
	./buildall.pl clean


clobber:
	./buildall.pl clobber


tests:
	./buildall.pl build+

