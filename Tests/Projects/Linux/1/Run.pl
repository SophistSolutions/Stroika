#!/usr/bin/perl 
require "../../../ScriptsLib/SimpleRun.pl";

local $date = `sh -c date`;
chomp ($date);
print "Starting regression test #[1] Foundation::Memory Test: $date ------------------------------\r\n";

DoRunSimpleTestArgv ($ARGV[0], "[1] Foundation::Memory Test", "../../../../Builds/Platform_Linux/", "Test1");

local $date = `sh -c date`;
chomp ($date);
print "End of Regression Test #[1] Foundation::Memory Test: $date -------------------------------\r\n";
