#!/usr/bin/perl 

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

print ("Building library...\n");

my $useBld = lc ($BLD_TRG);
if ("$^O" eq "linux") {
	if ($useBld eq "build") {
		$useBld = "all";
	}
	if ($useBld eq "rebuild") {
	    system ("cd ../IntermediateFiles/Platform_Linux/Debug/Library/Foundation/; make clobber");
	    system ("cd ../IntermediateFiles/Platform_Linux/Debug/Library/Frameworks/; make clobber");
		$useBld = "all";
	}
    system ("cd ../IntermediateFiles/Platform_Linux/Debug/Library/Foundation/; make $useBld");
    system ("cd ../IntermediateFiles/Platform_Linux/Debug/Library/Frameworks/; make $useBld");
}
else {
    system ("cd Projects/VisualStudio.Net-2010; perl buildall.pl $useBld");
}
