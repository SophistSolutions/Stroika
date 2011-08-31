CheckFileExists ("../../Lib/Debug-A-32/Stroika-Foundation.lib");
CheckFileExists ("../../Lib/Release-A-32/Stroika-Foundation.lib");

CheckFileExists ("../../Lib/Debug-U-32/Stroika-Foundation.lib");
CheckFileExists ("../../Lib/Debug-U-64/Stroika-Foundation.lib");
CheckFileExists ("../../Lib/Release-U-32/Stroika-Foundation.lib");
CheckFileExists ("../../Lib/Release-U-64/Stroika-Foundation.lib");

CheckFileExists ("../../Lib/Release-Logging-U-32/Stroika-Foundation.lib");
CheckFileExists ("../../Lib/Release-Logging-U-64/Stroika-Foundation.lib");

CheckFileExists ("../../Lib/Release-DbgMemLeaks-U-32/Stroika-Foundation.lib");


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