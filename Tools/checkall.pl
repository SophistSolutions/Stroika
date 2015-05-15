#!/usr/bin/perl

print ("   Checking Stroika Tools...\n");
require "../ScriptsLib/ConfigurationReader.pl";

if ("$^O" eq "linux") {
	system ("cd Projects/Linux; perl checkall.pl");
}
else {
	my $useProjectDir= "Projects/" . GetProjectPlatformSubdir ();
	system ("cd $useProjectDir; perl checkall.pl");
}