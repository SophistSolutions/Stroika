require "../../../../ScriptsLib/ConfigurationReader.pl";

sub	DoCheckBuilds {
	my $testNumber = $_[0];
	
	my $activeConfig = $ENV{'CONFIGURATION'};
	my $projectPlatformSubdir = GetProjectPlatformSubdir($activeConfig);
	if (index($projectPlatformSubdir, "VisualStudio") != -1) {
		CheckFileExists ("../../../../Builds/$activeConfig/Test$testNumber/Test$testNumber.exe");
	}
	else {
		CheckFileExists ("../../../../Builds/$activeConfig/Test$testNumber/Test$testNumber");
	}
	
	print "[SUCCEEDED]\r\n";
	
	
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
	
}

1
