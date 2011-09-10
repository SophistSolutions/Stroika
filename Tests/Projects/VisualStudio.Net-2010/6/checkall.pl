CheckFileExists ("../../../../Builds/Windows/Debug-A-32/Test6/Test6.exe");
CheckFileExists ("../../../../Builds/Windows/Release-A-32/Test6/Test6.exe");

CheckFileExists ("../../../../Builds/Windows/Debug-U-32/Test6/Test6.exe");
CheckFileExists ("../../../../Builds/Windows/Release-U-32/Test6/Test6.exe");
CheckFileExists ("../../../../Builds/Windows/Debug-U-64/Test6/Test6.exe");
CheckFileExists ("../../../../Builds/Windows/Release-U-64/Test6/Test6.exe");

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