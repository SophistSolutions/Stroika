#!/usr/bin/perl

require "../ScriptsLib/ConfigurationReader.pl";

print ("Checking Library...\n");
if ("$^O" eq "linux") {
	system ("cd Projects/Linux; perl checkall.pl");
}
else {
	my $useProjectDir= "Projects/" . GetProjectPlatformSubdir ();
	system ("cd $useProjectDir; perl checkall.pl");
}