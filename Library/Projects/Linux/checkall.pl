#!/usr/bin/perl
#EVENTUALYL fix to do  stuff like this
#CheckFileExists ("../../../Builds/Windows/Lib/Debug-A-32/Stroika-Foundation.lib");
#CheckFileExists ("../../../Builds/Windows/Lib/Release-A-32/Stroika-Foundation.lib");

#CheckFileExists ("../../../Builds/Windows/Lib/Debug-U-32/Stroika-Foundation.lib");
#CheckFileExists ("../../../Builds/Windows/Lib/Debug-U-64/Stroika-Foundation.lib");
#CheckFileExists ("../../../Builds/Windows/Lib/Release-U-32/Stroika-Foundation.lib");
#CheckFileExists ("../../../Builds/Windows/Lib/Release-U-64/Stroika-Foundation.lib");

#CheckFileExists ("../../../Builds/Windows/Lib/Release-Logging-U-32/Stroika-Foundation.lib");
#CheckFileExists ("../../../Builds/Windows/Lib/Release-Logging-U-64/Stroika-Foundation.lib");

#CheckFileExists ("../../../Builds/Windows/Lib/Release-DbgMemLeaks-U-32/Stroika-Foundation.lib");


CheckFileExists ("../../../Builds/DefaultConfiguration/Stroika-Foundation.a");
CheckFileExists ("../../../Builds/DefaultConfiguration/Stroika-Frameworks.a");


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
