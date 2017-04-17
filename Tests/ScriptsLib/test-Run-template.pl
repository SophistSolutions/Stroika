#!/usr/bin/perl 

BEGIN{ @INC = ( "./", @INC ); }

require "../../../ScriptsLib/SimpleRun.pl";
require "../../../ScriptsLib/TestsList.pl";


require "../../../ScriptsLib/ConfigurationReader.pl";

my $activeConfig = $ENV{'CONFIGURATION'};

sub DoRun {
	my $testNumber = $_[0];
	my $testName = GetTestName ($testNumber);
	my $progress = $_[1];

	local $date = `sh -c date`;
	chomp ($date);
	if ($progress) {
		print "Starting regression test #[$testNumber] $testName Test: $date ------------------------------\n";
	}
	
	if (index($projectPlatformSubdir, "VisualStudio") == -1) {
		DoRunSimpleTestArgv ($activeConfig, "[$testNumber] $testName", "../../../../Builds/$activeConfig/", "Test$testNumber");
	}
	else {
		DoRunSimpleTestArgv ($activeConfig, "[$testNumber] $testName", "../../../../Builds/", "Test$testNumber/Test$testNumber.exe");
	}
	
	if ($progress) {
		local $date = `sh -c date`;
		chomp ($date);
		print "End of Regression Test #[$testNumber] $testName Test: $date -------------------------------\n";
	}
}
