#!/usr/bin/perl

# MUST FIX TO CHECK FOR --only-if-unconfigured - for now thats all the logic we'll support
# but should check flag...
#TRY THESE AGAIN, but for now generate too many problems...
#use strict;
#use warnings;

require ("Tests/ScriptsLib/TestsList.pl");


use constant false => 0;
use constant true  => 1;





my $intermediateFiles	=	"IntermediateFiles/";
my $platform			=	"Platform_Linux";
my $target			=	"Debug";


my $forceRecreate = true;

my $useThirdPartyXerces	=	true;

my @useExtraCDefines;
my @useExtraMakeDefines;

## FOR NOW ONLY USED ON LINUX BUILDS
my $ENABLE_ASSERTIONS = -1;				#-1 not specified
my $ENABLE_LIBCURL = 0;
my $ENABLE_WINHTTP = 0;
my $ENABLE_TRACE2FILE = -1;
my $INCLUDE_SYMBOLS = 1;
my $COPTIMIZE_FLAGS = "";
my $STATIC_LINK_GCCRUNTIME = 1;

#quite unclear why this is needed - or even if it is - but it appears to be that
# just regular
my $IF_STATIC_LINK_GCCRUNTIME_USE_PRINTPATH_METHOD = 1;


sub	DoHelp_
{
	print("Usage:\n");
	print("	configure.pl OPTIONS where options can be:\n");
	print("	    --force                      /* forces rebuild of the given configuration */\n");
	print("	    --default-for-platform       /* May create multiple targets (recursive call to configure) - but generates all the default settings for this platform */\n");
	print("	    --only-if-unconfigured       /* Opposite of --force - only rebuilds the configfiles if absent */\n");
	print("	    --platform {PLATFORM}        /* specifies the directory under Builds/Intermediate Files to create */\n");
	print("	    --target {TARGET}            /* specifies the directory under Platform to create (no other semantics - just a name) */\n");
	print("	    --enable-assertions          /* enables assertions for the configuration being configured*/\n");
	print("	    --disable-assertions         /* disables assertions for the configuration being configured*/\n");
	print("	    --default-assertions         /* default assertions (based on NDEBUG flag) for the configuration being configured - so */\n");
	print("	    --has-libcurl                /* enables libcurl for the configuration being configured*/\n");
	print("	    --no-has-libcurl             /* disables libcurl for the configuration being configured*/\n");
	print("	    --enable-trace2file          /* enables trace2file for the configuration being configured*/\n");
	print("	    --disable-trace2file         /* disables trace2file for the configuration being configured*/\n");
	print("	    --cpp-optimize-flag  {FLAG}  /* Sets $COPTIMIZE_FLAGS (empty str means none, -O2 is typical for optimize) - UNIX ONLY*/\n");
	print("	    --c-define {ARG}             /* Define C++ / CPP define for the given target */\n");
	print("	    --make-define {ARG}          /* Define makefile define for the given target */\n");

print(" >>> MUST ADD HAS_FEATURE\n");
	exit (0);
}


sub	SetDefaultForPlatform_
{
	if ("$^O" eq "linux" && -e '/usr/include/curl/curl.h') {
		$ENABLE_LIBCURL = 1;
	}
	if ("$^O" eq "cygwin") {
		$ENABLE_WINHTTP = 1;
	}
}

sub	ParseCommandLine_
{
	SetDefaultForPlatform_ ();
	for ($i = 0; $i <= $#ARGV; $i++) {
		my $var = $ARGV[$i];
		if (lc ($var) eq "-force" or lc ($var) eq "--force") {
			$forceRecreate = true;
			print "Forcing recreate...\n";
		}
		if (lc ($var) eq "-c-define" or lc ($var) eq "--c-define") {
			$i++;
			$var = $ARGV[$i];
			$useExtraCDefines[@useExtraCDefines] = $var;
		}
		if (lc ($var) eq "-make-define" or lc ($var) eq "--make-define") {
			$i++;
			$var = $ARGV[$i];
			$useExtraMakeDefines[@useExtraMakeDefines] = $var;
		}
		if ((lc ($var) eq "-target") or (lc ($var) eq "--target")) {
			$i++;
			$var = $ARGV[$i];
			$target = $var;
		}
		if ((lc ($var) eq "-platform") or (lc ($var) eq "--platform")) {
			$i++;
			$var = $ARGV[$i];
			$platform = $var;
			SetDefaultForPlatform_ ();
		}
		if ((lc ($var) eq "-enable-assertions") or (lc ($var) eq "--enable-assertions")) {
			$ENABLE_ASSERTIONS = 1;
		}
		if ((lc ($var) eq "-disable-assertions") or (lc ($var) eq "--disable-assertions")) {
			$ENABLE_ASSERTIONS = 0;
		}
		if ((lc ($var) eq "-default-assertions") or (lc ($var) eq "--default-assertions")) {
			$ENABLE_ASSERTIONS = -1;
		}
		if ((lc ($var) eq "-has-libcurl") or (lc ($var) eq "--has-libcurl")) {
			$ENABLE_LIBCURL = 1;
		}
		if ((lc ($var) eq "-no-has-libcurl") or (lc ($var) eq "--no-has-libcurl")) {
			$ENABLE_LIBCURL = 0;
		}
		if ((lc ($var) eq "-has-winhttp") or (lc ($var) eq "--has-winhttp")) {
			$ENABLE_WINHTTP = 1;
		}
		if ((lc ($var) eq "-no-has-winhttp") or (lc ($var) eq "--no-has-winhttp")) {
			$ENABLE_WINHTTP = 0;
		}
		if ((lc ($var) eq "-enable-trace2file") or (lc ($var) eq "--enable-trace2file")) {
			$ENABLE_TRACE2FILE = 1;
		}
		if ((lc ($var) eq "-disable-trace2file") or (lc ($var) eq "--disable-trace2file")) {
			$ENABLE_TRACE2FILE = 0;
		}
		if ((lc ($var) eq "-enable-static-link-gccruntime") or (lc ($var) eq "--enable-static-link-gccruntime")) {
			$STATIC_LINK_GCCRUNTIME = 1;
		}
		if ((lc ($var) eq "-disable-static-link-gccruntime") or (lc ($var) eq "--disable-static-link-gccruntime")) {
			$STATIC_LINK_GCCRUNTIME = 0;
		}
		if ((lc ($var) eq "-cpp-optimize-flag") or (lc ($var) eq "--cpp-optimize-flag")) {
			$i++;
			$var = $ARGV[$i];
			$COPTIMIZE_FLAGS = $var;
		}
		if ((lc ($var) eq "-only-if-unconfigured") or (lc ($var) eq "--only-if-unconfigured")) {
			$forceRecreate = false;
		}
		if ((lc ($var) eq "-default-for-platform") or (lc ($var) eq "--default-for-platform")) {
			SetDefaultForPlatform_ ();
		}
		if ((lc ($var) eq "-help") or (lc ($var) eq "--help") or (lc ($var) eq "-?")) {
			DoHelp_ ();
		}
	}
}

ParseCommandLine_ ();




if ($forceRecreate) {
	print "Forcing recreate...\n";
}




#
# For now KISS - just check if the file doesn't exist, and if so write a default value.
#
my $configFileCName		=	"Library/Sources/Stroika/Foundation/Configuration/StroikaConfig.h";
my $configFileMakeName	=	"$intermediateFiles$platform/$target/Library/Configuration.mk";

my $masterXMLConfigFile	=	"$intermediateFiles/Configuration.xml";




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

		mkDirWithLinks("Foundation/Cache", "Makefile-Foundation-Cache");
		mkDirWithLinks("Foundation/Characters", "Makefile-Foundation-Characters");
		mkDirWithLinks2("Foundation/Characters/CString", "Makefile-Foundation-Characters-CString);
		mkDirWithLinks("Foundation/Configuration", "Makefile-Foundation-Configuration");
		mkDirWithLinks("Foundation/Containers", "Makefile-Foundation-Containers");
		mkDirWithLinks("Foundation/Cryptography", "Makefile-Foundation-Cryptography");
		mkDirWithLinks2("Foundation/Cryptography/SSL", "Makefile-Foundation-Cryptography-SSL");
		mkDirWithLinks("Foundation/Database", "Makefile-Foundation-Database");
		mkDirWithLinks("Foundation/DataExchangeFormat", "Makefile-Foundation-DataExchangeFormat");
		mkDirWithLinks2("Foundation/DataExchangeFormat/JSON", "Makefile-Foundation-DataExchangeFormat-JSON");
		mkDirWithLinks2("Foundation/DataExchangeFormat/XML", "Makefile-Foundation-DataExchangeFormat-XML");
		mkDirWithLinks("Foundation/Debug", "Makefile-Foundation-Debug");
		mkDirWithLinks("Foundation/Execution", "Makefile-Foundation-Execution");
		mkdir "$intermediateFiles/$platform/$target/Library/Foundation/Execution/Platform";
		mkDirWithLinks3("Foundation/Execution/Platform/POSIX", "Makefile-Foundation-Execution-Platform-POSIX");
		mkDirWithLinks("Foundation/IO", "Makefile-Foundation-IO");
		mkDirWithLinks2("Foundation/IO/FileSystem", "Makefile-Foundation-IO-FileSystem");
		mkDirWithLinks2("Foundation/IO/Network", "Makefile-Foundation-IO-Network");
		mkDirWithLinks3("Foundation/IO/Network/Transfer", "Makefile-Foundation-IO-Network-Transfer");
		mkDirWithLinks3("Foundation/IO/Network/HTTP", "Makefile-Foundation-IO-Network-HTTP");
		mkDirWithLinks("Foundation/Linguistics", "Makefile-Foundation-Linguistics");
		mkDirWithLinks("Foundation/Math", "Makefile-Foundation-Math");
		mkDirWithLinks("Foundation/Memory", "Makefile-Foundation-Memory");
		mkDirWithLinks("Foundation/Streams", "Makefile-Foundation-Streams");
		mkDirWithLinks2("Foundation/Streams/iostream", "Makefile-Foundation-Streams-iostream");
		mkDirWithLinks("Foundation/Time", "Makefile-Foundation-Time");

		system ("ln -s ../../../../../Library/Projects/Linux/Makefile-Frameworks $intermediateFiles/$platform/$target/Library/Frameworks/Makefile");

		mkDirWithLinks("Frameworks/Service", "Makefile-Frameworks-Service");
		mkDirWithLinks("Frameworks/WebServer", "Makefile-Frameworks-WebServer");

		mkdir "$intermediateFiles/$platform/$target/Tools";
		mkdir "$intermediateFiles/$platform/$target/Tools/Frameworks/";
		mkdir "$intermediateFiles/$platform/$target/Tools/Frameworks/WebServer";
		mkdir "$intermediateFiles/$platform/$target/Tools/Frameworks/WebServer/HTMLViewCompiler";
		system ("ln -s ../../../../../Tools/Projects/Linux/Makefile-Frameworks $intermediateFiles/$platform/$target/Tools/Frameworks/Makefile");
		system ("ln -s ../../../../../../Tools/Projects/Linux/Makefile-Frameworks-WebServer $intermediateFiles/$platform/$target/Tools/Frameworks/WebServer/Makefile");
		system ("ln -s ../../../../../../../Tools/Projects/Linux/Makefile-Frameworks-WebServer-HTMLViewCompiler $intermediateFiles/$platform/$target/Tools/Frameworks/WebServer/HTMLViewCompiler/Makefile");

		foreach $tst (GetAllTests ()) {
			mkdir "$intermediateFiles/$platform/$target/Test$tst";
			system ("ln -s ../../../../Tests/Projects/Linux/$tst/Makefile $intermediateFiles/$platform/$target/Test$tst/Makefile");
		}
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
	print (OUT "\n");



	print (OUT "//--enable-assertions or --disable-assertions to force a particular value. --default-assertions to depend on NDEBUG\n");
	if ($ENABLE_ASSERTIONS == -1) {
		print (OUT "// UNSET so defaulting (to 0 iff NDEBUG set)\n");
	}
	else {
		print (OUT "// set\n");
		if ($ENABLE_ASSERTIONS) {
			print (OUT "#define	qDebug 1\n");
		}	
		else {
			print (OUT "#define	qDebug 0\n");
		}
		print (OUT "\n");
	}
	print (OUT "\n");


	print (OUT "//--has-libcurl or --no-has-libcurl\n");
	if ($ENABLE_LIBCURL) {
		print (OUT "#define	qHasFeature_libcurl 1\n");
	}	
	else {
		print (OUT "#define	qHasFeature_libcurl 0\n");
	}	
	print (OUT "\n");


	print (OUT "//--has-winhttp or --no-has-winhttp\n");
	if ($ENABLE_WINHTTP) {
		print (OUT "#define	qHasFeature_WinHTTP 1\n");
	}	
	else {
		print (OUT "#define	qHasFeature_WinHTTP 0\n");
	}	
	print (OUT "\n");



	print (OUT "//--enable-trace2file or --disable-trace2file\n");
	if ($ENABLE_TRACE2FILE == -1) {
		print (OUT "// UNSET so defaulting\n");
	}
	else {
		print (OUT "// set\n");
		if ($ENABLE_TRACE2FILE) {
			print (OUT "#define	qTraceToFile 1\n");
		}	
		else {
			print (OUT "#define	qTraceToFile 0\n");
		}	
		print (OUT "\n");
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
	print (OUT "#include	\"Private/Defaults_Cache_LRUCache_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Cache_TimedCache_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Characters_Character_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Characters_StringUtils_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Characters_TChar_.h\"\n");
	print (OUT "#include	\"Private/Defaults_CompilerAndStdLib_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Cryptography_SSL_.h\"\n");
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



# Eventually  -make this more central - put all configs here, and then
# build C++/dirs etc from this!
sub	WriteConfigFile_
{
	my $PROJECTPLATFORMSUBDIR='';
	if ("$^O" eq "linux") {
		$PROJECTPLATFORMSUBDIR = 'Linux';
	}
	if ("$^O" eq "cygwin") {
		# first try vs 2k12
		if ($PROJECTPLATFORMSUBDIR eq "") {
			local $PROGRAMFILESDIR= `cygpath \"$ENV{'PROGRAMFILES'}\"`;
			local $PROGRAMFILESDIR2= "/cygdrive/c/Program Files (x86)/";
			if (-e "$PROGRAMFILESDIR/Microsoft Visual Studio 11.0/VC") {
				$PROJECTPLATFORMSUBDIR = 'VisualStudio.Net-2012';
			}
			if (-e "$PROGRAMFILESDIR2/Microsoft Visual Studio 11.0/VC") {
				$PROJECTPLATFORMSUBDIR = 'VisualStudio.Net-2012';
			}
		}

		# first try vs 2k10
		if ($PROJECTPLATFORMSUBDIR eq "") {
			local $PROGRAMFILESDIR= `cygpath \"$ENV{'PROGRAMFILES'}\"`;
			local $PROGRAMFILESDIR2= "/cygdrive/c/Program Files (x86)/";
			if (-e "$PROGRAMFILESDIR/Microsoft Visual Studio 10.0/VC") {
				$PROJECTPLATFORMSUBDIR = 'VisualStudio.Net-2010';
			}
			if (-e "$PROGRAMFILESDIR2/Microsoft Visual Studio 10.0/VC") {
				$PROJECTPLATFORMSUBDIR = 'VisualStudio.Net-2010';
			}
		}
	}

	open(OUT,">$masterXMLConfigFile");
	print (OUT "<Configuration>\n");
	print (OUT "    <ProjectPlatformSubdir>$PROJECTPLATFORMSUBDIR</ProjectPlatformSubdir>\n");
	print (OUT "</Configuration>\n");
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


	if ($ENABLE_ASSERTIONS == 1) {
		print (OUT "ENABLE_GLIBC_ASSERTIONS=1\n");
	}

	if ($ENABLE_LIBCURL) {
		print (OUT "qHasFeature_libcurl=1\n");
	}	
	else {
		print (OUT "qHasFeature_libcurl=0\n");
	}	

	print (OUT "#Defaults overrides:\n");

	print (OUT "\n");
	print (OUT "#Configure Command Line Arguments (-make-define)\n");
	foreach $var (@useExtraMakeDefines)
	{
		print (OUT "$var\n");
	}
	
	
	print (OUT "\n");
	print (OUT "STATIC_LINK_GCCRUNTIME=	$STATIC_LINK_GCCRUNTIME\n");
	print (OUT "IF_STATIC_LINK_GCCRUNTIME_USE_PRINTPATH_METHOD=	$IF_STATIC_LINK_GCCRUNTIME_USE_PRINTPATH_METHOD\n");
	print (OUT "INCLUDE_SYMBOLS=			$INCLUDE_SYMBOLS\n");
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



#print ("OS: $^O\n");



if ($forceRecreate) {
    system ("rm -f $configFileCName $configFileMakeName");
}

if ("$^O" eq "linux") {
    MakeUnixDirs ();
}

unless (-e $intermediateFiles) {
    mkdir ($intermediateFiles);
}

unless (-e $masterXMLConfigFile) {
	print("Writing \"$masterXMLConfigFile\"...\n");
	WriteConfigFile_ ();
}

unless (-e $configFileCName) {
	print("Writing \"$configFileCName\"...\n");
	WriteStroikaConfigCHeader ();
}


unless (-e $configFileMakeName) {
	print("Writing \"$configFileMakeName\"...\n");
	WriteStroikaConfigMakeHeader ();
}
