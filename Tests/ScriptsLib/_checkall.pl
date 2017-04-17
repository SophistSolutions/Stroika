#!/usr/bin/perl

BEGIN{ @INC = ( "./", @INC ); }

require "../ScriptsLib/ConfigurationReader.pl";
require ("ScriptsLib/TestsList.pl");

my $activeConfig = $ENV{'CONFIGURATION'};
my $level = $ENV{'MAKE_INDENT_LEVEL'};
$level = $level;
my $subLevel = $level + 1;

my $subLevelStr = `../ScriptsLib/PrintLevelLeader.sh $subLevel`;
print (`../ScriptsLib/PrintLevelLeader.sh $level` . "Checking Stroika {$activeConfig} Tests:\n");
my $checkallDir = "Projects/" . GetProjectPlatformSubdir ($activeConfig);

use Cwd;
use Cwd 'abs_path';
my $savedDir = abs_path (getcwd ());
chdir ($checkallDir);
	foreach $tst (GetAllTests ()) {
		my $tstName = GetTestName ($tst);
		print ($subLevelStr . "Test $tst: $tstName:  ");
		system ("CONFIGURATION=$activeConfig cd $tst; perl checkall.pl");
	}
chdir ($savedDir);
