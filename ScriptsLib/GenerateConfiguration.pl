#!/usr/bin/perl

#TRY THESE AGAIN, but for now generate too many problems...
#use strict;
#use warnings;

require ("ScriptsLib/StringUtils.pl");

############### NOTE - STILL NOT READY TO BE USED - DRAFT...

use constant false => 0;
use constant true  => 1;
use constant DEFAULT_BOOL_OPTIONS => -1;

my $configurationFiles	=	"ConfigurationFiles/";
my $platform			=	"";
my $target			=	"";

my $masterXMLConfigFile	=	"$configurationFiles/DefaultConfiguration.xml";

my $forceWriteConfig	=	true;





########### CONFIG_Variables ###############
my $PROJECTPLATFORMSUBDIR='';

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


my $ENABLE_ASSERTIONS = DEFAULT_BOOL_OPTIONS;
my $ENABLE_GLIBCXX_DEBUG = DEFAULT_BOOL_OPTIONS;
my $CPPSTD_VERSION_FLAG = '';
my $FEATUREFLAG_LIBCURL = $LIBFEATUREFLAG_No;		#$LIBFEATUREFLAG_UseStaticTPP; tricky some places because of dependencies - resolve that first
my $FEATUREFLAG_XERCES = $LIBFEATUREFLAG_UseStaticTPP;
my $FEATUREFLAG_OpenSSL = $LIBFEATUREFLAG_UseStaticTPP;
my $ENABLE_ZLIB = 1;
my $ENABLE_WINHTTP = 0;
my $ENABLE_TRACE2FILE = DEFAULT_BOOL_OPTIONS;
my $INCLUDE_SYMBOLS = 1;
my $COPTIMIZE_FLAGS = "";
my $STATIC_LINK_GCCRUNTIME = DEFAULT_BOOL_OPTIONS;
my $COMPILER_DRIVER = "";





sub	DoHelp_
{
    print("Usage:\n");
        print("  make default-configuration DEFAULT_CONFIGURATION_ARGS= OPTIONS where options can be:\n");
        print("	    --only-if-unconfigured                     /* Opposite of --force - only rebuilds the configfiles if absent */\n");
        print("	    --default-for-platform                     /* May create multiple targets (recursive call to configure) - but generates all the default");
        print("                                                                       settings for this platform */\n");
        print("	    --platform {PLATFORM}                      /* specifies the directory under Builds/Intermediate Files to create */\n");
        print("	    --target {TARGET}                          /* specifies the directory under Platform to create (no other semantics - just a name) */\n");
        print("	    --enable-assertions                        /* enables assertions for the configuration being configured */\n");
        print("	    --disable-assertions                       /* disables assertions for the configuration being configured */\n");
        print("	    --default-assertions                       /* default assertions (based on NDEBUG flag) for the configuration being configured - so */\n");
        print("	    --enable-GLIBCXX_DEBUG                     /* enables GLIBCXX_DEBUG (G++-specific) */\n");
        print("	    --disable-GLIBCXX_DEBUG                    /* disables GLIBCXX_DEBUG (G++-specific) */\n");
        print("	    --default-GLIBCXX_DEBUG                    /* default GLIBCXX_DEBUG (based on enable-assertions flag and platform) for the configuration being");
        print("                                                                configured - so */\n");
        print("	    --cppstd-version-flag {FLAG}               /* Sets $CPPSTD_VERSION_FLAG (empty str means default, but can be --std=c++11, or --std=c++1y, etc)");
        print("                                                                                    - UNIX ONLY */\n");
        print("	    --LibCurl {build-only|use|use-system|no}   /* enables/disables use of LibCurl and build for the confguration being defined [default TBD]*/\n");
        print("	    --Xerces {build-only|use|use-system|no}    /* enables/disables use of Xerces and build for the confguration being defined [default use] */\n");
        print("	    --OpenSSL {build-only|use|use-system|no}   /* enables/disables use of OpenSSL and build for the confguration being defined [default use] */\n");
        print("	    --has-xerces                               /* DEPRECATED-2015-04-02 --xerces use */\n");
        print("	    --no-has-xerces                            /* DEPRECATED-2015-04-02 --xerces no */\n");
        print("	    --has-openssl                              /* DEPRECATED-2015-04-02 --openssl use */\n");
        print("	    --no-has-openssl                           /* DEPRECATED-2015-04-02 --openssl no */\n");
        print("	    --has-winhttp                              /* enables winhttp for the configuration being configured */\n");
        print("	    --no-has-winhttp                           /* disables winhttp for the configuration being configured */\n");
        print("	    --has-zlib                                 /* enables zlib for the configuration being configured */\n");
        print("	    --no-has-zlib                              /* disables zlib for the configuration being configured */\n");
        print("	    --enable-trace2file                        /* enables trace2file for the configuration being configured */\n");
        print("	    --disable-trace2file                       /* disables trace2file for the configuration being configured */\n");
        print("	    --cpp-optimize-flag  {FLAG}                /* Sets $$COPTIMIZE_FLAGS (empty str means none, -O2 is typical for optimize) - UNIX ONLY */\n");
        print("	    --c-define {ARG}                           /* Define C++ / CPP define for the given target */\n");
        print("	    --make-define {ARG}                        /* Define makefile define for the given target */\n");
        print("	    --compiler-driver {ARG}                    /* default is g++ */\n");

	exit (0);
}


### Initial defaults before looking at command-line arguments
sub	SetInitialDefaults_
{
	if (("$^O" eq "linux") or ("$^O" eq "darwin")) {
		$PROJECTPLATFORMSUBDIR = 'Linux';
	}
	if ("$^O" eq "cygwin") {
		# try vs 2k13
		if ($PROJECTPLATFORMSUBDIR eq "") {
			local $PROGRAMFILESDIR= trim (`cygpath \"$ENV{'PROGRAMFILES'}\"`);
			local $PROGRAMFILESDIR2= "/cygdrive/c/Program Files (x86)/";
			if (-e "$PROGRAMFILESDIR/Microsoft Visual Studio 12.0/VC") {
				$PROJECTPLATFORMSUBDIR = 'VisualStudio.Net-2013';
			}
			if (-e "$PROGRAMFILESDIR2/Microsoft Visual Studio 12.0/VC") {
				$PROJECTPLATFORMSUBDIR = 'VisualStudio.Net-2013';
			}
		}

		# try vs 2k12
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

		# try vs 2k10 (NOTE - OBSOLETE)
		if ($PROJECTPLATFORMSUBDIR eq "") {
			local $PROGRAMFILESDIR= trim (`cygpath \"$ENV{'PROGRAMFILES'}\"`);
			local $PROGRAMFILESDIR2= "/cygdrive/c/Program Files (x86)/";
			if (-e "$PROGRAMFILESDIR/Microsoft Visual Studio 10.0/VC") {
				$PROJECTPLATFORMSUBDIR = 'VisualStudio.Net-2010';
			}
			if (-e "$PROGRAMFILESDIR2/Microsoft Visual Studio 10.0/VC") {
				$PROJECTPLATFORMSUBDIR = 'VisualStudio.Net-2010';
			}
			if ("$PROJECTPLATFORMSUBDIR" eq "VisualStudio.Net-2010") {
				print ("****ERRROR - WE NO LONGER SUPPORT VISUAL STUDIO.Net 2010\n");
				exit (1);
			}
		}
	}

	if (("$^O" eq "linux") or ("$^O" eq "darwin")) {
		$FEATUREFLAG_LIBCURL = $LIBFEATUREFLAG_UseStaticTPP;
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
		elsif ((lc ($var) eq "-default-for-platform") or (lc ($var) eq "--default-for-platform")) {
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
		if ((lc ($var) eq "-only-if-unconfigured") or (lc ($var) eq "--only-if-unconfigured")) {
			$forceWriteConfig = false;
		}
		elsif (lc ($var) eq "-c-define" or lc ($var) eq "--c-define") {
			$i++;
			$var = $ARGV[$i];
			$useExtraCDefines[@useExtraCDefines] = $var;
		}
		elsif (lc ($var) eq "-make-define" or lc ($var) eq "--make-define") {
			$i++;
			$var = $ARGV[$i];
			$useExtraMakeDefines[@useExtraMakeDefines] = $var;
		}
		elsif ((lc ($var) eq "-enable-assertions") or (lc ($var) eq "--enable-assertions")) {
			$ENABLE_ASSERTIONS = 1;
		}
		elsif ((lc ($var) eq "-disable-assertions") or (lc ($var) eq "--disable-assertions")) {
			$ENABLE_ASSERTIONS = 0;
		}
		elsif ((lc ($var) eq "-default-assertions") or (lc ($var) eq "--default-assertions")) {
			$ENABLE_ASSERTIONS = DEFAULT_BOOL_OPTIONS;
		}
		elsif ((lc ($var) eq "-enable-glibcxx_debug") or (lc ($var) eq "--enable-glibcxx_debug")) {
			$ENABLE_GLIBCXX_DEBUG = 1;
		}
		elsif ((lc ($var) eq "-disable-glibcxx_debug") or (lc ($var) eq "--disable-glibcxx_debug")) {
			$ENABLE_GLIBCXX_DEBUG = 0;
		}
		elsif ((lc ($var) eq "-default-glibcxx_debug") or (lc ($var) eq "--default-glibcxx_debug")) {
			$ENABLE_GLIBCXX_DEBUG = DEFAULT_BOOL_OPTIONS;
		}
		elsif ((lc ($var) eq "-cppstd-version-flag") or (lc ($var) eq "--cppstd-version-flag")) {
			$i++;
			$var = $ARGV[$i];
			$CPPSTD_VERSION_FLAG = $var;
		}
		elsif ((lc ($var) eq "-has-openssl") or (lc ($var) eq "--has-openssl")) {
			$FEATUREFLAG_OpenSSL = $LIBFEATUREFLAG_UseStaticTPP;
			print ("$var flag DEPRECATED - use --openssl\n");
		}
		elsif ((lc ($var) eq "-no-has-openssl") or (lc ($var) eq "--no-has-openssl")) {
			$FEATUREFLAG_OpenSSL = $LIBFEATUREFLAG_No;
			print ("$var flag DEPRECATED - use --openssl\n");
		}
        elsif ((lc ($var) eq "-libcurl") or (lc ($var) eq "--libcurl")) {
            $i++;
            $var = $ARGV[$i];
            $FEATUREFLAG_LIBCURL = $var;
        }
		elsif ((lc ($var) eq "-has-winhttp") or (lc ($var) eq "--has-winhttp")) {
			$ENABLE_WINHTTP = 1;
		}
		elsif ((lc ($var) eq "-no-has-winhttp") or (lc ($var) eq "--no-has-winhttp")) {
			$ENABLE_WINHTTP = 0;
		}
		elsif ((lc ($var) eq "-has-xerces") or (lc ($var) eq "--has-xerces")) {
			$FEATUREFLAG_XERCES = $LIBFEATUREFLAG_UseStaticTPP;
			print ("$var flag DEPRECATED - use --xerces\n");
		}
		elsif ((lc ($var) eq "-no-has-xerces") or (lc ($var) eq "--no-has-xerces")) {
			$FEATUREFLAG_XERCES = $LIBFEATUREFLAG_No;
			print ("$var flag DEPRECATED - use --xerces\n");
		}
		elsif ((lc ($var) eq "-has-zlib") or (lc ($var) eq "--has-zlib")) {
			$ENABLE_ZLIB = 1;
		}
		elsif ((lc ($var) eq "-no-has-zlib") or (lc ($var) eq "--no-has-zlib")) {
			$ENABLE_ZLIB = 0;
		}
		elsif ((lc ($var) eq "-enable-trace2file") or (lc ($var) eq "--enable-trace2file")) {
			$ENABLE_TRACE2FILE = 1;
		}
		elsif ((lc ($var) eq "-disable-trace2file") or (lc ($var) eq "--disable-trace2file")) {
			$ENABLE_TRACE2FILE = 0;
		}
		elsif ((lc ($var) eq "-enable-static-link-gccruntime") or (lc ($var) eq "--enable-static-link-gccruntime")) {
			$STATIC_LINK_GCCRUNTIME = 1;
		}
		elsif ((lc ($var) eq "-disable-static-link-gccruntime") or (lc ($var) eq "--disable-static-link-gccruntime")) {
			$STATIC_LINK_GCCRUNTIME = 0;
		}
		elsif ((lc ($var) eq "-cpp-optimize-flag") or (lc ($var) eq "--cpp-optimize-flag")) {
			$i++;
			$var = $ARGV[$i];
			$COPTIMIZE_FLAGS = $var;
		}
		elsif ((lc ($var) eq "-help") or (lc ($var) eq "--help") or (lc ($var) eq "-?")) {
			DoHelp_ ();
		}
                else {
                    if ((lc ($var) eq "-platform") or (lc ($var) eq "--platform")) {
                        $i++;
                    }
                    elsif ((lc ($var) eq "-default-for-platform") or (lc ($var) eq "--default-for-platform")) {
                    }
                    elsif ((lc ($var) eq "-target") or (lc ($var) eq "--target")) {
                        $i++;
                    }
                    elsif (lc ($var) eq "-compiler-driver" or lc ($var) eq "--compiler-driver") {
                        $i++;
                    }
                    elsif (lc ($var) eq "-has-libcurl" or lc ($var) eq "--has-libcurl") {
                        print ("UNRECOGNIZED ARG: $var: use --libcurl use\n");
                        DoHelp_ ();
                    }
                    elsif (lc ($var) eq "-no-has-libcurl" or lc ($var) eq "--no-has-libcurl") {
                        print ("UNRECOGNIZED ARG: $var: use --libcurl no\n");
                        DoHelp_ ();
                    }
                    else {
                        print ("UNRECOGNIZED ARG: $var\n");
                        DoHelp_ ();
                    }
                }
	}
}


sub     CHECK_FEATURE_OPTION
{
    my $x = shift(@_);
    if (!($x eq $LIBFEATUREFLAG_BuildOnly) && !($x eq $LIBFEATUREFLAG_UseStaticTPP) && !($x eq $LIBFEATUREFLAG_UseSystem) && !($x eq $LIBFEATUREFLAG_No)) {
            die ("Cannot identify ProjectPlatformSubdir\n");
    }
}

sub	CHECK_OPTIONS_
{
	if ($PROJECTPLATFORMSUBDIR eq "") {
		die ("Cannot identify ProjectPlatformSubdir\n");
	}
	if ($PROJECTPLATFORMSUBDIR eq "VisualStudio.Net-2010") {
		die ("WE NO LONGER SUPPORT VISUAL STUDIO.Net 2010\n");
	}
  	CHECK_FEATURE_OPTION($FEATUREFLAG_LIBCURL);
}

sub	ParseCommandLine_
{
	if (false) {
		# Helpful to debug scripts...
		print "Entering GenerateConfiguration.pl (";
		for ($i = 0; $i <= $#ARGV; $i++) {
			my $var = $ARGV[$i];
			print ($var);
			print (" ");
		}
		print (")\n");
	}

	SetInitialDefaults_ ();
	
	ParseCommandLine_Platform_ ();
	SetDefaultForPlatform_ ();

	ParseCommandLine_Target_ ();
	
	ParseCommandLine_CompilerDriver_ ();
	SetDefaultForCompilerDriver_();

	ParseCommandLine_Remaining_ ();

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
	if ($ENABLE_GLIBCXX_DEBUG != DEFAULT_BOOL_OPTIONS) {
		print (OUT "    <ENABLE_GLIBCXX_DEBUG>$ENABLE_GLIBCXX_DEBUG</ENABLE_GLIBCXX_DEBUG>\n");
	}

	if (not ($CPPSTD_VERSION_FLAG eq "")) {
		print (OUT "    <CPPSTD_VERSION_FLAG>$CPPSTD_VERSION_FLAG</CPPSTD_VERSION_FLAG>\n");
	}
	
	print (OUT "    <qFeatureFlag_libcurl>$FEATUREFLAG_LIBCURL</qFeatureFlag_libcurl>\n");
	print (OUT "    <qFeatureFlag_OpenSSL>$FEATUREFLAG_OpenSSL</qFeatureFlag_OpenSSL>\n");
	print (OUT "    <qFeatureFlag_Xerces>$FEATUREFLAG_XERCES</qFeatureFlag_Xerces>\n");
	print (OUT "    <qHasFeature_zlib>$ENABLE_ZLIB</qHasFeature_zlib>\n");

	#print (OUT "    <qHasFeature_openssl>$ENABLE_OPENSSL</qHasFeature_openssl>\n");
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
		foreach $var (@useExtraMakeDefines)
		{
			print (OUT "       <MakeDefine>$var</MakeDefine>\n");
		}
	print (OUT "    </ExtraMakeDefines>\n");
	
	print (OUT "</Configuration>\n");
	close(OUT);
}

#print ("OS: $^O\n");

mkdir ($configurationFiles);

#if ($forceWriteConfig) {
#	#print("Forcing recreate of \"$masterXMLConfigFile\"...\n");
#}

if (not (-e $masterXMLConfigFile) or $forceWriteConfig) {
	print("Writing \"$masterXMLConfigFile\"...\n");
	WriteConfigFile_ ();
	system ("rm -f IntermediateFiles/APPLIED_CONFIGURATIONS");
	system ("rm -f Library/Sources/Stroika/Foundation/Configuration/StroikaConfig.h");
}

