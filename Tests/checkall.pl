#!/usr/bin/perl

require ("ScriptsLib/TestsList.pl");

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
		system ("cd $tst; perl checkall.pl");
	}
chdir ($savedDir);
