#!/usr/bin/env perl
use File::Copy;
use File::Compare;

require "../../ScriptsLib/TestsList.pl";


sub	CopyWithWithSubst {

	my $srcFile = $_[0];
	my $trgFile = $_[1];
	my $substValue = $_[2];

	open (FILE, "$srcFile") or die("Unable to open $srcFile");
	my @data = <FILE>;
	close(FILE);
	
	my $trgFileTmp = $trgFile . ".tmp";
	open FILE, ">", "$trgFileTmp" or die $!;
	
	foreach $line (@data) {
		$line =~ s/NNN/$substValue/;
		print FILE $line;
	}
	close(FILE);

	if (compare($trgFileTmp, $trgFile) == 0) {
		#print ("$trgFile unchanged\n");
		unlink ($trgFileTmp);
	}
	else {
		#print ("$trgFile updated\n");
		rename ($trgFileTmp, $trgFile);
	}
}



sub	DoDirN {
	my $tst = $_[0];
	mkdir ($tst);
	CopyWithWithSubst ("Project-Template/TestN.Makefile","$tst/Makefile", $tst);
}

sub DoRun {
	foreach $test (GetAllTests ()) {
		DoDirN ($test);
	}
}


DoRun();