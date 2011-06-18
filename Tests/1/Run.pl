#!/usr/bin/perl 
require "../ScriptsLib/SimpleRun.pl";

local $date = `sh -c date`;
chomp ($date);
print "Starting regression test #[1] Cryptography Test: $date ------------------------------\r\n";

DoRunSimpleTestArgv ($ARGV[0], "[1] Cryptography Test");

local $date = `sh -c date`;
chomp ($date);
print "End of Regression Test #[1] Cryptography Test: $date -------------------------------\r\n";
