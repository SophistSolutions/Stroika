#!/usr/bin/perl

my $date = "2013-02-25";
system ("svn export https://astyle.svn.sourceforge.net/svnroot/astyle/trunk astyle-trunk");
system ("tar cvf astyle-trunk-$date.tar astyle-trunk");
system ("gzip --best astyle-trunk-$date.tar");
system ("mv astyle-trunk-$date.tar.gz Origs/");
system ("rm -rf astyle-trunk");
