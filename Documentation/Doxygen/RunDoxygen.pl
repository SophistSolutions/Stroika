#!/usr/bin/perl

#print ("OS: $^O\n");

my $doxygen = "";
if ("$^O" eq "linux") {
	$doxygen = "doxygen";
}
if ("$^O" eq "cygwin") {
	$doxygen = "\'c:/Program Files/doxygen/bin/doxygen.exe\'";
}

my $doxyArgs = "Stroika-Library.cfg";



#foreach $i (@ARGV) {
#	$runFile = $i;
#}

system ("$doxygen $doxyArgs");

#### # INSTALL LATEX ...
#### # USE IT TO GENERATE PDF


