#!/usr/bin/perl

#print ("OS: $^O\n");

my $doxygen = "";

my $latexMake = "";

if ("$^O" eq "linux") {
	$doxygen = "doxygen";
	$latexMake = "make";
}
if ("$^O" eq "cygwin") {
	$doxygen = "\'c:/Program Files/doxygen/bin/doxygen.exe\'";
	$latexMake = "make.bat";
}

my $doxyArgs = "Stroika-Library.cfg";



#foreach $i (@ARGV) {
#	$runFile = $i;
#}

system ("rm -rf IntermediateFiles");
system ("$doxygen $doxyArgs");
system ("cd IntermediateFiles && cd latex && $latexMake");

#### # INSTALL LATEX ...
#### # USE IT TO GENERATE PDF


