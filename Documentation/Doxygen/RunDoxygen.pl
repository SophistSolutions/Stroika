#!/usr/bin/perl

#print ("OS: $^O\n");

my $doxygen = "";

#my $latexMake = "";

if ("$^O" eq "linux") {
	$doxygen = "doxygen";
#	$latexMake = "make";
}
if ("$^O" eq "cygwin") {
	my $doxygenFile = "c:/Program Files/doxygen/bin/doxygen.exe";
	$doxygen = "\'$doxygenFile\'";
	unless (-e $doxygenFile) {
		print "Error: $doxygen Doesn't Exist! - Run Installer (from ThirdPartyProducts)\r\n";
		exit(1);
	}
 #	$latexMake = "make.bat";
}

my $doxyArgs = "Stroika-Library.cfg";

$doxyArgs = $doxyArgs . ">doxygen.stdout 2>doxygen.stderr";

#foreach $i (@ARGV) {
#	$runFile = $i;
#}

system ("rm -rf Output");
system ("rm -rf IntermediateFiles");
system ("$doxygen $doxyArgs");
#system ("cd IntermediateFiles; cd latex; cmd /C $latexMake");
system ("cd IntermediateFiles; mv html Stroika-Doxygen-Help; tar cf Stroika-Doxygen-Help.tar Stroika-Doxygen-Help; gzip --best Stroika-Doxygen-Help.tar; mv Stroika-Doxygen-Help.tar.gz ..");

#### # INSTALL LATEX ...
#### # USE IT TO GENERATE PDF


