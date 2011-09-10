#!/usr/bin/perl 
require "../../../ScriptsLib/SimpleRun.pl";

local $testNum = "6";
local $testName = "DataExchangeFormat/XML";

local $date = `sh -c date`;
chomp ($date);
print "Starting regression test #[$testNum] $testName Test: $date ------------------------------\r\n";

DoRunSimpleTestArgv ($ARGV[0], "[$testNum] $testName Test", "../../../../Builds/Windows/", "Test6/Test6.exe");

local $date = `sh -c date`;
chomp ($date);
print "End of Regression Test #[$testNum] $testName Test: $date -------------------------------\r\n";
