#!/usr/bin/perl 

require "../../../ScriptsLib/test-Run-template.pl";
DoRun ("NNN");

exit(0);


require "../../../ScriptsLib/SimpleRun.pl";

local $date = `sh -c date`;
chomp ($date);
print "Starting regression test #[NNN]: $date ------------------------------\r\n";

local $testName = "TestNNN";

DoRunSimpleTestArgv ($ARGV[0], "[NNN]", "../../../../Builds/DefaultConfiguration/", "$testName");

local $date = `sh -c date`;
chomp ($date);
print "End of Regression Test #[NNN]: $date -------------------------------\r\n";
