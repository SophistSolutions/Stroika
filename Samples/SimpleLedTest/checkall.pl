if ("$^O" eq "linux") {
	# nothing to check - not supported on Linux
}
else {
	CheckFileExists ("../../Builds/Windows/Debug-U-32/Samples-SimpleLedTest/SimpleLedTest.exe");
	CheckFileExists ("../../Builds/Windows/Release-U-32/Samples-SimpleLedTest/SimpleLedTest.exe");
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
