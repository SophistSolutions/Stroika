#!/usr/bin/perl 

### FROM WINDOWS - MOVE TOWARDS THIS - SO PRINTS NAMES OF TESTS
####require "../../../ScriptsLib/test-Run-template.pl";
###
####DoRun (NNN);

require "../../../ScriptsLib/SimpleRun.pl";

local $date = `sh -c date`;
chomp ($date);
print "Starting regression test #[NNN]: $date ------------------------------\r\n";

local $testName = "TestNNN";

DoRunSimpleTestArgv ($ARGV[0], "[NNN]", "../../../../Builds/Platform_Linux/", "$testName");

local $date = `sh -c date`;
chomp ($date);
print "End of Regression Test #[NNN]: $date -------------------------------\r\n";
