#!/usr/bin/perl -w

# MUST FIX TO CHECK FOR --only-if-unconfigured - for now thats all the logic we'll support
# but should check flag...
#TRY THESE AGAIN, but for now generate too many problems...
#use strict;
#use warnings;



use constant false => 0;
use constant true  => 1;



#
# For now KISS - just check if the file doesn't exist, and if so write a default value.
#
my $configFileName	=	"Library/Sources/Stroika/Foundation/Configuration/StroikaConfig.h";


my $intermediateFiles	=	"IntermediateFiles/";
my $platform		=	"Platform_Linux";
my $target		=	"Debug";

my $forceRecreate = true;

my $useThirdPartyXerces	=	true;



sub mkDirWithLinks
{
	local $relPath = $_[0];
	local $makefileName = $_[1];

	mkdir "$intermediateFiles/$platform/$target/Library/$relPath";
	system ("ln -s ../../../../../Library/Projects/Linux/$makefileName $intermediateFiles$platform/$target/Library/$relPath/Makefile");
}
sub mkDirWithLinks2
{
	local $relPath = $_[0];
	local $makefileName = $_[1];

	mkdir "$intermediateFiles/$platform/$target/Library/$relPath";
	system ("ln -s ../../../../../../Library/Projects/Linux/$makefileName $intermediateFiles$platform/$target/Library/$relPath/Makefile");
}


sub MakeUnixDirs {
	if ($forceRecreate) {
		system ("rm -rf $intermediateFiles");
	}
	unless (-e $intermediateFiles) {
		mkdir "$intermediateFiles";
		mkdir "$intermediateFiles/$platform";
		mkdir "$intermediateFiles/$platform/$target";
		mkdir "$intermediateFiles/$platform/$target/Library";
		system ("ln -s ../../../../Library/Projects/Linux/Makefile-Foundation $intermediateFiles/$platform/$target/Library/Makefile");
		system ("cp Library/Projects/Linux/Configuration-Default.mk $intermediateFiles/$platform/$target/Library/Configuration.mk");
		system ("cp Library/Projects/Linux/SharedBuildRules-Default.mk $intermediateFiles/$platform/$target/Library/SharedBuildRules.mk");
		system ("cp Library/Projects/Linux/SharedMakeVariables-Default.mk $intermediateFiles/$platform/$target/Library/SharedMakeVariables.mk");

		mkDirWithLinks("Characters", "Makefile-Foundation-Characters");
		mkDirWithLinks("Configuration", "Makefile-Foundation-Configuration");
		mkDirWithLinks("Containers", "Makefile-Foundation-Containers");
		mkDirWithLinks("Cryptography", "Makefile-Foundation-Cryptography");
		mkDirWithLinks("Database", "Makefile-Foundation-Database");
		mkDirWithLinks("DataExchangeFormat", "Makefile-Foundation-DataExchangeFormat");
		mkDirWithLinks2("DataExchangeFormat/JSON", "Makefile-Foundation-DataExchangeFormat-JSON");
		mkDirWithLinks2("DataExchangeFormat/XML", "Makefile-Foundation-DataExchangeFormat-XML");
		mkDirWithLinks("Debug", "Makefile-Foundation-Debug");
		mkDirWithLinks("Execution", "Makefile-Foundation-Execution");
		mkDirWithLinks("IO", "Makefile-Foundation-IO");
		mkDirWithLinks2("IO/Network", "Makefile-Foundation-IO-Network");
		mkDirWithLinks("Linguistics", "Makefile-Foundation-Linguistics");
		mkDirWithLinks("Math", "Makefile-Foundation-Math");
		mkDirWithLinks("Memory", "Makefile-Foundation-Memory");
		mkDirWithLinks("Streams", "Makefile-Foundation-Streams");
		mkDirWithLinks("Time", "Makefile-Foundation-Time");

		mkdir "$intermediateFiles/$platform/$target/Test1";
		system ("ln -s ../../../../Tests/Projects/Linux/Makefile-Test1 $intermediateFiles/$platform/$target/Test1/Makefile");
		mkdir "$intermediateFiles/$platform/$target/Test2";
		system ("ln -s ../../../../Tests/Projects/Linux/Makefile-Test2 $intermediateFiles/$platform/$target/Test2/Makefile");
		mkdir "$intermediateFiles/$platform/$target/Test3";
		system ("ln -s ../../../../Tests/Projects/Linux/Makefile-Test3 $intermediateFiles/$platform/$target/Test3/Makefile");
		mkdir "$intermediateFiles/$platform/$target/Test4";
		system ("ln -s ../../../../Tests/Projects/Linux/Makefile-Test4 $intermediateFiles/$platform/$target/Test4/Makefile");
		mkdir "$intermediateFiles/$platform/$target/Test5";
		system ("ln -s ../../../../Tests/Projects/Linux/Makefile-Test5 $intermediateFiles/$platform/$target/Test5/Makefile");
		mkdir "$intermediateFiles/$platform/$target/Test6";
		system ("ln -s ../../../../Tests/Projects/Linux/Makefile-Test6 $intermediateFiles/$platform/$target/Test6/Makefile");
		mkdir "$intermediateFiles/$platform/$target/Test7";
		system ("ln -s ../../../../Tests/Projects/Linux/Makefile-Test7 $intermediateFiles/$platform/$target/Test7/Makefile");
		mkdir "$intermediateFiles/$platform/$target/Test8";
		system ("ln -s ../../../../Tests/Projects/Linux/Makefile-Test8 $intermediateFiles/$platform/$target/Test8/Makefile");
		mkdir "$intermediateFiles/$platform/$target/Test9";
		system ("ln -s ../../../../Tests/Projects/Linux/Makefile-Test9 $intermediateFiles/$platform/$target/Test9/Makefile");
		mkdir "$intermediateFiles/$platform/$target/Test10";
		system ("ln -s ../../../../Tests/Projects/Linux/Makefile-Test10 $intermediateFiles/$platform/$target/Test10/Makefile");
		mkdir "$intermediateFiles/$platform/$target/Test11";
		system ("ln -s ../../../../Tests/Projects/Linux/Makefile-Test11 $intermediateFiles/$platform/$target/Test11/Makefile");
	}
}





sub WriteDefault
{
	#open(OUT,">$configFileName");
	open(OUT,">:crlf", "$configFileName");
	print (OUT "/*\n");
	print (OUT " * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved\n");
	print (OUT " */\n");
	print (OUT "#ifndef	_Stroika_Foundation_Configuration_StroikaConfig_h_\n");
	print (OUT "#define	_Stroika_Foundation_Configuration_StroikaConfig_h_	1\n");
	print (OUT "\n");
	print (OUT "//WARNING: THIS FILE WAS AUTOGENERATED WITH configure.pl\n");
	print (OUT "//WARNING: THIS FILE WAS AUTOGENERATED WITH configure.pl\n");
	print (OUT "//WARNING: THIS FILE WAS AUTOGENERATED WITH configure.pl\n");
	print (OUT "//WARNING: THIS FILE WAS AUTOGENERATED WITH configure.pl\n");
	print (OUT "//WARNING: THIS FILE WAS AUTOGENERATED WITH configure.pl\n");
	print (OUT "//WARNING: THIS FILE WAS AUTOGENERATED WITH configure.pl\n");
	print (OUT "//WARNING: THIS FILE WAS AUTOGENERATED WITH configure.pl\n");
	print (OUT "\n");
	print (OUT "\n");

	print (OUT "//Defaults overrides:\n");
	if ($useThirdPartyXerces) {
		print (OUT "#define	qHasLibrary_Xerces	1\n");
	}
	else {
		print (OUT "#define	qHasLibrary_Xerces	0\n");
	}


	print (OUT "\n");
	print (OUT "\n");
	print (OUT "#include	\"Private/Defaults_Configuration_Common_.h\"\n");
	print (OUT "\n");

	print (OUT "//Out of alphabetic order because other defaults depend on qDebug\n");
	print (OUT "#include	\"Private/Defaults_Debug_Assertions_.h\"\n");
	print (OUT "\n");
	print (OUT "#include	\"Private/Defaults_Characters_Character_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Characters_StringUtils_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Characters_TChar_.h\"\n");
	print (OUT "#include	\"Private/Defaults_CompilerAndStdLib_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Containers_LRUCache_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Containers_TimedCache_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Database_ODBCClient_.h\"\n");
	print (OUT "#include	\"Private/Defaults_DataExchangeFormat_XML_Common_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Debug_Trace_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Execution_Threads_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Memory_Common_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Memory_BlockAllocated_.h\"\n");
	print (OUT "\n");

	print (OUT "#endif	/*_Stroika_Foundation_Configuration_StroikaConfig_h_*/\n");
	close(OUT);
}


foreach $var (@ARGV)
{
	if (lc ($var) eq "-force" or lc ($var) eq "--force") {
		$forceRecreate = true;
		print "Forcing recreate...\n";
	}
}




if ($forceRecreate) {
    system ("rm $configFileName");
}

unless (-e $configFileName) {
	print("Writing \"$configFileName\"...\n");
	WriteDefault ();
}

#print ("OS: $^O\n");

if ("$^O" eq "linux") {
    MakeUnixDirs ();
}
