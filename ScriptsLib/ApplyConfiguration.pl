#!/usr/bin/perl

# MUST FIX TO CHECK FOR --only-if-unconfigured - for now thats all the logic we'll support
# but should check flag...
#TRY THESE AGAIN, but for now generate too many problems...
#use strict;
#use warnings;

BEGIN{ @INC = ( "./", @INC ); }

require ("Tests/ScriptsLib/TestsList.pl");
require ("ScriptsLib/StringUtils.pl");
require ("ScriptsLib/ConfigurationReader.pl");

use constant false => 0;
use constant true  => 1;
use constant DEFAULT_BOOL_OPTIONS => -1;


my $MAKE_INDENT_LEVEL = $ENV{'MAKE_INDENT_LEVEL'};
$MAKE_INDENT_LEVEL = $MAKE_INDENT_LEVEL + 1;

my $intermediateFiles	=	"IntermediateFiles/";
my $activeConfiguration	=	$ARGV[0];


my $PROJECTPLATFORMSUBDIR='';

my $forceRecreate = true;


my @useExtraCDefines;
my @useExtraMakeDefines;


#
# BUILD iff LIBFEATUREFLAG_BuildOnly OR LIBFEATUREFLAG_UseStaticTPP
# HAS_FEATURE iff LIBFEATUREFLAG_UseStaticTPP OR LIBFEATUREFLAG_UseSystem
#
my $LIBFEATUREFLAG_BuildOnly = "build-only";
my $LIBFEATUREFLAG_UseStaticTPP = "use";
my $LIBFEATUREFLAG_UseSystem = "use-system";
my $LIBFEATUREFLAG_No = "no";


## FOR NOW ONLY USED ON Unix BUILDS (may not be true anymore - gradually changing so always used)
my $ENABLE_ASSERTIONS = 0;
my $FEATUREFLAG_LIBCURL = $LIBFEATUREFLAG_No;
my $FEATUREFLAG_OpenSSL = $LIBFEATUREFLAG_UseStaticTPP;
my $FEATUREFLAG_XERCES = $LIBFEATUREFLAG_UseStaticTPP;
my $FEATUREFLAG_ZLib = $LIBFEATUREFLAG_UseStaticTPP;
my $FEATUREFLAG_sqlite = $LIBFEATUREFLAG_UseStaticTPP;
my $FEATUREFLAG_boost = $LIBFEATUREFLAG_UseStaticTPP;
my $FEATUREFLAG_LZMA = $LIBFEATUREFLAG_UseStaticTPP;
my $ENABLE_TRACE2FILE = DEFAULT_BOOL_OPTIONS;
my $COPTIMIZE_FLAGS = "";
my $STATIC_LINK_GCCRUNTIME = 1;
my $COMPILER_DRIVER_C = "";
my $COMPILER_DRIVER_CPlusPlus = "";
my $AR = undef;
my $RANLIB = undef;
my $EXTRA_COMPILER_ARGS = "";
my $EXTRA_PREFIX_LINKER_ARGS = "";
my $EXTRA_SUFFIX_LINKER_ARGS = "";


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
	if ($v eq "false") {
		return 0;
	}
	if ($v eq "1") {
		return 1;
	}
	if ($v eq "true") {
		return 1;
	}
	return DEFAULT_BOOL_OPTIONS;
}

sub	ReadConfiguration_
{
	$PROJECTPLATFORMSUBDIR = GetConfigurationParameter($activeConfiguration, "ProjectPlatformSubdir");
	$COMPILER_DRIVER_C = GetConfigurationParameter($activeConfiguration, "CompilerDriver-C");
	$COMPILER_DRIVER_CPlusPlus = GetConfigurationParameter($activeConfiguration, "CompilerDriver-C++");
	$AR = GetConfigurationParameter($activeConfiguration, "AR");
	$RANLIB = GetConfigurationParameter($activeConfiguration, "RANLIB");
	$EXTRA_COMPILER_ARGS = GetConfigurationParameter($activeConfiguration, "EXTRA_COMPILER_ARGS");
	$EXTRA_PREFIX_LINKER_ARGS = GetConfigurationParameter($activeConfiguration, "EXTRA_PREFIX_LINKER_ARGS");
	$EXTRA_SUFFIX_LINKER_ARGS = GetConfigurationParameter($activeConfiguration, "EXTRA_SUFFIX_LINKER_ARGS");

	$FEATUREFLAG_LIBCURL = GetConfigurationParameter($activeConfiguration, "qFeatureFlag_LibCurl");
	$FEATUREFLAG_OpenSSL = GetConfigurationParameter($activeConfiguration, "qFeatureFlag_OpenSSL");
	$FEATUREFLAG_WinHTTP = GetConfigurationParameter($activeConfiguration, "qFeatureFlag_WinHTTP");
	$FEATUREFLAG_ATLMFC = GetConfigurationParameter($activeConfiguration, "qFeatureFlag_ATLMFC");
	$FEATUREFLAG_XERCES = GetConfigurationParameter($activeConfiguration, "qFeatureFlag_Xerces");
	$FEATUREFLAG_ZLib = GetConfigurationParameter($activeConfiguration, "qFeatureFlag_ZLib");
	$FEATUREFLAG_sqlite = GetConfigurationParameter($activeConfiguration, "qFeatureFlag_sqlite");
	$FEATUREFLAG_boost = GetConfigurationParameter($activeConfiguration, "qFeatureFlag_boost");
	$FEATUREFLAG_LZMA = GetConfigurationParameter($activeConfiguration, "qFeatureFlag_LZMA");
	$ENABLE_ASSERTIONS = ConfigParam2BoolInt (GetConfigurationParameter($activeConfiguration, "ENABLE_ASSERTIONS"));
	$ENABLE_GLIBCXX_DEBUG = ConfigParam2BoolInt (GetConfigurationParameter($activeConfiguration, "ENABLE_GLIBCXX_DEBUG"));
	$CWARNING_FLAGS = GetConfigurationParameter($activeConfiguration, "CWARNING_FLAGS");
	$INCLUDES_PATH = GetConfigurationParameter($activeConfiguration, "INCLUDES_PATH");
	$LIBS_PATH = GetConfigurationParameter($activeConfiguration, "LIBS_PATH");
	$LIB_DEPENDENCIES = GetConfigurationParameter($activeConfiguration, "LIB_DEPENDENCIES");
	$ENABLE_TRACE2FILE = ConfigParam2BoolInt (GetConfigurationParameter($activeConfiguration, "ENABLE_TRACE2FILE"));
	$COPTIMIZE_FLAGS = GetConfigurationParameter($activeConfiguration, "OptimizerFlag");

	$IncludeDebugSymbolsInExecutables = GetConfigurationParameter($activeConfiguration, "IncludeDebugSymbolsInExecutables");
	$IncludeDebugSymbolsInLibraries = GetConfigurationParameter($activeConfiguration, "IncludeDebugSymbolsInLibraries");


	if (not (defined $COPTIMIZE_FLAGS)) {
		$COPTIMIZE_FLAGS = "";
	}
	$STATIC_LINK_GCCRUNTIME = ConfigParam2BoolInt (GetConfigurationParameter($activeConfiguration, "STATIC_LINK_GCCRUNTIME"));

	foreach $cdef (GetConfigurationParameter($activeConfiguration, "ExtraCDefines")) {
		$useExtraCDefines[@useExtraCDefines] = $cdef;
	}

	foreach $mdef (GetConfigurationParameter($activeConfiguration, "ExtraMakeDefines")) {
		push (@useExtraMakeDefines, $mdef);
	}

	#print "PROJECTPLATFORMSUBDIR = $PROJECTPLATFORMSUBDIR\n";
}


ParseCommandLine_ ();

if ("$^O" eq "cygwin") {
	if (
		$activeConfiguration eq "Debug-U-32" ||
		$activeConfiguration eq "Debug-U-64" ||
		$activeConfiguration eq "Release-DbgMemLeaks-U-32" ||
		$activeConfiguration eq "Release-Logging-U-32" ||
		$activeConfiguration eq "Release-Logging-U-64" ||
		$activeConfiguration eq "Release-U-32" ||
		$activeConfiguration eq "Release-U-64"
	  ) {
	}
	else {
		die ("On windows - for now - $a is not a valid configuraiton name (Debug-U-32, Debug-U-64, Release-DbgMemLeaks-U-32, Release-Logging-U-32, Release-Logging-U-64, Release-U-32, Release-U-64)")
	}
}

ReadConfiguration_();



mkdir ($intermediateFiles);



#
# For now KISS - just check if the file doesn't exist, and if so write a default value.
#
my $configFileCName		=	"$intermediateFiles$activeConfiguration/Stroika-Current-Configuration.h";
my $configFileMakeName	=	"$intermediateFiles$activeConfiguration/Configuration.mk";


my $stkRoot	=	trim(`realpath .`);




sub MakeUnixDirs {
	if ($forceRecreate) {
		system ("rm -rf $intermediateFiles/$activeConfiguration");
	}
	unless (-e "$intermediateFiles/$activeConfiguration") {
		mkdir "$intermediateFiles$activeConfiguration";
		mkdir "$intermediateFiles$activeConfiguration/Library";
	}
}





sub WriteStroikaConfigCHeader
{
	#open(OUT,">$configFileCName");
	open(OUT,">:crlf", "$configFileCName");
	print (OUT "/*\n");
	print (OUT " * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved\n");
	print (OUT " */\n");
	print (OUT "#ifndef	_Stroika_Foundation_Configuration_StroikaCurrentConfiguration_h_\n");
	print (OUT "#define	_Stroika_Foundation_Configuration_StroikaCurrentConfiguration_h_	1\n");
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

	print (OUT "//--enable-assertions or --disable-assertions to force a particular value. --default-assertions to depend on NDEBUG\n");
	 {
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


    print (OUT "//--LibCurl {build-only|use|use-system|no}\n");
	if (($FEATUREFLAG_LIBCURL eq $LIBFEATUREFLAG_UseStaticTPP) || ($FEATUREFLAG_LIBCURL eq $LIBFEATUREFLAG_UseSystem)) {
		print (OUT "#define	qHasFeature_LibCurl 1\n");
	}	
	else {
		print (OUT "#define	qHasFeature_LibCurl 0\n");
	}	
	print (OUT "\n");


    print (OUT "//--OpenSSL {build-only|use|use-system|no}\n");
	if (($FEATUREFLAG_OpenSSL eq $LIBFEATUREFLAG_UseStaticTPP) || ($FEATUREFLAG_OpenSSL eq $LIBFEATUREFLAG_UseSystem)) {
		print (OUT "#define	qHasFeature_OpenSSL 1\n");
	}	
	else {
		print (OUT "#define	qHasFeature_OpenSSL 0\n");
	}	
	print (OUT "\n");


    print (OUT "//--WinHTTP {use-system|no}\n");
	if ($FEATUREFLAG_WinHTTP eq $LIBFEATUREFLAG_UseSystem) {
		print (OUT "#define	qHasFeature_WinHTTP 1\n");
	}	
	else {
		print (OUT "#define	qHasFeature_WinHTTP 0\n");
	}	
	print (OUT "\n");


    print (OUT "//--ATLMFC {use-system|no}\n");
	if ($FEATUREFLAG_ATLMFC eq $LIBFEATUREFLAG_UseSystem) {
		print (OUT "#define	qHasFeature_ATLMFC 1\n");
	}	
	else {
		print (OUT "#define	qHasFeature_ATLMFC 0\n");
	}	
	print (OUT "\n");


     print (OUT "//--Xerces {build-only|use|use-system|no}\n");
	if (($FEATUREFLAG_XERCES eq $LIBFEATUREFLAG_UseStaticTPP) || ($FEATUREFLAG_XERCES eq $LIBFEATUREFLAG_UseSystem)) {
		print (OUT "#define	qHasFeature_Xerces	1\n");
	}
	else {
		print (OUT "#define	qHasFeature_Xerces	0\n");
	}
	print (OUT "\n");


     print (OUT "//--ZLib {build-only|use|use-system|no}\n");
	if (($FEATUREFLAG_ZLib eq $LIBFEATUREFLAG_UseStaticTPP) || ($FEATUREFLAG_ZLib eq $LIBFEATUREFLAG_UseSystem)) {
		print (OUT "#define	qHasFeature_ZLib	1\n");
	}
	else {
		print (OUT "#define	qHasFeature_ZLib	0\n");
	}
	print (OUT "\n");

     print (OUT "//--sqlite {build-only|use|use-system|no}\n");
	if (($FEATUREFLAG_sqlite eq $LIBFEATUREFLAG_UseStaticTPP) || ($FEATUREFLAG_sqlite eq $LIBFEATUREFLAG_UseSystem)) {
		print (OUT "#define	qHasFeature_sqlite	1\n");
	}
	else {
		print (OUT "#define	qHasFeature_sqlite	0\n");
	}
	print (OUT "\n");

    print (OUT "//--LZMA {build-only|use|use-system|no}\n");
	if (($FEATUREFLAG_LZMA eq $LIBFEATUREFLAG_UseStaticTPP) || ($FEATUREFLAG_LZMA eq $LIBFEATUREFLAG_UseSystem)) {
		print (OUT "#define	qHasFeature_LZMA	1\n");
	}
	else {
		print (OUT "#define	qHasFeature_LZMA	0\n");
	}
	print (OUT "\n");

    print (OUT "//--boost {build-only|use|use-system|no}\n");
	if (($FEATUREFLAG_boost eq $LIBFEATUREFLAG_UseStaticTPP) || ($FEATUREFLAG_boost eq $LIBFEATUREFLAG_UseSystem)) {
		print (OUT "#define	qHasFeature_boost	1\n");
	}
	else {
		print (OUT "#define	qHasFeature_boost	0\n");
	}
	print (OUT "\n");


	print (OUT "\n");
	print (OUT "\n");
	print (OUT "\n");


	print (OUT "// --trace2file enable or  --trace2file disable\n");
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

	print (OUT "#endif	/*_Stroika_Foundation_Configuration_StroikaCurrentConfiguration_h_*/\n");
	close(OUT);
}



sub WriteStroikaConfigMakeHeader_CachedLineItem_
{
	local  $vn = $_[0];
	local  $v = @_ >= 2? $_[1] : GetConfigurationParameter($activeConfiguration, $vn);
	print (OUT '#CACHED VALUE OF: ' . $vn . '			:=	$(shell $(StroikaRoot)ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" ' . $vn . ")\n");
	print (OUT "$vn=$v\n");
	print (OUT "\n");
}

sub WriteStroikaConfigMakeHeader
{
	mkdir ("$intermediateFiles");
	mkdir ("$intermediateFiles$activeConfiguration");
	mkdir ("$intermediateFiles$activeConfiguration/Library");
	
	open(OUT,">$configFileMakeName");
	print (OUT "#\n");
	print (OUT "#Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved\n");
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
	print (OUT "#\n");
	print (OUT "#This file mainly consists of CACHES of the values that could have been retrieved WITH\n");
	print (OUT "#      VAR :=	$(shell $(StroikaRoot)ScriptsLib/GetConfigurationParameter \"$(CONFIGURATION)\" VAR)\n");
	print (OUT "\n");
	print (OUT "\n");

	
	print (OUT 'ifndef StroikaRoot' . "\n");
	print (OUT '$error("StroikaRoot must be defined and included before this file")' . "\n");
	print (OUT 'endif' . "\n");
	print (OUT "\n");

	WriteStroikaConfigMakeHeader_CachedLineItem_("ProjectPlatformSubdir", $PROJECTPLATFORMSUBDIR);
	print (OUT "\n");

	##if $ENABLE_GLIBCXX_DEBUG defaulted, use $ENABLE_ASSERTIONS
	#if ($ENABLE_GLIBCXX_DEBUG == 1 || ($ENABLE_GLIBCXX_DEBUG == DEFAULT_BOOL_OPTIONS && $ENABLE_ASSERTIONS == 1)) {
	#	print (OUT "ENABLE_GLIBCXX_DEBUG=1\n");
	#}

	WriteStroikaConfigMakeHeader_CachedLineItem_("ARCH");
	WriteStroikaConfigMakeHeader_CachedLineItem_("CC");
	WriteStroikaConfigMakeHeader_CachedLineItem_("CXX");
	WriteStroikaConfigMakeHeader_CachedLineItem_("AS");
	WriteStroikaConfigMakeHeader_CachedLineItem_("EXTRA_COMPILER_ARGS");
	WriteStroikaConfigMakeHeader_CachedLineItem_("ENABLE_ASSERTIONS", $ENABLE_ASSERTIONS);
	WriteStroikaConfigMakeHeader_CachedLineItem_("ENABLE_GLIBCXX_DEBUG", $ENABLE_GLIBCXX_DEBUG);
	WriteStroikaConfigMakeHeader_CachedLineItem_("LIBS_PATH");
	print (OUT "\n");

	WriteStroikaConfigMakeHeader_CachedLineItem_("OptimizerFlag", $COPTIMIZE_FLAGS);
	WriteStroikaConfigMakeHeader_CachedLineItem_("STRIP");
	WriteStroikaConfigMakeHeader_CachedLineItem_("AR");
	WriteStroikaConfigMakeHeader_CachedLineItem_("RANLIB");
	print (OUT "\n");

	WriteStroikaConfigMakeHeader_CachedLineItem_("RUN_PREFIX");
	WriteStroikaConfigMakeHeader_CachedLineItem_("CrossCompiling");
	print (OUT "\n");


	print (OUT "CONFIGURATION=$activeConfiguration\n");


	print (OUT '# WARNING - qFeatureFlag_EnableAssertions is DEPRECATED as of Stroika v2.1d13 - use ENABLE_ASSERTIONS' . "\n");
	if ($ENABLE_ASSERTIONS) {
		print (OUT "qFeatureFlag_EnableAssertions=1\n");
	}	
	else {
		print (OUT "qFeatureFlag_EnableAssertions=0\n");
	}




	if (not ($CWARNING_FLAGS eq '')) {
		print (OUT "CWARNING_FLAGS= $CWARNING_FLAGS\n");
	}

	WriteStroikaConfigMakeHeader_CachedLineItem_("qFeatureFlag_ActivePerl");
	WriteStroikaConfigMakeHeader_CachedLineItem_("qFeatureFlag_ATLMFC");
	WriteStroikaConfigMakeHeader_CachedLineItem_("qFeatureFlag_boost");
	WriteStroikaConfigMakeHeader_CachedLineItem_("qFeatureFlag_LibCurl");
	WriteStroikaConfigMakeHeader_CachedLineItem_("qFeatureFlag_LZMA");
	WriteStroikaConfigMakeHeader_CachedLineItem_("qFeatureFlag_OpenSSL");
	WriteStroikaConfigMakeHeader_CachedLineItem_("qFeatureFlag_OpenSSL_ExtraArgs");
	WriteStroikaConfigMakeHeader_CachedLineItem_("qFeatureFlag_PrivateOverrideOfCMake");
	WriteStroikaConfigMakeHeader_CachedLineItem_("qFeatureFlag_sqlite");
	WriteStroikaConfigMakeHeader_CachedLineItem_("qFeatureFlag_WinHTTP");
	WriteStroikaConfigMakeHeader_CachedLineItem_("qFeatureFlag_Xerces");
	WriteStroikaConfigMakeHeader_CachedLineItem_("qFeatureFlag_ZLib");
	

	print (OUT "\n\n");
	print (OUT "#Configured Command Line Arguments (-make-define)\n");
	foreach $var (@useExtraMakeDefines)
	{
		print (OUT "$var\n");
	}
	

	print (OUT "INCLUDES_PATH_COMPILER_DIRECTIVES=");
	for my $i (split / /, $INCLUDES_PATH) {
		print (OUT "-I$i ");
	}
	print (OUT "\n");
	print (OUT "LIBS_PATH_DIRECTIVES=");
	for my $l (split /:/, $LIBS_PATH) {
		print (OUT "-L$l ");
	}
	print (OUT "\n");
	print (OUT "LIB_DEPENDENCIES=$LIB_DEPENDENCIES");
	print (OUT "\n");

	print (OUT "\n\n");



	if ($PROJECTPLATFORMSUBDIR eq "Unix") {
		print (OUT "LIBSUFFIX=.a\n");
	}
	elsif ($PROJECTPLATFORMSUBDIR eq "VisualStudio.Net-2017") {
		print (OUT "LIBSUFFIX=.lib\n");
	}

	
	print (OUT "\n");
	print (OUT "\n");
	print (OUT "# Can be blank, or -O2, etc..\n");
	print (OUT "COPTIMIZE_FLAGS=	$COPTIMIZE_FLAGS\n");
	print (OUT "\n");
	print (OUT "\n");
	print (OUT "#Linker-Driver\n");
###	print (OUT "Linker=	\$(CXX)\n");
	WriteStroikaConfigMakeHeader_CachedLineItem_("Linker");

	print (OUT "#\n");
	print (OUT "EXTRA_COMPILER_ARGS=	$EXTRA_COMPILER_ARGS\n");

	print (OUT "#\n");
	print (OUT "EXTRA_PREFIX_LINKER_ARGS=	$EXTRA_PREFIX_LINKER_ARGS\n");
	print (OUT "EXTRA_SUFFIX_LINKER_ARGS=	$EXTRA_SUFFIX_LINKER_ARGS\n");

	print (OUT "IncludeDebugSymbolsInExecutables=	$IncludeDebugSymbolsInExecutables\n");
	print (OUT "IncludeDebugSymbolsInLibraries=	$IncludeDebugSymbolsInLibraries\n");


	if ($PROJECTPLATFORMSUBDIR eq "VisualStudio.Net-2017") {
		print (OUT "\n");
		print (OUT "##\n");
		print (OUT "##\n");
		print (OUT "## Windows format path versions of some of these variables\n");
		print (OUT "##\n");
		print (OUT "##WIN_$$var = cygpath --windows \"\$var\"                OR\n");
		print (OUT "##WIN_$$var = cygpath --windows --path \"\$var\"         (as appropriate)\n");
		print (OUT "##\n");
		print (OUT "##\n");
		my $Linker = GetConfigurationParameter($activeConfiguration, "Linker");
		my $WIN_LD		=	trim (`cygpath --windows "$Linker"`);
		print (OUT "WIN_Linker=$WIN_LD\n");


		my $AR = GetConfigurationParameter($activeConfiguration, "AR");
		my $WIN_AR		=	trim (`cygpath --windows "$AR"`);
		print (OUT "WIN_AR=$WIN_AR\n");

		my $CC = GetConfigurationParameter($activeConfiguration, "CC");
		my $WIN_CC		=	trim (`cygpath --windows "$CC"`);
		print (OUT "WIN_CC=$WIN_CC\n");

		my $CXX = GetConfigurationParameter($activeConfiguration, "CXX");
		my $WIN_CXX		=	trim (`cygpath --windows "$CXX"`);
		print (OUT "WIN_CXX=$WIN_CXX\n");

		my $LIBS_PATH = GetConfigurationParameter($activeConfiguration, "LIBS_PATH");
		my $WIN_LIBS_PATH		=	`cygpath --windows --path "$LIBS_PATH"`;
		print (OUT "WIN_LIBS_PATH=$WIN_LIBS_PATH\n");
		print (OUT "\n");
		print (OUT "\n");
	}



	##### DEPRECATED#####
	print (OUT "\n");
	print (OUT "##\n");
	print (OUT "##\n");
	print (OUT "###DEPRECATED SINCE Stroika 2.1d14\n");
	print (OUT "##\n");
	print (OUT "##\n");
	print (OUT "\n");
	WriteStroikaConfigMakeHeader_CachedLineItem_("CompilerDriver-C");
	WriteStroikaConfigMakeHeader_CachedLineItem_("CompilerDriver-C++");
	print (OUT "#C++-Compiler\n");
	print (OUT "CC_C=	$COMPILER_DRIVER_C\n");
	print (OUT "CC_CPlusPlus=	$COMPILER_DRIVER_CPlusPlus\n");
	print (OUT "CPlusPlus=	$COMPILER_DRIVER_CPlusPlus\n");
	print (OUT "\n");
	print (OUT "\n");

	print (OUT "\n");

	close(OUT);


	#@todo DEPRECATED - BACKWARD COMPAT - REMOVE SOON --LGP 2017-03-14
	#But still dependended on as of 2018-03-23
	system ("cp $configFileMakeName  $intermediateFiles$activeConfiguration/Library/Configuration.mk")
}



#print ("OS: $^O\n");



if ($forceRecreate) {
    system ("rm -f $configFileCName $configFileMakeName");
}

#Really only for UNIX (for now) but harmless on WinDoze
MakeUnixDirs ();

unless (-e $intermediateFiles) {
    mkdir ($intermediateFiles);
}

unless (-e $configFileCName) {
	print(`ScriptsLib/PrintLevelLeader.sh $MAKE_INDENT_LEVEL` . "Writing \"$configFileCName\" ... ");
	WriteStroikaConfigCHeader ();
	print("done\n");
}


unless (-e $configFileMakeName) {
	print(`ScriptsLib/PrintLevelLeader.sh $MAKE_INDENT_LEVEL` . "Writing \"$configFileMakeName\" ... ");
	WriteStroikaConfigMakeHeader ();
	print("done\n");
}
