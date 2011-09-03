#!/usr/bin/perl 
require "../../../ScriptsLib/SimpleRun.pl";

local $testNum = "5";
local $testName = "Strings";

local $date = `sh -c date`;
chomp ($date);
print "Starting regression test #[$testNum] Characters/$testName Test: $date ------------------------------\r\n";

DoRunSimpleTestArgv ($ARGV[0], "[$testNum] Characters/$testName Test", "../../../../Builds/Windows/Test5/");

local $date = `sh -c date`;
chomp ($date);
print "End of Regression Test #[$testNum] Characters/$testName Test: $date -------------------------------\r\n";
