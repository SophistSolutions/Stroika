#!/usr/bin/perl 

require "../ScriptsLib/ConfigurationReader.pl";
require ("ScriptsLib/TestsList.pl");

$SIG{INT}  = \&signal_handler;
$SIG{TERM} = \&signal_handler;

sub signal_handler {
    die "Caught a signal $!";
}

# this causes confusion with make -j
delete $ENV{'MAKEFLAGS'};


my $activeConfig = $ENV{'CONFIGURATION'};
my $projectPlatformSubdir = GetProjectPlatformSubdir($activeConfig);

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '' || $BLD_TRG eq 'build') {
	$BLD_TRG = 'Build';
}


my $level = $ENV{'MAKE_INDENT_LEVEL'};
$level = $level;
print(`../ScriptsLib/PrintLevelLeader.sh $level` . "Building Stroika {$activeConfig} Tests:\n");


my $subLevel = $level + 1;
my $subLevelLeader = `../ScriptsLib/PrintLevelLeader.sh $subLevel`;

my $useBld = lc ($BLD_TRG);

if (index($projectPlatformSubdir, "VisualStudio") == -1) {
	if ($useBld eq "build") {
		$useBld = "all";
	}
	use Cwd;
	use Cwd 'abs_path';
	my $savedDir = abs_path (getcwd ());

	chdir ("Projects/Unix");
		system ("perl BuildProjectsFiles.pl");
	chdir ($savedDir);

	chdir ("../IntermediateFiles/$activeConfig/");
	foreach $tst (GetAllTests ()) {
		my $tstName = GetTestName ($tst);
		print ("   Test $tst: $tstName; $useBld...\n");
		$exit_status = system ("cd Test$tst; make -s $useBld CONFIGURATION=$activeConfig\n");
		if ($exit_status != 0) {
			die("Stopping build: failed");
		}
	}
	chdir ($savedDir);
}
else {
	use Cwd;
	use Cwd 'abs_path';
	my $savedDir = abs_path (getcwd ());
    	my $useProjectDir= "Projects/" . $projectPlatformSubdir;
	chdir ($useProjectDir);
		system ("perl BuildProjectsFiles.pl");
		foreach $tst (GetAllTests ()) {
			my $tstName = GetTestName ($tst);
			print ("$subLevelLeader$BLD_TRG Test $tst: $tstName ...\n");
			$exit_status = system ("cd $tst; perl buildall.pl $BLD_TRG");
			if ($exit_status != 0) {
				die("Stopping build: failed");
			}
		}
	chdir ($savedDir);
}
