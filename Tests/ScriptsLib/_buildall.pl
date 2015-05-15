#!/usr/bin/perl 

require "../ScriptsLib/ConfigurationReader.pl";
require ("ScriptsLib/TestsList.pl");

$SIG{INT}  = \&signal_handler;
$SIG{TERM} = \&signal_handler;

sub signal_handler {
    die "Caught a signal $!";
}

my $activeConfig = GetActiveConfigurationName ();

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '' || $BLD_TRG eq 'build') {
	$BLD_TRG = 'Build';
}


my $level = $ENV{'MAKELEVEL'} - 1;
$level=0;
print(`../ScriptsLib/PrintLevelLeader.sh $level` . "Building Tests...\n");

my $useBld = lc ($BLD_TRG);


if ("$^O" eq "linux") {
	if ($useBld eq "build") {
		$useBld = "all";
	}
	use Cwd;
	use Cwd 'abs_path';
	my $savedDir = abs_path (getcwd ());

	chdir ("Projects/Linux");
		system ("perl BuildProjectsFiles.pl");
	chdir ($savedDir);

	chdir ("../IntermediateFiles/$activeConfig/");
	if ($useBld eq "rebuild") {
		foreach $tst (GetAllTests ()) {
			my $tstName = GetTestName ($tst);
			print ("   Test $tst: $tstName; clobber...\n");
			if ($exit_status != 0) {
				die("Stopping build: failed");
			}
		}
		$useBld = "all";
	}
	foreach $tst (GetAllTests ()) {
		my $tstName = GetTestName ($tst);
		print ("   Test $tst: $tstName; $useBld...\n");
		$exit_status = system ("cd Test$tst; make -s $useBld\n");
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
    	my $useProjectDir= "Projects/" . GetProjectPlatformSubdir ();
	chdir ($useProjectDir);
		system ("perl BuildProjectsFiles.pl");
		foreach $tst (GetAllTests ()) {
			my $tstName = GetTestName ($tst);
			print ("   $BLD_TRG Test $tst: $tstName ...\n");
			$exit_status = system ("cd $tst; perl buildall.pl $BLD_TRG");
			if ($exit_status != 0) {
				die("Stopping build: failed");
			}
		}
	chdir ($savedDir);
}
print(`../ScriptsLib/PrintLevelLeader.sh $level` . "Building Tests...done\n");
