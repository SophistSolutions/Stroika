CheckFileExists ("../../../Builds/Debug-U-32/Library/Stroika-Foundation.lib");
CheckFileExists ("../../../Builds/Debug-U-64/Library/Stroika-Foundation.lib");
CheckFileExists ("../../../Builds/Release-U-32/Library/Stroika-Foundation.lib");
CheckFileExists ("../../../Builds/Release-U-64/Library/Stroika-Foundation.lib");

CheckFileExists ("../../../Builds/Release-Logging-U-32/Library/Stroika-Foundation.lib");
CheckFileExists ("../../../Builds/Release-Logging-U-64/Library/Stroika-Foundation.lib");

CheckFileExists ("../../../Builds/Release-DbgMemLeaks-U-32/Library/Stroika-Foundation.lib");


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
