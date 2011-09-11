#!/usr/bin/perl 


my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

require ("ScriptsLib/TestsList.pl");

print ("Building Tests...\n");

my $useBld = lc ($BLD_TRG);


if ("$^O" eq "linux") {
	if ($useBld eq "build") {
		$useBld = "all";
	}
	if ($useBld eq "rebuild") {
		system ("cd ../IntermediateFiles/Platform_Linux/Debug/Test1; make clobber");
		system ("cd ../IntermediateFiles/Platform_Linux/Debug/Test2; make clobber");
		system ("cd ../IntermediateFiles/Platform_Linux/Debug/Test3; make clobber");
		system ("cd ../IntermediateFiles/Platform_Linux/Debug/Test4; make clobber");
		system ("cd ../IntermediateFiles/Platform_Linux/Debug/Test5; make clobber");
		system ("cd ../IntermediateFiles/Platform_Linux/Debug/Test6; make clobber");
		$useBld = "all";
	}
	system ("cd ../IntermediateFiles/Platform_Linux/Debug/Test1; make $useBld");
	system ("cd ../IntermediateFiles/Platform_Linux/Debug/Test2; make $useBld");
	system ("cd ../IntermediateFiles/Platform_Linux/Debug/Test3; make $useBld");
	system ("cd ../IntermediateFiles/Platform_Linux/Debug/Test4; make $useBld");
	system ("cd ../IntermediateFiles/Platform_Linux/Debug/Test5; make $useBld");
	system ("cd ../IntermediateFiles/Platform_Linux/Debug/Test6; make $useBld");
}
else {
	use Cwd;
	use Cwd 'abs_path';
	my $savedDir = abs_path (getcwd ());
	chdir ("Projects/VisualStudio.Net-2010");
		foreach $tst (GetAllTests ()) {
			my $tstName = GetTestName ($tst);
			print ("Test $tst: $tstName; $BLD_TRG...\n");
			system ("cd $tst; perl buildall.pl $BLD_TRG");
		}
	chdir ($savedDir);
}
