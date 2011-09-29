#!/usr/bin/perl

# MUST FIX TO CHECK FOR --only-if-unconfigured - for now thats all the logic we'll support
# but should check flag...
#TRY THESE AGAIN, but for now generate too many problems...
#use strict;
#use warnings;



use constant false => 0;
use constant true  => 1;





my $intermediateFiles	=	"IntermediateFiles/";
my $platform			=	"Platform_Linux";
my $target			=	"Debug";


#
# For now KISS - just check if the file doesn't exist, and if so write a default value.
#
my $configFileCName		=	"Library/Sources/Stroika/Foundation/Configuration/StroikaConfig.h";
my $configFileMakeName	=	"$intermediateFiles$platform/$target/Library/Configuration.mk";


my $forceRecreate = true;

my $useThirdPartyXerces	=	true;

my @useExtraCDefines;
my @useExtraMakeDefines;

## FOR NOW ONLY USED ON LINUX BUILDS
my $ENABLE_ASSERTIONS = 1;
my $INCLUDE_SYMBOLS = 1;
my $COPTIMIZE_FLAGS = "";




sub mkDirWithLinks
{
	local $relPath = $_[0];
	local $makefileName = $_[1];

	mkdir "$intermediateFiles/$platform/$target/Library/$relPath";
	system ("ln -s ../../../../../../Library/Projects/Linux/$makefileName $intermediateFiles$platform/$target/Library/$relPath/Makefile");
}
sub mkDirWithLinks2
{
	local $relPath = $_[0];
	local $makefileName = $_[1];

	mkdir "$intermediateFiles/$platform/$target/Library/$relPath";
	system ("ln -s ../../../../../../../Library/Projects/Linux/$makefileName $intermediateFiles$platform/$target/Library/$relPath/Makefile");
}
sub mkDirWithLinks3
{
	local $relPath = $_[0];
	local $makefileName = $_[1];

	mkdir "$intermediateFiles/$platform/$target/Library/$relPath";
	system ("ln -s ../../../../../../../../Library/Projects/Linux/$makefileName $intermediateFiles$platform/$target/Library/$relPath/Makefile");
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
		mkdir "$intermediateFiles/$platform/$target/Library/Foundation";
		mkdir "$intermediateFiles/$platform/$target/Library/Frameworks";

		system ("ln -s ../../../../../Library/Projects/Linux/Makefile-Foundation $intermediateFiles/$platform/$target/Library/Foundation/Makefile");
		#system ("cp Library/Projects/Linux/Configuration-Default.mk $intermediateFiles/$platform/$target/Library/Configuration.mk");
		system ("cp Library/Projects/Linux/SharedBuildRules-Default.mk $intermediateFiles/$platform/$target/Library/SharedBuildRules.mk");
		system ("cp Library/Projects/Linux/SharedMakeVariables-Default.mk $intermediateFiles/$platform/$target/Library/SharedMakeVariables.mk");

		mkDirWithLinks("Foundation/Characters", "Makefile-Foundation-Characters");
		mkDirWithLinks("Foundation/Configuration", "Makefile-Foundation-Configuration");
		mkDirWithLinks("Foundation/Containers", "Makefile-Foundation-Containers");
		mkDirWithLinks("Foundation/Cryptography", "Makefile-Foundation-Cryptography");
		mkDirWithLinks("Foundation/Database", "Makefile-Foundation-Database");
		mkDirWithLinks("Foundation/DataExchangeFormat", "Makefile-Foundation-DataExchangeFormat");
		mkDirWithLinks2("Foundation/DataExchangeFormat/JSON", "Makefile-Foundation-DataExchangeFormat-JSON");
		mkDirWithLinks2("Foundation/DataExchangeFormat/XML", "Makefile-Foundation-DataExchangeFormat-XML");
		mkDirWithLinks("Foundation/Debug", "Makefile-Foundation-Debug");
		mkDirWithLinks("Foundation/Execution", "Makefile-Foundation-Execution");
		mkDirWithLinks("Foundation/IO", "Makefile-Foundation-IO");
		mkDirWithLinks2("Foundation/IO/FileSystem", "Makefile-Foundation-IO-FileSystem");
		mkDirWithLinks2("Foundation/IO/Network", "Makefile-Foundation-IO-Network");
		mkDirWithLinks3("Foundation/IO/Network/Transfer", "Makefile-Foundation-IO-Network-Transfer");
		mkDirWithLinks("Foundation/Linguistics", "Makefile-Foundation-Linguistics");
		mkDirWithLinks("Foundation/Math", "Makefile-Foundation-Math");
		mkDirWithLinks("Foundation/Memory", "Makefile-Foundation-Memory");
		mkDirWithLinks("Foundation/Streams", "Makefile-Foundation-Streams");
		mkDirWithLinks2("Foundation/Streams/iostream", "Makefile-Foundation-Streams-iostream");
		mkDirWithLinks("Foundation/Time", "Makefile-Foundation-Time");

		system ("ln -s ../../../../../Library/Projects/Linux/Makefile-Frameworks $intermediateFiles/$platform/$target/Library/Frameworks/Makefile");

		mkDirWithLinks("Frameworks/Service", "Makefile-Frameworks-Service");


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





sub WriteStroikaConfigCHeader
{
	#open(OUT,">$configFileCName");
	open(OUT,">:crlf", "$configFileCName");
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
	print (OUT "//Configure Command Line Arguments (-c-define)\n");
	foreach $var (@useExtraCDefines)
	{
		print (OUT "$var\n");
	}
	print (OUT "\n");
	print (OUT "\n");

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
	print (OUT "#include	\"Private/Defaults_Execution_Module_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Execution_Threads_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Memory_Common_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Memory_BlockAllocated_.h\"\n");
	print (OUT "\n");

	print (OUT "#endif	/*_Stroika_Foundation_Configuration_StroikaConfig_h_*/\n");
	close(OUT);
}





sub WriteStroikaConfigMakeHeader
{
	open(OUT,">$configFileMakeName");
	print (OUT "#\n");
	print (OUT "#Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved\n");
	print (OUT "#\n");
	print (OUT "\n");
	print (OUT "\n");
	print (OUT "#WARNING: THIS FILE WAS AUTOGENERATED WITH configure.pl\n");
	print (OUT "#WARNING: THIS FILE WAS AUTOGENERATED WITH configure.pl\n");
	print (OUT "#WARNING: THIS FILE WAS AUTOGENERATED WITH configure.pl\n");
	print (OUT "#WARNING: THIS FILE WAS AUTOGENERATED WITH configure.pl\n");
	print (OUT "#WARNING: THIS FILE WAS AUTOGENERATED WITH configure.pl\n");
	print (OUT "#WARNING: THIS FILE WAS AUTOGENERATED WITH configure.pl\n");
	print (OUT "#WARNING: THIS FILE WAS AUTOGENERATED WITH configure.pl\n");
	print (OUT "#WARNING: THIS FILE WAS AUTOGENERATED WITH configure.pl\n");
	print (OUT "\n");
	print (OUT "\n");

	print (OUT "#Defaults overrides:\n");

	print (OUT "\n");
	print (OUT "#Configure Command Line Arguments (-make-define)\n");
	foreach $var (@useExtraMakeDefines)
	{
		print (OUT "$var\n");
	}
	
	
	print (OUT "\n");
	print (OUT "ENABLE_ASSERTIONS=	$ENABLE_ASSERTIONS\n");
	print (OUT "INCLUDE_SYMBOLS=		$INCLUDE_SYMBOLS\n");
	print (OUT "\n");
	print (OUT "\n");
	print (OUT "# Can be blank, or -O2, etc..\n");
	print (OUT "COPTIMIZE_FLAGS=	$COPTIMIZE_FLAGS\n");
	print (OUT "\n");
	print (OUT "\n");
	print (OUT "#C++-Compiler\n");
	print (OUT "CPlusPlus=	g++\n");
	print (OUT "#CPlusPlus=	gcc\n");
	print (OUT "#CPlusPlus=	g++-4.6\n");
	print (OUT "#CPlusPlus=	g++ -V4.5\n");
	print (OUT "#CPlusPlus=	g++ -V4.6\n");
	print (OUT "\n");
	print (OUT "\n");
	print (OUT "#Linker-Driver\n");
	print (OUT "Linker=	\$(CPlusPlus)\n");
	print (OUT "\n");

	close(OUT);
}

for ($i = 0; $i <= $#ARGV; $i++) {
	my $var = $ARGV[$i];
	if (lc ($var) eq "-force" or lc ($var) eq "--force") {
		$forceRecreate = true;
		print "Forcing recreate...\n";
	}
	if (lc ($var) eq "-c-define") {
		$i++;
		$var = $ARGV[$i];
		$useExtraCDefines[@useExtraCDefines] = $var;
	}
	if (lc ($var) eq "-make-define") {
		$i++;
		$var = $ARGV[$i];
		$useExtraMakeDefines[@useExtraMakeDefines] = $var;
	}
}

#print ("OS: $^O\n");



if ($forceRecreate) {
    system ("rm -f $configFileCName $configFileMakeName");
}

if ("$^O" eq "linux") {
    MakeUnixDirs ();
}


unless (-e $configFileCName) {
	print("Writing \"$configFileCName\"...\n");
	WriteStroikaConfigCHeader ();
}


unless (-e $configFileMakeName) {
	print("Writing \"$configFileMakeName\"...\n");
	WriteStroikaConfigMakeHeader ();
}
