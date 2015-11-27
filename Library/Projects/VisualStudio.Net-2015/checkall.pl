my $activeConfig = $ENV{'CONFIGURATION'};

## COULD also check PDB and IDB files but not needed
CheckFileExists ("../../../Builds/$activeConfig/Library/Stroika-Foundation.lib");
CheckFileExists ("../../../Builds/$activeConfig/Library/Stroika-Frameworks-Led.lib");
CheckFileExists ("../../../Builds/$activeConfig/Library/Stroika-Frameworks-Service.lib");
CheckFileExists ("../../../Builds/$activeConfig/Library/Stroika-Frameworks-SystemPerformance.lib");
CheckFileExists ("../../../Builds/$activeConfig/Library/Stroika-Frameworks-UPnP.lib");
CheckFileExists ("../../../Builds/$activeConfig/Library/Stroika-Frameworks-WebServer.lib");

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
