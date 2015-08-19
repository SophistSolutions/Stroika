#!/usr/bin/perl

#TRY THESE AGAIN, but for now generate too many problems...
#use strict;
#use warnings;

require ("ScriptsLib/StringUtils.pl");

use constant false => 0;
use constant true  => 1;
use constant DEFAULT_BOOL_OPTIONS => -1;

my $configurationFiles	=	"ConfigurationFiles/";
my $platform			=	"";

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
my $CWARNING_FLAGS = '<<USE_DEFAULTS>>';
my $DEFAULT_CWARNING_FLAGS = '-Wall -Wno-switch -Wno-sign-compare -Wno-unused-variable -Wno-unused-but-set-variable  -Wno-unused-value -Wno-strict-aliasing -Wno-unused-local-typedefs -Wno-comment -Wno-unused-function -Wno-unknown-warning-option ';
my $DEFAULT_CWARNING_FLAGS_EXTRA4CLANG46 = 'Wno-future-compat ';

my $FEATUREFLAG_LIBCURL = $LIBFEATUREFLAG_No;		#$LIBFEATUREFLAG_UseStaticTPP; tricky some places because of dependencies - resolve that first
my $FEATUREFLAG_OpenSSL = $LIBFEATUREFLAG_UseStaticTPP;
my $FEATUREFLAG_WinHTTP = $LIBFEATUREFLAG_No;
my $FEATUREFLAG_ATLMFC = $LIBFEATUREFLAG_No;
my $FEATUREFLAG_Xerces = $LIBFEATUREFLAG_UseStaticTPP;
my $FEATUREFLAG_ZLib = $LIBFEATUREFLAG_UseStaticTPP;
my $ENABLE_TRACE2FILE = DEFAULT_BOOL_OPTIONS;
my $INCLUDE_SYMBOLS = 1;
my $COPTIMIZE_FLAGS = "";
my $STATIC_LINK_GCCRUNTIME = DEFAULT_BOOL_OPTIONS;
my $COMPILER_DRIVER = "";
my $AR = undef;
my $RANLIB = undef;
my $EXTRA_COMPILER_ARGS = "";
my $EXTRA_LINKER_ARGS = "";





sub	DoHelp_
{
    print("Usage:\n");
        print("  make default-configuration DEFAULT_CONFIGURATION_ARGS= OPTIONS where options can be:\n");
        print("	    --platform {PLATFORM}                      /* Specifies the directory under Builds/Intermediate Files to create (defaults automatically) - not needed */\n");
        print("	    --assertions { enable|disable|default }    /* Enables/disable assertion feature (setting qDebug) */\n");
        print("	    --GLIBCXX_DEBUG { enable|disable|default } /* Enables/Disables GLIBCXX_DEBUG (G++-specific) */\n");
        print("	    --cppstd-version-flag {FLAG}               /* Sets \$CPPSTD_VERSION_FLAG (empty str means default, but can be --std=c++11, --std=c++14, or --std=c++1z, etc) - UNIX ONLY */\n");
        print("	    --LibCurl {build-only|use|use-system|no}   /* Enables/disables use of LibCurl for this configuration [default TBD]*/\n");
        print("	    --OpenSSL {build-only|use|use-system|no}   /* Enables/disables use of OpenSSL for this configuration [default use] */\n");
        print("	    --WinHTTP {use-system|no}                  /* Enables/disables use of WinHTTP for this configuration [default use-system on windows, and no otherwise] */\n");
        print("	    --ATLMFC {use-system|no}                   /* Enables/disables use of ATLMFC for this configuration [default use-system on windows, and no otherwise] */\n");
        print("	    --Xerces {build-only|use|use-system|no}    /* Enables/disables use of Xerces for this configuration [default use] */\n");
        print("	    --ZLib {build-only|use|use-system|no}      /* Enables/disables use of ZLib for this configuration [default use] */\n");
        print("	    --trace2file { enable|disable|default }    /* Enables/disable trace2file feature */\n");
        print("	    --cpp-optimize-flag  {FLAG}                /* Sets \$COPTIMIZE_FLAGS (empty str means none, -O2 is typical for optimize) - UNIX ONLY */\n");
        print("	    --c-define {ARG}                           /* Define C++ define for the given configuration: arg appears as a line in Stroika-Configuraiton.h */\n");
        print("	    --make-define {ARG}                        /* Define makefile define for the given configuration: text of arg appears as line in Configuration.mk */\n");
        print("	    --compiler-driver {ARG}                    /* default is g++ */\n");
        print("	    --ar {ARG}                                 /* default is undefined, but if compiler-driver is gcc or g++, this is gcc-ar */\n");
        print("	    --ranlib {ARG}                             /* default is undefined, but if compiler-driver is gcc or g++, this is gcc-ranlib */\n");
        print("	    --extra-compiler-args {ARG}                /* Sets variable with extra args for compiler */\n");
        print("	    --extra-linker-args {ARG}                  /* Sets variable with extra args for linker */\n");
        print("	    --pg {ARG}                                 /* Turn on -pg option (profile for UNIX/gcc platform) on linker/compiler */\n");
        print("	    --lto {ARG}                                /* Turn on link time code gen on linker/compiler (for now only gcc/unix stack) */\n");

	exit (0);
}


### Initial defaults before looking at command-line arguments
sub	SetInitialDefaults_
{
	if (("$^O" eq "linux") or ("$^O" eq "darwin") or ("$^O" eq "aix")) {
		$PROJECTPLATFORMSUBDIR = 'Linux';
	}
	if ("$^O" eq "cygwin") {
		# try vs 2k15
		if ($PROJECTPLATFORMSUBDIR eq "") {
			local $PROGRAMFILESDIR= trim (`cygpath \"$ENV{'PROGRAMFILES'}\"`);
			local $PROGRAMFILESDIR2= "/cygdrive/c/Program Files (x86)/";
			if (-e "$PROGRAMFILESDIR/Microsoft Visual Studio 14.0/VC") {
				$PROJECTPLATFORMSUBDIR = 'VisualStudio.Net-2015';
			}
			if (-e "$PROGRAMFILESDIR2/Microsoft Visual Studio 14.0/VC") {
				$PROJECTPLATFORMSUBDIR = 'VisualStudio.Net-2015';
			}
			#autodetect ATLMFC (Express verison missing it)
			if (-e "$PROGRAMFILESDIR2/Microsoft Visual Studio 14.0/VC/atlmfc") {
				$FEATUREFLAG_ATLMFC = $LIBFEATUREFLAG_UseSystem;
			}
			else  {
				$FEATUREFLAG_ATLMFC = $LIBFEATUREFLAG_No;
			}
		}

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
			#autodetect ATLMFC (Express verison missing it)
			if (-e "$PROGRAMFILESDIR2/Microsoft Visual Studio 12.0/VC/atlmfc") {
				$FEATUREFLAG_ATLMFC = $LIBFEATUREFLAG_UseSystem;
			}
			else  {
				$FEATUREFLAG_ATLMFC = $LIBFEATUREFLAG_No;
			}
		}

	}

	if (("$^O" eq "linux") or ("$^O" eq "darwin") or ("$^O" eq "aix")) {
		$FEATUREFLAG_LIBCURL = $LIBFEATUREFLAG_UseStaticTPP;
	}
	if ("$^O" eq "cygwin") {
		$FEATUREFLAG_WinHTTP = $LIBFEATUREFLAG_UseSystem;
	}
	#if ("$^O" eq "cygwin") {
	#	$FEATUREFLAG_ATLMFC = $LIBFEATUREFLAG_UseSystem;
	#}
}


sub	SetDefaultForCompilerDriver_
{
	if ($PROJECTPLATFORMSUBDIR eq 'Linux') {
		$STATIC_LINK_GCCRUNTIME = 1;
	}
	if ($CWARNING_FLAGS eq "<<USE_DEFAULTS>>") {
		#todo fix so we check compiler and adjust
		$CWARNING_FLAGS = $DEFAULT_CWARNING_FLAGS;
		if ($COMPILER_DRIVER eq "clang++-4.6") {
			$CWARNING_FLAGS = $DEFAULT_CWARNING_FLAGS + $DEFAULT_CWARNING_FLAGS_EXTRA4CLANG46;
		}
	}
	if (!(defined $AR) and (($COMPILER_DRIVER eq "g++") || ($COMPILER_DRIVER eq "gcc") || ($COMPILER_DRIVER eq "g++-4.9"))) {
		$AR = "gcc-ar";
	}
	if (!(defined $RANLIB) and (($COMPILER_DRIVER eq "g++") || ($COMPILER_DRIVER eq "gcc") || ($COMPILER_DRIVER eq "g++-4.9"))) {
		$RANLIB = "gcc-ranlib";
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
	}
	#if ($PROJECTPLATFORMSUBDIR eq 'VisualStudio.Net-2012') {
	#	$COMPILER_DRIVER = "CL";
	#	$platform = "Windows";
	#}
	
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
		elsif (lc ($var) eq "-make-define" or lc ($var) eq "--make-define") {
			$i++;
			$var = $ARGV[$i];
			$useExtraMakeDefines[@useExtraMakeDefines] = $var;
		}
		elsif ((lc ($var) eq "-assertions") or (lc ($var) eq "--assertions")) {
			$i++;
			$var = $ARGV[$i];
			if ($var eq "enable") {
				$ENABLE_ASSERTIONS = 1;
			}
			elsif ($var eq "disable") {
				$ENABLE_ASSERTIONS = 0;
			}
			elsif ($var eq "default") {
				$ENABLE_ASSERTIONS = DEFAULT_BOOL_OPTIONS;
			}
			else  {
                print ("UNRECOGNIZED assertions ARG: $var\n");
                DoHelp_ ();
			}
		}
		elsif ((lc ($var) eq "-glibcxx_debug") or (lc ($var) eq "--glibcxx_debug")) {
			$i++;
			$var = $ARGV[$i];
			if ($var eq "enable") {
				$ENABLE_GLIBCXX_DEBUG = 1;
			}
			elsif ($var eq "disable") {
				$ENABLE_GLIBCXX_DEBUG = 0;
			}
			elsif ($var eq "default") {
				$ENABLE_GLIBCXX_DEBUG = DEFAULT_BOOL_OPTIONS;
			}
			else  {
                print ("UNRECOGNIZED$ENABLE_GLIBCXX_DEBUG ARG: $var\n");
                DoHelp_ ();
			}
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
        elsif ((lc ($var) eq "-openssl") or (lc ($var) eq "--openssl")) {
            $i++;
            $var = $ARGV[$i];
            $FEATUREFLAG_OpenSSL = $var;
        }
        elsif ((lc ($var) eq "-winhttp") or (lc ($var) eq "--winhttp")) {
            $i++;
            $var = $ARGV[$i];
            $FEATUREFLAG_WinHTTP = $var;
        }
        elsif ((lc ($var) eq "-atlmfc") or (lc ($var) eq "--atlmfc")) {
            $i++;
            $var = $ARGV[$i];
            $FEATUREFLAG_ATLMFC = $var;
        }
        elsif ((lc ($var) eq "-xerces") or (lc ($var) eq "--xerces")) {
            $i++;
            $var = $ARGV[$i];
            $FEATUREFLAG_Xerces = $var;
        }
        elsif ((lc ($var) eq "-zlib") or (lc ($var) eq "--zlib")) {
            $i++;
            $var = $ARGV[$i];
            $FEATUREFLAG_ZLib = $var;
        }
		elsif ((lc ($var) eq "-trace2file") or (lc ($var) eq "--trace2file")) {
			$i++;
			$var = $ARGV[$i];
			if ($var eq "enable") {
				$ENABLE_TRACE2FILE = 1;
			}
			elsif ($var eq "disable") {
				$ENABLE_TRACE2FILE = 0;
			}
			elsif ($var eq "default") {
				$ENABLE_TRACE2FILE = DEFAULT_BOOL_OPTIONS;
			}
			else  {
                print ("UNRECOGNIZED ENABLE_TRACE2FILEG ARG: $var\n");
                DoHelp_ ();
			}
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
		elsif ((lc ($var) eq "-ar") or (lc ($var) eq "--ar")) {
			$i++;
			$var = $ARGV[$i];
			$AR = $var;
		}
		elsif ((lc ($var) eq "-ranlib") or (lc ($var) eq "--ranlib")) {
			$i++;
			$var = $ARGV[$i];
			$RANLIB = $var;
		}

		elsif ((lc ($var) eq "-extra-compiler-args") or (lc ($var) eq "--extra-compiler-args")) {
			$i++;
			$var = $ARGV[$i];
			$EXTRA_COMPILER_ARGS = $var;
		}
		elsif ((lc ($var) eq "-extra-linker-args") or (lc ($var) eq "--extra-linker-args")) {
			$i++;
			$var = $ARGV[$i];
			$EXTRA_LINKER_ARGS = $var;
		}
		elsif ((lc ($var) eq "-lto") or (lc ($var) eq "--lto")) {
			$EXTRA_COMPILER_ARGS .= " -flto";
			$EXTRA_LINKER_ARGS .= " -flto";
		}
		elsif ((lc ($var) eq "-pg") or (lc ($var) eq "--pg")) {
			$EXTRA_COMPILER_ARGS .= " -pg";
			$EXTRA_LINKER_ARGS .= " -pg";
		}
		elsif ((lc ($var) eq "-help") or (lc ($var) eq "--help") or (lc ($var) eq "-?")) {
			DoHelp_ ();
		}
        else {
            if ((lc ($var) eq "-platform") or (lc ($var) eq "--platform")) {
                $i++;
            }
            elsif (lc ($var) eq "-compiler-driver" or lc ($var) eq "--compiler-driver") {
                $i++;
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
	if ($PROJECTPLATFORMSUBDIR eq "VisualStudio.Net-2012") {
		die ("WE NO LONGER SUPPORT VISUAL STUDIO.Net 2012\n");
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
	if (not ($CWARNING_FLAGS eq "")) {
		print (OUT "    <CWARNING_FLAGS>$CWARNING_FLAGS</CWARNING_FLAGS>\n");
	}
	
	
	print (OUT "    <qFeatureFlag_LibCurl>$FEATUREFLAG_LIBCURL</qFeatureFlag_LibCurl>\n");
	print (OUT "    <qFeatureFlag_OpenSSL>$FEATUREFLAG_OpenSSL</qFeatureFlag_OpenSSL>\n");
	print (OUT "    <qFeatureFlag_WinHTTP>$FEATUREFLAG_WinHTTP</qFeatureFlag_WinHTTP>\n");
	print (OUT "    <qFeatureFlag_ATLMFC>$FEATUREFLAG_ATLMFC</qFeatureFlag_ATLMFC>\n");
	print (OUT "    <qFeatureFlag_Xerces>$FEATUREFLAG_Xerces</qFeatureFlag_Xerces>\n");
	print (OUT "    <qFeatureFlag_ZLib>$FEATUREFLAG_ZLib</qFeatureFlag_ZLib>\n");

	if (defined $AR) {
		print (OUT "    <AR>$AR</AR>\n");
	}
	if (defined $RANLIB) {
		print (OUT "    <RANLIB>$RANLIB</RANLIB>\n");
	}

	print (OUT "    <EXTRA_COMPILER_ARGS>$EXTRA_COMPILER_ARGS</EXTRA_COMPILER_ARGS>\n");
	print (OUT "    <EXTRA_LINKER_ARGS>$EXTRA_LINKER_ARGS</EXTRA_LINKER_ARGS>\n");


	if ($ENABLE_TRACE2FILE != DEFAULT_BOOL_OPTIONS) {
		print (OUT "    <ENABLE_TRACE2FILE>$ENABLE_TRACE2FILE</ENABLE_TRACE2FILE>\n");
	}
	print (OUT "    <IncludeDebugSymbolsInExecutables>$INCLUDE_SYMBOLS</IncludeDebugSymbolsInExecutables>\n");
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
	print("   Writing \"$masterXMLConfigFile\"...\n");
	WriteConfigFile_ ();
	system ("rm -f IntermediateFiles/APPLIED_CONFIGURATIONS");
}

