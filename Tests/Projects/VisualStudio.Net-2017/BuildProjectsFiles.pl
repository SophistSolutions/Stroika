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
	copy("Project-Template/.gitignore","$tst/.gitignore") or die "Copy failed: $!";
	CopyWithWithSubst ("Project-Template/TestN.vcxproj","$tst/Test$tst.vcxproj", $tst);
	CopyWithWithSubst ("Project-Template/TestN.vcxproj.filters","$tst/Test$tst.vcxproj.filters", $tst);
	
}

sub DoRun {
	foreach $test (GetAllTests ()) {
		DoDirN ($test);
	}
}


DoRun();
