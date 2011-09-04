#!/usr/bin/perl 

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

print ("Building Tests...\n");
if ("$^O" eq "linux") {
	my $useBld = lc ($BLD_TRG);
	if ($useBld eq "build") {
		$useBld = "all";
	}
    system ("cd ../IntermediateFiles/Platform_Linux/Debug/Test1; make $useBld");
    system ("cd ../IntermediateFiles/Platform_Linux/Debug/Test2; make $useBld");
    system ("cd ../IntermediateFiles/Platform_Linux/Debug/Test3; make $useBld");
    system ("cd ../IntermediateFiles/Platform_Linux/Debug/Test4; make $useBld");
    system ("cd ../IntermediateFiles/Platform_Linux/Debug/Test5; make $useBld");
}
else {
	system ("cd Projects/VisualStudio.Net-2010/1; perl buildall.pl $BLD_TRG");
	system ("cd Projects/VisualStudio.Net-2010/2; perl buildall.pl $BLD_TRG");
	system ("cd Projects/VisualStudio.Net-2010/3; perl buildall.pl $BLD_TRG");
	system ("cd Projects/VisualStudio.Net-2010/4; perl buildall.pl $BLD_TRG");
	system ("cd Projects/VisualStudio.Net-2010/5; perl buildall.pl $BLD_TRG");
}
