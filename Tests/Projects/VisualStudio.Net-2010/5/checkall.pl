CheckFileExists ("../../../../Builds/Windows/Test5/Debug-A-32/Test.exe");
CheckFileExists ("../../../../Builds/Windows/Test5/Release-A-32/Test.exe");

CheckFileExists ("../../../../Builds/Windows/Test5/Debug-U-32/Test.exe");
CheckFileExists ("../../../../Builds/Windows/Test5/Release-U-32/Test.exe");
CheckFileExists ("../../../../Builds/Windows/Test5/Debug-U-64/Test.exe");
CheckFileExists ("../../../../Builds/Windows/Test5/Release-U-64/Test.exe");

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