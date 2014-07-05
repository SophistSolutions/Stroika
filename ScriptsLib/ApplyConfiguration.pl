#!/usr/bin/perl

# MUST FIX TO CHECK FOR --only-if-unconfigured - for now thats all the logic we'll support
# but should check flag...
#TRY THESE AGAIN, but for now generate too many problems...
#use strict;
#use warnings;

require ("Tests/ScriptsLib/TestsList.pl");
require ("ScriptsLib/StringUtils.pl");
require ("ScriptsLib/ConfigurationReader.pl");

use constant false => 0;
use constant true  => 1;
use constant DEFAULT_BOOL_OPTIONS => -1;



my $intermediateFiles	=	"IntermediateFiles/";
my $platform			=	"Platform_Linux";
my $target			=	"Debug";
my $currentConfiguration	=	"DefaultConfiguration";

my $PROJECTPLATFORMSUBDIR='';

my $forceRecreate = true;

my $useThirdPartyXerces	=	true;

my @useExtraCDefines;
my @useExtraMakeDefines;

## FOR NOW ONLY USED ON LINUX BUILDS
my $ENABLE_ASSERTIONS = DEFAULT_BOOL_OPTIONS;
my $ENABLE_OPENSSL = 0;
my $ENABLE_LIBCURL = 0;
my $ENABLE_WINHTTP = 0;
my $ENABLE_TRACE2FILE = DEFAULT_BOOL_OPTIONS;
my $INCLUDE_SYMBOLS = 1;
my $COPTIMIZE_FLAGS = "";
my $STATIC_LINK_GCCRUNTIME = 1;
my $COMPILER_DRIVER = "";

#quite unclear why this is needed - or even if it is - but it appears to be that
# just regular
my $IF_STATIC_LINK_GCCRUNTIME_USE_PRINTPATH_METHOD = 1;


sub	DoHelp_
{
	print("Usage:\n");
	print("	ApplyConfiguration.pl OPTIONS where options can be:\n");
	print("	    --force                      /* forces rebuild of the given configuration */\n");
	print("	    --only-if-unconfigured       /* Opposite of --force - only rebuilds the configfiles if absent */\n");
	exit (0);
}

sub	ParseCommandLine_
{
	for ($i = 0; $i <= $#ARGV; $i++) {
		my $var = $ARGV[$i];
		if (lc ($var) eq "-force" or lc ($var) eq "--force") {
			$forceRecreate = true;
		}
		if ((lc ($var) eq "-only-if-unconfigured") or (lc ($var) eq "--only-if-unconfigured")) {
			$forceRecreate = false;
		}
		if ((lc ($var) eq "-help") or (lc ($var) eq "--help") or (lc ($var) eq "-?")) {
			DoHelp_ ();
		}
	}
}


sub	ConfigParam2BoolInt
{
	my $v = shift;
	if ($v eq "0") {
		return 0;
	}
	if ($v eq "1") {
		return 1;
	}
	return DEFAULT_BOOL_OPTIONS;
}

sub	ReadConfiguration_
{
	$PROJECTPLATFORMSUBDIR = GetConfigurationParameter("ProjectPlatformSubdir");
	$target = GetConfigurationParameter("Target");
	$platform = GetConfigurationParameter("Platform");
	$COMPILER_DRIVER = GetConfigurationParameter("CompilerDriver");

	$useThirdPartyXerces = ConfigParam2BoolInt (GetConfigurationParameter("qHasLibrary_Xerces"));

	$ENABLE_OPENSSL = ConfigParam2BoolInt (GetConfigurationParameter("qHasFeature_openssl"));
	$ENABLE_LIBCURL = ConfigParam2BoolInt (GetConfigurationParameter("qHasFeature_libcurl"));
	$ENABLE_WINHTTP = ConfigParam2BoolInt (GetConfigurationParameter("qHasFeature_WinHTTP"));
	$ENABLE_ASSERTIONS = ConfigParam2BoolInt (GetConfigurationParameter("ENABLE_ASSERTIONS"));
	$ENABLE_GLIBCXX_DEBUG = ConfigParam2BoolInt (GetConfigurationParameter("ENABLE_GLIBCXX_DEBUG"));
	$ENABLE_TRACE2FILE = ConfigParam2BoolInt (GetConfigurationParameter("ENABLE_TRACE2FILE"));
	$INCLUDE_SYMBOLS = ConfigParam2BoolInt (GetConfigurationParameter("IncludeDebugSymbtolsInExecutables"));
	$COPTIMIZE_FLAGS = GetConfigurationParameter("OptimizerFlag");
	if (not (defined $COPTIMIZE_FLAGS)) {
		$COPTIMIZE_FLAGS = "";
	}
	$STATIC_LINK_GCCRUNTIME = ConfigParam2BoolInt (GetConfigurationParameter("STATIC_LINK_GCCRUNTIME"));


	foreach $cdef (GetConfigurationParameter("ExtraCDefines")) {
		$useExtraCDefines[@useExtraCDefines] = $cdef;
	}

	foreach $mdef (GetConfigurationParameter("ExtraMakeDefines")) {
		push (@useExtraMakeDefines, $mdef);
	}


	#print "PROJECTPLATFORMSUBDIR = $PROJECTPLATFORMSUBDIR\n";
	#print "TARGET = $target\n";
	#print "platform = $platform\n";
}


ParseCommandLine_ ();

ReadConfiguration_();



#if ($forceRecreate) {
#	print "Forcing recreate...\n";
#}

mkdir ($intermediateFiles);



#
# For now KISS - just check if the file doesn't exist, and if so write a default value.
#
my $configFileCName		=	"Library/Sources/Stroika/Foundation/Configuration/StroikaConfig.h";
my $configFileMakeName	=	"$intermediateFiles$currentConfiguration/Library/Configuration.mk";





sub mkDirWithLinks
{
	local $relPath = $_[0];
	local $makefileName = $_[1];

	mkdir "$intermediateFiles/$currentConfiguration/Library/$relPath";
	system ("ln -s ../../../../../Library/Projects/Linux/$makefileName $intermediateFiles$currentConfiguration/Library/$relPath/Makefile");
}
sub mkDirWithLinks2
{
	local $relPath = $_[0];
	local $makefileName = $_[1];

	mkdir "$intermediateFiles/$currentConfiguration/Library/$relPath";
	system ("ln -s ../../../../../../Library/Projects/Linux/$makefileName $intermediateFiles$currentConfiguration/Library/$relPath/Makefile");
}
sub mkDirWithLinks3
{
	local $relPath = $_[0];
	local $makefileName = $_[1];

	mkdir "$intermediateFiles/$currentConfiguration/Library/$relPath";
	system ("ln -s ../../../../../../../Library/Projects/Linux/$makefileName $intermediateFiles$currentConfiguration/Library/$relPath/Makefile");
}


sub MakeUnixDirs {
	if ($forceRecreate) {
		system ("rm -rf $intermediateFiles/$currentConfiguration");
	}
	unless (-e "$intermediateFiles/$currentConfiguration") {
		mkdir "$intermediateFiles$currentConfiguration";
		mkdir "$intermediateFiles$currentConfiguration/Library";
		mkdir "$intermediateFiles$currentConfiguration/Library/Foundation";
		mkdir "$intermediateFiles$currentConfiguration/Library/Frameworks";

		system ("ln -s ../../../../Library/Projects/Linux/Makefile-Foundation $intermediateFiles/$currentConfiguration/Library/Foundation/Makefile");
		#system ("cp Library/Projects/Linux/Configuration-Default.mk $intermediateFiles$currentConfiguration/Library/Configuration.mk");
		system ("cp Library/Projects/Linux/SharedBuildRules-Default.mk $intermediateFiles$currentConfiguration/Library/SharedBuildRules.mk");
		system ("cp Library/Projects/Linux/SharedMakeVariables-Default.mk $intermediateFiles$currentConfiguration/Library/SharedMakeVariables.mk");

		mkDirWithLinks("Foundation/Cache", "Makefile-Foundation-Cache");
		mkDirWithLinks("Foundation/Characters", "Makefile-Foundation-Characters");
		mkDirWithLinks2("Foundation/Characters/CString", "Makefile-Foundation-Characters-CString");
		mkDirWithLinks2("Foundation/Characters/Concrete", "Makefile-Foundation-Characters-Concrete");
		mkDirWithLinks3("Foundation/Characters/Concrete/Private", "Makefile-Foundation-Characters-Concrete-Private");
		mkDirWithLinks("Foundation/Common", "Makefile-Foundation-Common");
		mkDirWithLinks("Foundation/Configuration", "Makefile-Foundation-Configuration");
		mkDirWithLinks("Foundation/Containers", "Makefile-Foundation-Containers");
		mkDirWithLinks("Foundation/Cryptography", "Makefile-Foundation-Cryptography");
		mkDirWithLinks2("Foundation/Cryptography/Hash", "Makefile-Foundation-Cryptography-Hash");
		mkDirWithLinks3("Foundation/Cryptography/Hash/Algorithms", "Makefile-Foundation-Cryptography-Hash-Algorithms");
		mkDirWithLinks2("Foundation/Cryptography/SSL", "Makefile-Foundation-Cryptography-SSL");
		mkDirWithLinks("Foundation/Database", "Makefile-Foundation-Database");
		mkDirWithLinks("Foundation/DataExchange", "Makefile-Foundation-DataExchange");
		mkDirWithLinks2("Foundation/DataExchange/INI", "Makefile-Foundation-DataExchange-INI");
		mkDirWithLinks2("Foundation/DataExchange/JSON", "Makefile-Foundation-DataExchange-JSON");
		mkDirWithLinks2("Foundation/DataExchange/XML", "Makefile-Foundation-DataExchange-XML");
		mkDirWithLinks("Foundation/Debug", "Makefile-Foundation-Debug");
		mkDirWithLinks("Foundation/Execution", "Makefile-Foundation-Execution");
		mkdir "$intermediateFiles$currentConfiguration/Library/Foundation/Execution/Platform";
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
		mkDirWithLinks("Foundation/Traversal", "Makefile-Foundation-Traversal");

		system ("ln -s ../../../../Library/Projects/Linux/Makefile-Frameworks $intermediateFiles$currentConfiguration/Library/Frameworks/Makefile");

		mkDirWithLinks("Frameworks/Service", "Makefile-Frameworks-Service");
		mkDirWithLinks("Frameworks/UPnP", "Makefile-Frameworks-UPnP");
		mkDirWithLinks2("Frameworks/UPnP/SSDP", "Makefile-Frameworks-UPnP-SSDP");
		mkDirWithLinks3("Frameworks/UPnP/SSDP/Client", "Makefile-Frameworks-UPnP-SSDP-Client");
		mkDirWithLinks3("Frameworks/UPnP/SSDP/Server", "Makefile-Frameworks-UPnP-SSDP-Server");
		mkDirWithLinks("Frameworks/SystemPerformance", "Makefile-Frameworks-SystemPerformance");
		mkDirWithLinks2("Frameworks/SystemPerformance/Instruments", "Makefile-Frameworks-SystemPerformance-Instruments");
		mkDirWithLinks("Frameworks/WebServer", "Makefile-Frameworks-WebServer");

		mkdir "$intermediateFiles$currentConfiguration/Tools";
		mkdir "$intermediateFiles$currentConfiguration/Tools/Frameworks/";
		mkdir "$intermediateFiles$currentConfiguration/Tools/Frameworks/WebServer";
		mkdir "$intermediateFiles$currentConfiguration/Tools/Frameworks/WebServer/HTMLViewCompiler";
		system ("ln -s ../../../../Tools/Projects/Linux/Makefile-Frameworks $intermediateFiles$currentConfiguration/Tools/Frameworks/Makefile");
		system ("ln -s ../../../../../Tools/Projects/Linux/Makefile-Frameworks-WebServer $intermediateFiles$currentConfiguration/Tools/Frameworks/WebServer/Makefile");
		system ("ln -s ../../../../../../Tools/Projects/Linux/Makefile-Frameworks-WebServer-HTMLViewCompiler $intermediateFiles$currentConfiguration/Tools/Frameworks/WebServer/HTMLViewCompiler/Makefile");

		mkdir "$intermediateFiles$currentConfiguration/Samples_SSDPClient";
		system ("ln -s ../../../Samples/SSDPClient/Projects/Linux/Makefile $intermediateFiles$currentConfiguration/Samples_SSDPClient/Makefile");
		mkdir "$intermediateFiles$currentConfiguration/Samples_SSDPServer";
		system ("ln -s ../../../Samples/SSDPServer/Projects/Linux/Makefile $intermediateFiles$currentConfiguration/Samples_SSDPServer/Makefile");

		mkdir "$intermediateFiles$currentConfiguration/Samples_SystemPerformanceClient";
		system ("ln -s ../../../Samples/SystemPerformanceClient/Projects/Linux/Makefile $intermediateFiles$currentConfiguration/Samples_SystemPerformanceClient/Makefile");

		mkdir "$intermediateFiles$currentConfiguration/Samples_WebServer";
		system ("ln -s ../../../Samples/WebServer/Projects/Linux/Makefile $intermediateFiles$currentConfiguration/Samples_WebServer/Makefile");

		mkdir "$intermediateFiles$currentConfiguration/Samples_SimpleService";
		system ("ln -s ../../../Samples/SimpleService/Projects/Linux/Makefile $intermediateFiles$currentConfiguration/Samples_SimpleService/Makefile");

		foreach $tst (GetAllTests ()) {
			mkdir "$intermediateFiles$currentConfiguration/Test$tst";
			system ("ln -s ../../../Tests/Projects/Linux/$tst/Makefile $intermediateFiles$currentConfiguration/Test$tst/Makefile");
		}
	}
}





sub WriteStroikaConfigCHeader
{
	#open(OUT,">$configFileCName");
	open(OUT,">:crlf", "$configFileCName");
	print (OUT "/*\n");
	print (OUT " * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved\n");
	print (OUT " */\n");
	print (OUT "#ifndef	_Stroika_Foundation_Configuration_StroikaConfig_h_\n");
	print (OUT "#define	_Stroika_Foundation_Configuration_StroikaConfig_h_	1\n");
	print (OUT "\n");
	print (OUT "//WARNING: THIS FILE WAS AUTOGENERATED WITH ApplyConfiguration.pl\n");
	print (OUT "//WARNING: THIS FILE WAS AUTOGENERATED WITH ApplyConfiguration.pl\n");
	print (OUT "//WARNING: THIS FILE WAS AUTOGENERATED WITH ApplyConfiguration.pl\n");
	print (OUT "//WARNING: THIS FILE WAS AUTOGENERATED WITH ApplyConfiguration.pl\n");
	print (OUT "//WARNING: THIS FILE WAS AUTOGENERATED WITH ApplyConfiguration.pl\n");
	print (OUT "//WARNING: THIS FILE WAS AUTOGENERATED WITH ApplyConfiguration.pl\n");
	print (OUT "//WARNING: THIS FILE WAS AUTOGENERATED WITH ApplyConfiguration.pl\n");
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
	if ($ENABLE_ASSERTIONS == DEFAULT_BOOL_OPTIONS) {
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


	print (OUT "//--has-openssl or --no-has-openssl\n");
	if ($ENABLE_OPENSSL) {
		print (OUT "#define	qHas_OpenSSL 1\n");
	}	
	else {
		print (OUT "#define	qHas_OpenSSL 0\n");
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
	if ($ENABLE_TRACE2FILE == DEFAULT_BOOL_OPTIONS) {
		print (OUT "// UNSET so defaulting\n");
	}
	else {
		print (OUT "// set\n");
		if ($ENABLE_TRACE2FILE) {
			print (OUT "#define	qTraceToFile 1\n");
			# though this is could be separately configured, it almost only makes sense to turn these on together and confusing
			# if one done but not the other.
			print (OUT "#define	qDefaultTracingOn 1\n");
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
	print (OUT "#include	\"Private/Defaults_Characters_Character_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Characters_StringUtils_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Characters_TChar_.h\"\n");
	print (OUT "#include	\"Private/Defaults_CompilerAndStdLib_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Cryptography_SSL_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Database_ODBCClient_.h\"\n");
	print (OUT "#include	\"Private/Defaults_DataExchange_XML_Common_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Debug_Trace_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Execution_Logging_.h\"\n");
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
	mkdir ("$intermediateFiles");
	mkdir ("$intermediateFiles$currentConfiguration");
	mkdir ("$intermediateFiles$currentConfiguration/Library");
	
	open(OUT,">$configFileMakeName");
	print (OUT "#\n");
	print (OUT "#Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved\n");
	print (OUT "#\n");
	print (OUT "\n");
	print (OUT "\n");
	print (OUT "#WARNING: THIS FILE WAS AUTOGENERATED WITH ApplyConfiguration.pl\n");
	print (OUT "#WARNING: THIS FILE WAS AUTOGENERATED WITH ApplyConfiguration.pl\n");
	print (OUT "#WARNING: THIS FILE WAS AUTOGENERATED WITH ApplyConfiguration.pl\n");
	print (OUT "#WARNING: THIS FILE WAS AUTOGENERATED WITH ApplyConfiguration.pl\n");
	print (OUT "#WARNING: THIS FILE WAS AUTOGENERATED WITH ApplyConfiguration.pl\n");
	print (OUT "#WARNING: THIS FILE WAS AUTOGENERATED WITH ApplyConfiguration.pl\n");
	print (OUT "#WARNING: THIS FILE WAS AUTOGENERATED WITH ApplyConfiguration.pl\n");
	print (OUT "#WARNING: THIS FILE WAS AUTOGENERATED WITH ApplyConfiguration.pl\n");
	print (OUT "\n");
	print (OUT "\n");


	// If $ENABLE_GLIBCXX_DEBUG defaulted, use $ENABLE_ASSERTIONS
	if ($ENABLE_GLIBCXX_DEBUG == 1 || ($ENABLE_GLIBCXX_DEBUG == DEFAULT_BOOL_OPTIONS && $ENABLE_ASSERTIONS == 1) {
		print (OUT "ENABLE_GLIBCXX_DEBUG=1\n");
	}

	if ($useThirdPartyXerces) {
		print (OUT "qHasLibrary_Xerces=1\n");
	}	
	else {
		print (OUT "qHasLibrary_Xerces=0\n");
	}	

	if ($ENABLE_LIBCURL) {
		print (OUT "qHasFeature_libcurl=1\n");
	}	
	else {
		print (OUT "qHasFeature_libcurl=0\n");
	}	

	print (OUT "#Third Party Product Libs to Build:\n");
	# VERY ROUGH DRAFT - NEED UPSTREAM BETTER INFO ABOUT WHAT TO BUILD
	if ($useThirdPartyXerces) {
		print (OUT "qBuildThirdPartyProducts_Xerces=1\n");
	}	
	else {
		print (OUT "qBuildThirdPartyProducts_Xerces=0\n");
	}	
	if ($ENABLE_LIBCURL) {
		print (OUT "qBuildThirdPartyProducts_libcurl=1\n");
	}	
	else {
		print (OUT "qBuildThirdPartyProducts_libcurl=0\n");
	}	
	print (OUT "qBuildThirdPartyProducts_zlib=1\n");
	if ($ENABLE_OPENSSL) {
		print (OUT "qBuildThirdPartyProducts_OpenSSL=1\n");
	}	
	else {
		print (OUT "qBuildThirdPartyProducts_OpenSSL=0\n");
	}	



	print (OUT "\n\n");
	print (OUT "#Configured Command Line Arguments (-make-define)\n");
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
	print (OUT "CPlusPlus=	$COMPILER_DRIVER\n");
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

if (("$^O" eq "linux") or ("$^O" eq "darwin")) {
    MakeUnixDirs ();
}

unless (-e $intermediateFiles) {
    mkdir ($intermediateFiles);
}

unless (-e $configFileCName) {
	print("Writing \"$configFileCName\"...\n");
	WriteStroikaConfigCHeader ();
}


unless (-e $configFileMakeName) {
	print("Writing \"$configFileMakeName\"...\n");
	WriteStroikaConfigMakeHeader ();
}
