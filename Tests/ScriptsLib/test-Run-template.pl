#!/usr/bin/perl 

require "../../../ScriptsLib/SimpleRun.pl";
require "../../../ScriptsLib/TestsList.pl";

sub DoRun {
	my $testNumber = $_[0];
	my $testName = GetTestName ($testNumber);
	my $progress = $_[1];

	local $date = `sh -c date`;
	chomp ($date);
	if ($progress) {
		print "Starting regression test #[$testNumber] $testName Test: $date ------------------------------\r\n";
	}
	
	if ("$^O" eq "linux") {
		DoRunSimpleTestArgv ($ARGV[0], "[$testNumber] $testName", "../../../../Builds/DefaultConfiguration/", "Test$testNumber");
	}
	else {
		DoRunSimpleTestArgv ($ARGV[0], "[$testNumber] $testName", "../../../../Builds/", "Test$testNumber/Test$testNumber.exe");
	}
	
	if ($progress) {
		local $date = `sh -c date`;
		chomp ($date);
		print "End of Regression Test #[$testNumber] $testName Test: $date -------------------------------\r\n";
	}
}
