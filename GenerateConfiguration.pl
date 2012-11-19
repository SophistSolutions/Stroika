#!/usr/bin/perl

#TRY THESE AGAIN, but for now generate too many problems...
#use strict;
#use warnings;

require ("ScriptsLib/StringUtils.pl");

############### NOTE - STILL NOT READY TO BE USED - DRAFT...

use constant false => 0;
use constant true  => 1;
use constant DEFAULT_BOOL_OPTIONS => -1;

my $intermediateFiles	=	"test_IntermediateFiles/";
my $platform			=	"";
my $target			=	"";

my $masterXMLConfigFile	=	"$intermediateFiles/Configuration.xml";






########### CONFIG_Variables ###############
my $PROJECTPLATFORMSUBDIR='';

my $useThirdPartyXerces	=	true;

my @useExtraCDefines;
my @useExtraMakeDefines;

my $ENABLE_ASSERTIONS = DEFAULT_BOOL_OPTIONS;
my $ENABLE_LIBCURL = 0;
my $ENABLE_WINHTTP = 0;
my $ENABLE_TRACE2FILE = DEFAULT_BOOL_OPTIONS;
my $INCLUDE_SYMBOLS = 1;
my $COPTIMIZE_FLAGS = "";
my $STATIC_LINK_GCCRUNTIME = DEFAULT_BOOL_OPTIONS;
my $COMPILER_DRIVER = "";

#quite unclear why this is needed - or even if it is - but it appears to be that
# just regular
my $IF_STATIC_LINK_GCCRUNTIME_USE_PRINTPATH_METHOD = 1;





sub	DoHelp_
{
	print("Usage:\n");
	print("	GenerateConfiguration.pl OPTIONS where options can be:\n");
	print("	    --default-for-platform       /* May create multiple targets (recursive call to configure) - but generates all the default settings for this platform */\n");
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
	print("	    --compiler-driver {ARG}      /* default is g++ */\n");

	exit (0);
}


### Initial defaults before looking at command-line arguments
sub	SetInitialDefaults_
{
	if ("$^O" eq "linux") {
		$PROJECTPLATFORMSUBDIR = 'Linux';
	}
	if ("$^O" eq "cygwin") {
		# first try vs 2k12
		if ($PROJECTPLATFORMSUBDIR eq "") {
			local $PROGRAMFILESDIR= trim (`cygpath \"$ENV{'PROGRAMFILES'}\"`);
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
			local $PROGRAMFILESDIR= trim (`cygpath \"$ENV{'PROGRAMFILES'}\"`);
			local $PROGRAMFILESDIR2= "/cygdrive/c/Program Files (x86)/";
			if (-e "$PROGRAMFILESDIR/Microsoft Visual Studio 10.0/VC") {
				$PROJECTPLATFORMSUBDIR = 'VisualStudio.Net-2010';
			}
			if (-e "$PROGRAMFILESDIR2/Microsoft Visual Studio 10.0/VC") {
				$PROJECTPLATFORMSUBDIR = 'VisualStudio.Net-2010';
			}
		}
	}

	if ("$^O" eq "linux" && -e '/usr/include/curl/curl.h') {
		$ENABLE_LIBCURL = 1;
	}
	if ("$^O" eq "cygwin") {
		$ENABLE_WINHTTP = 1;
	}
}


sub	SetDefaultForCompilerDriver_
{
	if ($PROJECTPLATFORMSUBDIR eq 'Linux') {
		$STATIC_LINK_GCCRUNTIME = 1;
	}
}

sub	SetDefaultForPlatform_
{
	if ($PROJECTPLATFORMSUBDIR eq 'Linux') {
		$COMPILER_DRIVER = "g++";
		$COMPILER_DRIVER = "g++";
		#$COMPILER_DRIVER = "clang++";
		#$COMPILER_DRIVER = "gcc";
		#$COMPILER_DRIVER = "g++-4.6";
		#$COMPILER_DRIVER = "g++ -V4.5";
		#$COMPILER_DRIVER = "g++ -V4.6";
		$platform = "Platform_Linux";
		$target			=	"Debug";
	}
	if ($PROJECTPLATFORMSUBDIR eq 'VisualStudio.Net-2012') {
		$COMPILER_DRIVER = "CL";
		$platform = "Windows";
	}
	
	SetDefaultForCompilerDriver_();
}




### Do initial pass, just looking for platform
sub	ParseCommandLine_Platform_
{
	for ($i = 0; $i <= $#ARGV; $i++) {
		my $var = $ARGV[$i];
		if ((lc ($var) eq "-platform") or (lc ($var) eq "--platform")) {
			$i++;
			$var = $ARGV[$i];
			$platform = $var;
			SetDefaultForPlatform_ ();
		}
	}
}

### Do initial pass, just looking for platform
sub	ParseCommandLine_Target_
{
	for ($i = 0; $i <= $#ARGV; $i++) {
		my $var = $ARGV[$i];
		if ((lc ($var) eq "-target") or (lc ($var) eq "--target")) {
			$i++;
			$var = $ARGV[$i];
			$target = $var;
		}
	}
}

### Do initial pass, just looking for platform
sub	ParseCommandLine_CompilerDriver_
{
	for ($i = 0; $i <= $#ARGV; $i++) {
		my $var = $ARGV[$i];
		if (lc ($var) eq "-compiler-driver" or lc ($var) eq "--compiler-driver") {
			$i++;
			$var = $ARGV[$i];
			$COMPILER_DRIVER = $var;
		}
	}
}

sub	ParseCommandLine_Remaining_
{
	for ($i = 0; $i <= $#ARGV; $i++) {
		my $var = $ARGV[$i];
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
		if ((lc ($var) eq "-enable-assertions") or (lc ($var) eq "--enable-assertions")) {
			$ENABLE_ASSERTIONS = 1;
		}
		if ((lc ($var) eq "-disable-assertions") or (lc ($var) eq "--disable-assertions")) {
			$ENABLE_ASSERTIONS = 0;
		}
		if ((lc ($var) eq "-default-assertions") or (lc ($var) eq "--default-assertions")) {
			$ENABLE_ASSERTIONS = DEFAULT_BOOL_OPTIONS;
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
		if ((lc ($var) eq "-default-for-platform") or (lc ($var) eq "--default-for-platform")) {
			SetDefaultForPlatform_ ();
		}
		if ((lc ($var) eq "-help") or (lc ($var) eq "--help") or (lc ($var) eq "-?")) {
			DoHelp_ ();
		}
	}
}

sub	CHECK_OPTIONS_
{
	if ($PROJECTPLATFORMSUBDIR eq "") {
		die ("Cannot identify ProjectPlatformSubdir");
	}
}

sub	ParseCommandLine_
{
	SetInitialDefaults_ ();
	
	ParseCommandLine_Platform_ ();
	ParseCommandLine_Target_ ();
	ParseCommandLine_CompilerDriver_ ();
	ParseCommandLine_Remaining_ ();

	SetDefaultForPlatform_ ();
	CHECK_OPTIONS_ ();
}



ParseCommandLine_ ();






# Eventually  -make this more central - put all configs here, and then
# build C++/dirs etc from this!
sub	WriteConfigFile_
{
	open(OUT,">$masterXMLConfigFile");
	print (OUT "<Configuration>\n");
	print (OUT "    <ProjectPlatformSubdir>$PROJECTPLATFORMSUBDIR</ProjectPlatformSubdir>\n");
	print (OUT "    <Platform>$platform</Platform>\n");
	if (not ($target eq "")) {
		print (OUT "    <Target>$target</Target>\n");
	}

	print (OUT "    <CompilerDriver>$COMPILER_DRIVER</CompilerDriver>\n");
	if ($ENABLE_ASSERTIONS != DEFAULT_BOOL_OPTIONS) {
		print (OUT "    <ENABLE_ASSERTIONS>$ENABLE_ASSERTIONS</ENABLE_ASSERTIONS>\n");
	}
	
	print (OUT "    <qHasLibrary_Xerces>$useThirdPartyXerces</qHasLibrary_Xerces>\n");
	print (OUT "    <qHasFeature_libcurl>$ENABLE_LIBCURL</qHasFeature_libcurl>\n");
	print (OUT "    <qHasFeature_WinHTTP>$ENABLE_WINHTTP</qHasFeature_WinHTTP>\n");

	if ($ENABLE_TRACE2FILE != DEFAULT_BOOL_OPTIONS) {
		print (OUT "    <ENABLE_TRACE2FILE>$ENABLE_TRACE2FILE</ENABLE_TRACE2FILE>\n");
	}
	print (OUT "    <IncludeDebugSymbtolsInExecutables>$INCLUDE_SYMBOLS</IncludeDebugSymbtolsInExecutables>\n");
	if (not ($COPTIMIZE_FLAGS eq "")) {
		print (OUT "    <OptimizerFlag>$COPTIMIZE_FLAGS</OptimizerFlag>\n");
	}
	if ($STATIC_LINK_GCCRUNTIME != DEFAULT_BOOL_OPTIONS) {
		print (OUT "    <STATIC_LINK_GCCRUNTIME>$STATIC_LINK_GCCRUNTIME</STATIC_LINK_GCCRUNTIME>\n");
	}


	print (OUT "    <ExtraCDefines>\n");
		foreach $var (@useExtraCDefines)
		{
			print (OUT "       <CDefine>$var</CDefine>\n");
		}
	print (OUT "    </ExtraCDefines>\n");
	
	print (OUT "    <ExtraMakeDefines>\n");
		foreach $var (@$useExtraMakeDefines)
		{
			print (OUT "       <MakeDefine>$var</MakeDefine>\n");
		}
	print (OUT "    </ExtraMakeDefines>\n");
	
	print (OUT "</Configuration>\n");
	close(OUT);
}

#print ("OS: $^O\n");


mkdir ($intermediateFiles);

print("Writing \"$masterXMLConfigFile\"...\n");
WriteConfigFile_ ();

