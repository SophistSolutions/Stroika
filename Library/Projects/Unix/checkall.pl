#!/usr/bin/perl

my $activeConfig = $ENV{'CONFIGURATION'};

CheckFileExists ("../../../Builds/$activeConfig/Stroika-Foundation.a");
CheckFileExists ("../../../Builds/$activeConfig/Stroika-Frameworks.a");


print "      [SUCCEEDED]\r\n";


sub	CheckFileExists {
	local $file = $_[0];
	if (not (-e $file)) {
		Failed ("$file does not exist");
	}
}


sub Failed {
	local $reason = $_[0];
	print "[FAILED]: $reason\r\n";
	exit(1);
}
