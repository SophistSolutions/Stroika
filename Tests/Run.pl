#!/usr/bin/perl 

require "../ScriptsLib/ConfigurationReader.pl";
require ("ScriptsLib/TestsList.pl");

print ("Running Tests...\n");

my $runAllDir	= "Projects/" . GetProjectPlatformSubdir ();

use Cwd;
use Cwd 'abs_path';
my $savedDir = abs_path (getcwd ());
chdir ($runAllDir);
	foreach $tst (GetAllTests ()) {
		system ("cd $tst; perl Run.pl");
	}
chdir ($savedDir);
