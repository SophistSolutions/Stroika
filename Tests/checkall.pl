#!/usr/bin/perl

require ("ScriptsLib/TestsList.pl");

print ("Checking Tests...\n");
my $checkallDir;
if ("$^O" eq "linux") {
    $checkallDir  = "Projects/Linux/";
}
else {
    $checkallDir  = "Projects/VisualStudio.Net-2010/";
}

use Cwd;
use Cwd 'abs_path';
my $savedDir = abs_path (getcwd ());
chdir ($checkallDir);
	foreach $tst (GetAllTests ()) {
		my $tstName = GetTestName ($tst);
		print ("Test $tst: $tstName:  ");
		system ("cd $tst; perl checkall.pl");
	}
chdir ($savedDir);
