sub	DoCheckBuilds {
	my $testNumber = $_[0];
	
	CheckFileExists ("../../../../Builds/Windows/Debug-A-32/Test$testNumber/Test$testNumber.exe");
	CheckFileExists ("../../../../Builds/Windows/Release-A-32/Test$testNumber/Test$testNumber.exe");
	
	CheckFileExists ("../../../../Builds/Windows/Debug-U-32/Test$testNumber/Test$testNumber.exe");
	CheckFileExists ("../../../../Builds/Windows/Release-U-32/Test$testNumber/Test$testNumber.exe");
	CheckFileExists ("../../../../Builds/Windows/Debug-U-64/Test$testNumber/Test$testNumber.exe");
	CheckFileExists ("../../../../Builds/Windows/Release-U-64/Test$testNumber/Test$testNumber.exe");
	
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