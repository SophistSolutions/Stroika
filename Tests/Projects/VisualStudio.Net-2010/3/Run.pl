#!/usr/bin/perl 
require "../../../ScriptsLib/SimpleRun.pl";

local $date = `sh -c date`;
chomp ($date);
print "Starting regression test #[3] Execution/Threads Test: $date ------------------------------\r\n";

DoRunSimpleTestArgv ($ARGV[0], "[3] Execution/Threads Test", "../../../../Builds/Windows/", "Test3/Test3.exe");

local $date = `sh -c date`;
chomp ($date);
print "End of Regression Test #[3] Execution/Threads Test: $date -------------------------------\r\n";
