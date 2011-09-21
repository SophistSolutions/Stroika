#!/usr/bin/perl 

require "../../../ScriptsLib/SimpleRun.pl";
require "../../../ScriptsLib/TestsList.pl";

sub DoRun {
	my $testNumber = $_[0];
	my $testName = GetTestName ($testNumber);

	local $date = `sh -c date`;
	chomp ($date);
	print "Starting regression test #[$testNumber] $testName Test: $date ------------------------------\r\n";
	
	DoRunSimpleTestArgv ($ARGV[0], "[$testNumber] $testName", "../../../../Builds/Windows/", "Test$testNumber/Test$testNumber.exe");
	
	local $date = `sh -c date`;
	chomp ($date);
	print "End of Regression Test #[$testNumber] $testName Test: $date -------------------------------\r\n";
}