#!/usr/bin/perl 
require "../ScriptsLib/SimpleRun.pl";

local $date = `sh -c date`;
chomp ($date);
print "Starting regression test #[2] Streams/JSON Test: $date ------------------------------\r\n";

DoRunSimpleTestArgv ($ARGV[0], "[2] Streams/JSON Test");

local $date = `sh -c date`;
chomp ($date);
print "End of Regression Test #[2] Streams/JSON Test: $date -------------------------------\r\n";
