
my $activeConfig = $ENV{'CONFIGURATION'};


CheckFileExists ("../../../Builds/$activeConfig/HTMLViewCompiler");


print "      [SUCCEEDED]\n";


sub	CheckFileExists {
	local $file = $_[0];
	if (not (-e $file)) {
		Failed ("$file does not exist");
	}
}


sub Failed {
	local $reason = $_[0];
	print "[FAILED]: $reason\n";
	exit(1);
}
