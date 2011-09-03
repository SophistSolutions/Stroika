#!/usr/bin/perl 
require "../../../ScriptsLib/SimpleRun.pl";

local $testNum = "4";
local $testName = "Exceptions";

local $date = `sh -c date`;
chomp ($date);
print "Starting regression test #[$testNum] Execution/$testName Test: $date ------------------------------\r\n";

DoRunSimpleTestArgv ($ARGV[0], "[$testNum] Execution/$testName Test", "../../../../Builds/Windows/Test4/");

local $date = `sh -c date`;
chomp ($date);
print "End of Regression Test #[$testNum] Execution/$testName Test: $date -------------------------------\r\n";
