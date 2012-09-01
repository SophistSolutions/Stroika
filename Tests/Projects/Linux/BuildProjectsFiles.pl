#!/usr/bin/perl 
use File::Copy;

require "../../ScriptsLib/TestsList.pl";


sub	CopyWithWithSubst {

	my $srcFile = $_[0];
	my $trgFile = $_[1];
	my $substValue = $_[2];

	open (FILE, "$srcFile") or die("Unable to open $srcFile");
	my @data = <FILE>;
	close(FILE);
	
	open FILE, ">", "$trgFile" or die $!;
	
	foreach $line (@data) {
		$line =~ s/NNN/$substValue/;
		print FILE $line;
	}
}



sub	DoDirN {
	my $tst = $_[0];
	mkdir ($tst);
	CopyWithWithSubst ("Project-Template/checkall.pl","$tst/checkall.pl", $tst);
	CopyWithWithSubst ("Project-Template/Run.pl","$tst/Run.pl", $tst);
	CopyWithWithSubst ("Project-Template/TestN.Makefile","$tst/Makefile", $tst);
}

sub DoRun {
	foreach $test (GetAllTests ()) {
		DoDirN ($test);
	}
}


DoRun();