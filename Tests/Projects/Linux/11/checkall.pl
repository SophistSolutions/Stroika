#CheckFileExists ("../../../../Builds/Windows/Test1/Debug-A-32/Test.exe");
#CheckFileExists ("../../../../Builds/Windows/Test1/Release-A-32/Test.exe");

#CheckFileExists ("../../../../Builds/Windows/Test1/Debug-U-32/Test.exe");
#CheckFileExists ("../../../../Builds/Windows/Test1/Release-U-32/Test.exe");
#CheckFileExists ("../../../../Builds/Windows/Test1/Debug-U-64/Test.exe");
#CheckFileExists ("../../../../Builds/Windows/Test1/Release-U-64/Test.exe");

CheckFileExists ("../../../../Builds/Platform_Linux/Test11");

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
