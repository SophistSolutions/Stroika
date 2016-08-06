#!/usr/bin/perl

#TRY THESE AGAIN, but for now generate too many problems...
#use strict;
#use warnings;

require ("ScriptsLib/StringUtils.pl");

use constant false => 0;
use constant true  => 1;
use constant DEFAULT_BOOL_OPTIONS => -1;

my $configurationFiles	=	"ConfigurationFiles/";

my $configurationName	=	undef;

my $MAKE_INDENT_LEVEL = $ENV{'MAKE_INDENT_LEVEL'};
if (!defined $MAKE_INDENT_LEVEL) {
	$MAKE_INDENT_LEVEL = 0;
}




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


my $ENABLE_LTO = DEFAULT_BOOL_OPTIONS;
my $ENABLE_ASSERTIONS = DEFAULT_BOOL_OPTIONS;
my $ENABLE_GLIBCXX_DEBUG = DEFAULT_BOOL_OPTIONS;
my $CPPSTD_VERSION_FLAG = '';
my $CWARNING_FLAGS = '<<USE_DEFAULTS>>';

my $runtimeStackProtectorFlag = DEFAULT_BOOL_OPTIONS;
my $sanitizerFlags = "";
my $noSanitizerFlags = "";

my $ApplyDebugFlags = DEFAULT_BOOL_OPTIONS;
my $ApplyReleaseFlags = DEFAULT_BOOL_OPTIONS;

my $DEFAULT_CWARNING_FLAGS_SAFE_COMMON_ = '-Wall ';
my $DEFAULT_CWARNING_FLAGS_SAFE_COMMON_ = $DEFAULT_CWARNING_FLAGS_SAFE_COMMON_ . '-Wno-switch ';
my $DEFAULT_CWARNING_FLAGS_SAFE_COMMON_ = $DEFAULT_CWARNING_FLAGS_SAFE_COMMON_ . '-Wno-sign-compare ';
my $DEFAULT_CWARNING_FLAGS_SAFE_COMMON_ = $DEFAULT_CWARNING_FLAGS_SAFE_COMMON_ . '-Wno-unused-variable ';
my $DEFAULT_CWARNING_FLAGS_SAFE_COMMON_ = $DEFAULT_CWARNING_FLAGS_SAFE_COMMON_ . '-Wno-unused-value ';
my $DEFAULT_CWARNING_FLAGS_SAFE_COMMON_ = $DEFAULT_CWARNING_FLAGS_SAFE_COMMON_ . '-Wno-strict-aliasing ';
my $DEFAULT_CWARNING_FLAGS_SAFE_COMMON_ = $DEFAULT_CWARNING_FLAGS_SAFE_COMMON_ . '-Wno-comment ';
my $DEFAULT_CWARNING_FLAGS_SAFE_COMMON_ = $DEFAULT_CWARNING_FLAGS_SAFE_COMMON_ . '-Wno-unused-function ';

my $DEFAULT_CWARNING_FLAGS_GCC		= $DEFAULT_CWARNING_FLAGS_SAFE_COMMON_;
my $DEFAULT_CWARNING_FLAGS_GCC		= $DEFAULT_CWARNING_FLAGS_GCC . '-Wno-unused-but-set-variable ';
my $DEFAULT_CWARNING_FLAGS_GCC		= $DEFAULT_CWARNING_FLAGS_GCC . '-Wno-unused-local-typedefs ';

my $DEFAULT_CWARNING_FLAGS_GCC_AIX	= '-Wno-cpp ';

my $DEFAULT_CWARNING_FLAGS_CLANG	= $DEFAULT_CWARNING_FLAGS_SAFE_COMMON_;
#my $DEFAULT_CWARNING_FLAGS_CLANG	= $DEFAULT_CWARNING_FLAGS_CLANG . '-Wno-unknown-warning-option ';
my $DEFAULT_CWARNING_FLAGS_CLANG	= $DEFAULT_CWARNING_FLAGS_CLANG . '-Wno-unused-const-variable ';

my $DEFAULT_CWARNING_FLAGS_CLANG36Plus	= '';
my $DEFAULT_CWARNING_FLAGS_CLANG36Plus	= $DEFAULT_CWARNING_FLAGS_CLANG36Plus . '-Wno-unused-local-typedef ';

my $DEFAULT_CWARNING_FLAGS_CLANG36EXTRA	= '-Wno-future-compat ';

my $FEATUREFLAG_LIBCURL = $LIBFEATUREFLAG_No;		#$LIBFEATUREFLAG_UseStaticTPP; tricky some places because of dependencies - resolve that first
my $FEATUREFLAG_OpenSSL = "";
my $FEATUREFLAG_OpenSSLExtraArgs = "";
my $FEATUREFLAG_WinHTTP = $LIBFEATUREFLAG_No;
my $FEATUREFLAG_ATLMFC = $LIBFEATUREFLAG_No;
my $FEATUREFLAG_Xerces = $LIBFEATUREFLAG_UseStaticTPP;
my $FEATUREFLAG_ZLib = $LIBFEATUREFLAG_UseStaticTPP;
my $FEATUREFLAG_sqlite = $LIBFEATUREFLAG_UseStaticTPP;
my $FEATUREFLAG_LZMA = $LIBFEATUREFLAG_UseStaticTPP;
my $ENABLE_TRACE2FILE = DEFAULT_BOOL_OPTIONS;
my $INCLUDE_SYMBOLS_LIB = true;
my $INCLUDE_SYMBOLS_EXE = DEFAULT_BOOL_OPTIONS;
my $MALLOC_GUARD = DEFAULT_BOOL_OPTIONS;
my $COPTIMIZE_FLAGS = "";
my $STATIC_LINK_GCCRUNTIME = DEFAULT_BOOL_OPTIONS;
my $COMPILER_DRIVER = "";
my $COMPILER_DRIVER_C = "";				# @todo allow cmdline option to set _C or _CPlusPlus version
my $COMPILER_DRIVER_CPlusPlus = "";
my $AR = undef;
my $RANLIB = undef;
my $EXTRA_COMPILER_ARGS = "";
my $EXTRA_LINKER_ARGS = "";
my $CrossCompiling = "false";
my $onlyGenerateIfCompilerExists = false;




sub	DoHelp_
{
    my $x = shift(@_);
    print("Usage:\n");
        print("  configure CONFIGURATION-NAME [OPTIONS]* where options can be:\n");
        print("	    --platform {PLATFORM}                           /* Specifies the ProjectPlatformSubdir (Unix, VisualStudio.Net-2015) - usually auto-detected */\n");
        print("	    --assertions { enable|disable|default }         /* Enables/disable assertion feature (setting qDebug) */\n");
        print("	    --block-allocation { enable|disable|default }   /* Enables/disable block-allocation (a feature that improves performance, but messes up valgrind) */\n");
        print("	    --valgrind { enable|disable|default }           /* Enables/disable valgrind-specific runtime code (so far only needed for clean helgrind use) */\n");
        print("	    --GLIBCXX_DEBUG { enable|disable|default }      /* Enables/Disables GLIBCXX_DEBUG (G++-specific) */\n");
        print("	    --cppstd-version-flag {FLAG}                    /* Sets \$CPPSTD_VERSION_FLAG (empty str means default, but can be --std=c++11, --std=c++14, or --std=c++1z, etc) - UNIX ONLY */\n");
        print("	    --LibCurl {build-only|use|use-system|no}        /* Enables/disables use of LibCurl for this configuration [default TBD]*/\n");
        print("	    --OpenSSL {build-only|use|use-system|no}        /* Enables/disables use of OpenSSL for this configuration [default use] */\n");
        print("	    --OpenSSL-ExtraArgs { purify? }                 /* Optionally configure extra OpenSSL features (see Stroika/OpenSSL makefile) */\n");
        print("	    --WinHTTP {use-system|no}                       /* Enables/disables use of WinHTTP for this configuration [default use-system on windows, and no otherwise] */\n");
        print("	    --ATLMFC {use-system|no}                        /* Enables/disables use of ATLMFC for this configuration [default use-system on windows, and no otherwise] */\n");
        print("	    --Xerces {build-only|use|use-system|no}         /* Enables/disables use of Xerces for this configuration [default use] */\n");
        print("	    --sqlite {build-only|use|use-system|no}         /* Enables/disables use of sqlite for this configuration [default use] */\n");
        print("	    --ZLib {build-only|use|use-system|no}           /* Enables/disables use of ZLib for this configuration [default use] */\n");
        print("	    --lzma {build-only|use|use-system|no}           /* Enables/disables use of LZMA SDK for this configuration [default use] */\n");
        print("	    --trace2file { enable|disable|default }         /* Enables/disable trace2file feature */\n");
        print("	    --static-link-gccruntime { enable|disable }     /* Enables/disable gcc runtime static link (only applies if gcc family compiler) */\n");
        print("	    --cpp-optimize-flag  {FLAG}                     /* Sets \$COPTIMIZE_FLAGS (empty str means none, -O2 is typical for optimize) - UNIX ONLY */\n");
        print("	    --c-define {ARG}                                /* Define C++ define for the given configuration: arg appears as a line in Stroika-Configuraiton.h */\n");
        print("	    --make-define {ARG}                             /* Define makefile define for the given configuration: text of arg appears as line in Configuration.mk */\n");
        print("	    --compiler-driver {ARG}                         /* default is gcc */\n");
        print("	    --ar {ARG}                                      /* default is undefined, but if compiler-driver is gcc or g++, this is gcc-ar */\n");
        print("	    --ranlib {ARG}                                  /* default is undefined, but if compiler-driver is gcc or g++, this is gcc-ranlib */\n");
        print("	    --extra-compiler-args {ARG}                     /* Sets variable with extra args for compiler */\n");
        print("	    --append-extra-compiler-args {ARG}              /* Appends ARG to 'extra compiler */\n");
        print("	    --extra-linker-args {ARG}                       /* Sets variable with extra args for linker */\n");
        print("	    --append-extra-linker-args {ARG}                /* Appends ARG to 'extra linker */\n");
        print("	    --append-extra-compiler-and-linker-args {ARG}   /* Appends ARG to 'extra compiler' and 'extra linker' args */\n");
        print("	    --pg {ARG}                                      /* Turn on -pg option (profile for UNIX/gcc platform) on linker/compiler */\n");
        print("	    --lto { enable|disable }                        /* Turn on link time code gen on linker/compiler (for now only gcc/unix stack) */\n");
        print("	    --cross-compiling {true|false}                  /* Defaults generally to false, but set explicitly to control if certain tests will be run */\n");
        print("	    --apply-default-debug-flags                     /*  */\n");
        print("	    --apply-default-release-flags                   /*  */\n");
        print("	    --only-if-has-compiler                          /* Only generate this configuration if the compiler appears to exist (test run)*/\n");
        print("	    --debug-symbols {true|false}                    /* defaults to true, but can be disabled if makes compile/link/etc too big/slow */\n");
        print("	    --debug-symbols-lib {true|false}                /* defaults to true, but can be disabled if makes compile/link/etc too big/slow */\n");
        print("	    --debug-symbols-exe {true|false}                /* defaults to true, but can be disabled if makes compile/link/etc too big/slow */\n");
        print("	    --malloc-guard {true|false}                     /* defaults to false (for now experimental and only works with GCC) */\n");
        print("	    --runtime-stack-check {true|false}              /* gcc -fstack-protector-all */\n");
        print("	    --sanitize {none|thread|address|undefined|leak} /* if arg none, reset to none, else adds arg to sanitized feature (gcc/clang only) - any arg you can pass to -fsanitize=XXXX */\n");
        print("	                                                    /* see https://gcc.gnu.org/onlinedocs/gcc-6.1.0/gcc.pdf (search -fsanitize= */\n");
        print("	    --no-sanitize {thread|vptr|etc...}				/* any from --sanitize or all */\n");
		
	exit ($x);
}





sub     ToBool_
{
    my $x = shift(@_);
	if ($x eq "true") {
		return true;
	}
	if ($x eq "false") {
		return false;
	}
	die ("expected bool argument to be true/false");
}




sub     IsGCCOrGPlusPlus_
{
    my $x = shift(@_);
	return ($x =~ /gcc[!\/]*/) || (($x =~ /g\+\+[!\/]*/) && !($x =~ /clang\+\+/));
}

sub     IsClangOrClangPlusPlus_
{
    my $x = shift(@_);
	return ($x =~ /clang[!\/]*/);
}

sub     IsClangPlusPlus_
{
    my $x = shift(@_);
	return ($x =~ /clang\+\+[!\/]*/);
}

sub     GetGCCVersion_
{
    my $x = shift(@_);
	return trim (`($x --version 2>/dev/null) | head -1 | sed 's/(.*)/x/' | awk '{print \$3;}'`);
}


sub     GetClangVersion_
{
    my $x = trim(shift(@_));
    my $ver = trim (`($x --version 2>/dev/null) | head -1 |  sed 's/.*LLVM/x/' | sed 's/)//' |  awk '{print \$2;}'`);
    if ($ver * 1 != 0) {
	return $ver;
    }
    $ver = trim(`($x --version 2>/dev/null) | head -1 | awk '{print \$3}'`);
    $ver = $ver * 1;
    return $ver;
}

sub     IsMSVCCompiler_
{
   #bad way but best I know for now
	if ("$^O" eq "cygwin") {
		return true;
	}
	return false
}



### Initial defaults before looking at command-line arguments
sub	SetInitialDefaults_
{
	if (("$^O" eq "linux") or ("$^O" eq "darwin") or ("$^O" eq "aix")) {
		$PROJECTPLATFORMSUBDIR = 'Unix';
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
	}

	if (("$^O" eq "linux") or ("$^O" eq "darwin") or ("$^O" eq "aix")) {
		$FEATUREFLAG_LIBCURL = $LIBFEATUREFLAG_UseStaticTPP;
	}
	if ("$^O" eq "aix") {
		# no good reason - just haven't gotten working yet, so default to no
		$FEATUREFLAG_LZMA = $LIBFEATUREFLAG_No;
	}
	if ("$^O" eq "cygwin") {
		$FEATUREFLAG_WinHTTP = $LIBFEATUREFLAG_UseSystem;
	}
	#if ("$^O" eq "cygwin") {
	#	$FEATUREFLAG_ATLMFC = $LIBFEATUREFLAG_UseSystem;
	#}
}



sub     ReplaceLast_
{
    my $srcString = shift(@_);
    my $replaceThis = shift(@_);
    my $withThis = shift(@_);

    #@todo find a better way to safely substitute
    my $rreplaceThis = trim (`echo $replaceThis | rev`);
    my $rwithThis = trim (`echo $withThis | rev`);
    return trim (`echo $srcString | rev | sed 's/$rreplaceThis/$rwithThis/' | rev`);
}


sub	SetDefaultForCompilerDriver_
{
	if ($CPPSTD_VERSION_FLAG eq '') {
		if (IsGCCOrGPlusPlus_ ($COMPILER_DRIVER)) {
			if (GetGCCVersion_ ($COMPILER_DRIVER) >= '4.9') {
				$CPPSTD_VERSION_FLAG="--std=c++14"
			}
			else {
				$CPPSTD_VERSION_FLAG="--std=c++11"
			}
		}
		elsif (IsClangOrClangPlusPlus_ ($COMPILER_DRIVER)) {
			if (GetClangVersion_ ($COMPILER_DRIVER) >= '3.5') {
				$CPPSTD_VERSION_FLAG="--std=c++14"
			}
			else {
				$CPPSTD_VERSION_FLAG="--std=c++11"
			}
		}
	}
	if ($PROJECTPLATFORMSUBDIR eq 'Unix') {
		$STATIC_LINK_GCCRUNTIME = 1;
	}
	if ($CWARNING_FLAGS eq "<<USE_DEFAULTS>>") {
		$CWARNING_FLAGS = '';
		if (IsGCCOrGPlusPlus_ ($COMPILER_DRIVER)) {
			$CWARNING_FLAGS = $CWARNING_FLAGS . $DEFAULT_CWARNING_FLAGS_GCC;
			if ("$^O" eq "aix") {
				$CWARNING_FLAGS = $CWARNING_FLAGS . $DEFAULT_CWARNING_FLAGS_GCC_AIX;
			}
			if (GetGCCVersion_ ($COMPILER_DRIVER) >= '5.2' && GetGCCVersion_ ($COMPILER_DRIVER) < '6') {
				#This is broken in gcc 5.2 - #https://gcc.gnu.org/ml/gcc-bugs/2015-08/msg01811.html
				$EXTRA_LINKER_ARGS = $EXTRA_LINKER_ARGS . " -Wno-odr"
			}
		}
		elsif (IsClangOrClangPlusPlus_($COMPILER_DRIVER)) {
			$CWARNING_FLAGS = $CWARNING_FLAGS . $DEFAULT_CWARNING_FLAGS_CLANG;
			my $clangVer = GetClangVersion_ ($COMPILER_DRIVER);
			if ('3.6' le $clangVer) {
				$CWARNING_FLAGS = $CWARNING_FLAGS . $DEFAULT_CWARNING_FLAGS_CLANG36Plus;
			}
			if ('3.6' le $clangVer and $clangVer lt '3.9') {
				$CWARNING_FLAGS = $CWARNING_FLAGS . $DEFAULT_CWARNING_FLAGS_CLANG36EXTRA;
			}
		}
	}

	if ($COMPILER_DRIVER_C eq "") {
		$COMPILER_DRIVER_C = $COMPILER_DRIVER;
		if (IsGCCOrGPlusPlus_($COMPILER_DRIVER)) {
			$COMPILER_DRIVER_C = ReplaceLast_ ($COMPILER_DRIVER, 'g++', 'gcc');
		}
		elsif (IsClangOrClangPlusPlus_($COMPILER_DRIVER)) {
			$COMPILER_DRIVER_C = ReplaceLast_ ($COMPILER_DRIVER, 'clang++', 'clang');
		}
	}
	if ($COMPILER_DRIVER_CPlusPlus eq "") {
		$COMPILER_DRIVER_CPlusPlus = $COMPILER_DRIVER;
		if (IsGCCOrGPlusPlus_($COMPILER_DRIVER)) {
			$COMPILER_DRIVER_CPlusPlus = ReplaceLast_ ($COMPILER_DRIVER, 'gcc', 'g++');
		}
		elsif (IsClangOrClangPlusPlus_($COMPILER_DRIVER_CPlusPlus)) {
			if (!IsClangPlusPlus_($COMPILER_DRIVER_CPlusPlus)) {
				$COMPILER_DRIVER_CPlusPlus = ReplaceLast_ ($COMPILER_DRIVER_CPlusPlus, 'clang', 'clang++');
			}
		}
	}

	if ($ApplyDebugFlags == true) {
		if ($ENABLE_ASSERTIONS == DEFAULT_BOOL_OPTIONS) {
			$ENABLE_ASSERTIONS = 1;
		}
		if (IsGCCOrGPlusPlus_($COMPILER_DRIVER_CPlusPlus) || IsClangOrClangPlusPlus_ ($COMPILER_DRIVER_CPlusPlus)) {
			if ($ENABLE_GLIBCXX_DEBUG == DEFAULT_BOOL_OPTIONS) {
				$ENABLE_GLIBCXX_DEBUG = 1;
			}
			if ($runtimeStackProtectorFlag == DEFAULT_BOOL_OPTIONS && !("$^O" eq "aix")) {
				### sadly - I've only needed this on AIX, and its only missing on AIX
				$runtimeStackProtectorFlag = true;
			}
		}

		if (IsMSVCCompiler_ ($COMPILER_DRIVER_CPlusPlus)) {
			$COPTIMIZE_FLAGS .= " /Od";
		}

		if ($ENABLE_TRACE2FILE == DEFAULT_BOOL_OPTIONS) {
			$ENABLE_TRACE2FILE = 1;
		}
		if ($INCLUDE_SYMBOLS_EXE == DEFAULT_BOOL_OPTIONS) {
			$INCLUDE_SYMBOLS_EXE = true;
		}

		if (!("$^O" eq "aix")) {
			#helpful to print stack traces in log (not critical, and has performance overhead)
			$EXTRA_LINKER_ARGS .= " -rdynamic";
		}
	}
	elsif ($ApplyReleaseFlags == true) {
		if ($ENABLE_ASSERTIONS == DEFAULT_BOOL_OPTIONS) {
			$ENABLE_ASSERTIONS = 0;
		}
		if ($ENABLE_LTO == DEFAULT_BOOL_OPTIONS) {
			if (IsGCCOrGPlusPlus_($COMPILER_DRIVER_CPlusPlus) || IsClangOrClangPlusPlus_ ($COMPILER_DRIVER_CPlusPlus)) {
				$ENABLE_LTO = 1;
			}
		}
		if (IsGCCOrGPlusPlus_($COMPILER_DRIVER_CPlusPlus) || IsClangOrClangPlusPlus_ ($COMPILER_DRIVER_CPlusPlus)) {
			if ($COPTIMIZE_FLAGS eq "") {
				$COPTIMIZE_FLAGS = "-O3";
			}
			if ($ENABLE_GLIBCXX_DEBUG == DEFAULT_BOOL_OPTIONS) {
				$ENABLE_GLIBCXX_DEBUG = 0;
			}
		}
		if (IsMSVCCompiler_ ($COMPILER_DRIVER_CPlusPlus)) {
			$COPTIMIZE_FLAGS .= " /O2 /Oi /Oy ";
		}
		if ($ENABLE_TRACE2FILE == DEFAULT_BOOL_OPTIONS) {
			$ENABLE_TRACE2FILE = 0;
		}
	}

	if (!(defined $AR) and (!("$^O" eq "aix") and IsGCCOrGPlusPlus_($COMPILER_DRIVER_CPlusPlus))) {
		my $ccLessArgs = $COMPILER_DRIVER_C;
		$ccLessArgs  =~ s/\ .*//;
		$AR = ReplaceLast_ ($ccLessArgs, 'gcc', 'gcc-ar');
	}
	if (!(defined $AR) and (!("$^O" eq "cygwin"))) {
		$AR = "ar";
	}
	if (!(defined $RANLIB) and (!("$^O" eq "aix") and IsGCCOrGPlusPlus_($COMPILER_DRIVER_CPlusPlus))) {
		my $ccLessArgs = $COMPILER_DRIVER_C;
		$ccLessArgs  =~ s/\ .*//;
		$RANLIB = ReplaceLast_ ($ccLessArgs, 'gcc', 'gcc-ranlib');
	}
	if (!(defined $RANLIB) and (!("$^O" eq "cygwin"))) {
		$RANLIB = "ranlib";
	}
}

sub	SetDefaultForPlatform_
{
	if ($PROJECTPLATFORMSUBDIR eq 'Unix') {
		$COMPILER_DRIVER = "gcc";
		if ("$^O" eq "aix") {
			$COMPILER_DRIVER = "gcc -pthread";
			#mminimal-toc seems to work better than bbigtoc -- LGP 2016-03-18
			#$EXTRA_LINKER_ARGS = "-Wl,-bbigtoc";	# we seem to almost always get these big TOC errors -- LGP 2015-08-21
			$EXTRA_COMPILER_ARGS = "-mminimal-toc";	# avoid big TOC errors -- LGP 2016-03-18
		}
		#$COMPILER_DRIVER = "clang++";
		#$COMPILER_DRIVER = "gcc";
		#$COMPILER_DRIVER = "g++ -V5.0";
	}
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
			$COMPILER_DRIVER_C = "";		#reset so computed later
			$COMPILER_DRIVER_CPlusPlus = "";
		}
		elsif ((lc ($var) eq "-apply-default-debug-flags") or (lc ($var) eq "--apply-default-debug-flags")) {
			$ApplyDebugFlags = true;
		}
		elsif ((lc ($var) eq "-apply-default-release-flags") or (lc ($var) eq "--apply-default-release-flags")) {
			$ApplyReleaseFlags = true;
		}
	}
}

sub	ParseCommandLine_Remaining_
{
	for ($i = 1; $i <= $#ARGV; $i++) {
		my $var = $ARGV[$i];
		if (lc ($var) eq "-c-define" or lc ($var) eq "--c-define") {
			$i++;
			$var = $ARGV[$i];
			push (@useExtraCDefines, $var);
		}
		elsif (lc ($var) eq "-make-define" or lc ($var) eq "--make-define") {
			$i++;
			$var = $ARGV[$i];
			push (@useExtraMakeDefines, $var);
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
                DoHelp_ (1);
			}
		}
		elsif ((lc ($var) eq "-block-allocation") or (lc ($var) eq "--block-allocation")) {
			$i++;
			$var = $ARGV[$i];
			if ($var eq "enable") {
				push (@useExtraCDefines, '#define qAllowBlockAllocation 1');
			}
			elsif ($var eq "disable") {
				push (@useExtraCDefines, '#define qAllowBlockAllocation 0');
			}
			elsif ($var eq "default") {
			}
			else  {
                print ("UNRECOGNIZED block-allocation ARG: $var\n");
                DoHelp_ (1);
			}
		}
		elsif ((lc ($var) eq "-valgrind") or (lc ($var) eq "--valgrind")) {
			$i++;
			$var = $ARGV[$i];
			if ($var eq "enable") {
				push (@useExtraCDefines, '#define qStroika_FeatureSupported_Valgrind 1');
			}
			elsif ($var eq "disable") {
				push (@useExtraCDefines, '#define qStroika_FeatureSupported_Valgrind 0');
			}
			elsif ($var eq "default") {
			}
			else  {
                print ("UNRECOGNIZED block-allocation ARG: $var\n");
                DoHelp_ (1);
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
                DoHelp_ (1);
			}
		}
		elsif ((lc ($var) eq "-cppstd-version-flag") or (lc ($var) eq "--cppstd-version-flag")) {
			$i++;
			$var = $ARGV[$i];
			$CPPSTD_VERSION_FLAG = $var;
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
        elsif ((lc ($var) eq "-openssl-extraargs") or (lc ($var) eq "--openssl-extraargs")) {
            $i++;
            $var = $ARGV[$i];
            $FEATUREFLAG_OpenSSLExtraArgs = $var;
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
        elsif ((lc ($var) eq "-sqlite") or (lc ($var) eq "--sqlite")) {
            $i++;
            $var = $ARGV[$i];
            $FEATUREFLAG_sqlite = $var;
        }
        elsif ((lc ($var) eq "-lzma") or (lc ($var) eq "--lzma")) {
            $i++;
            $var = $ARGV[$i];
            $FEATUREFLAG_LZMA = $var;
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
                DoHelp_ (1);
			}
		}
        elsif ((lc ($var) eq "-static-link-gccruntime") or (lc ($var) eq "--static-link-gccruntime")) {
            $i++;
            $var = $ARGV[$i];
            if ($var eq "enable") {
				$STATIC_LINK_GCCRUNTIME = 1;
			}
            elsif ($var eq "disable") {
				$STATIC_LINK_GCCRUNTIME = 0;
			}
			else {
                print ("UNRECOGNIZED static-link-gccruntime ARG: $var\n");
                DoHelp_ (1);
			}
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
		elsif ((lc ($var) eq "-append-extra-compiler-args") or (lc ($var) eq "--append-extra-compiler-args")) {
			$i++;
			$var = $ARGV[$i];
			if (not ($EXTRA_COMPILER_ARGS eq "")) {
				$EXTRA_COMPILER_ARGS .= " ";
			}
			$EXTRA_COMPILER_ARGS .= $var;
		}
		elsif ((lc ($var) eq "-extra-linker-args") or (lc ($var) eq "--extra-linker-args")) {
			$i++;
			$var = $ARGV[$i];
			$EXTRA_LINKER_ARGS = $var;
		}
		elsif ((lc ($var) eq "-append-extra-linker-args") or (lc ($var) eq "--append-extra-linker-args")) {
			$i++;
			$var = $ARGV[$i];
			if (not ($EXTRA_LINKER_ARGS eq "")) {
				$EXTRA_LINKER_ARGS .= " ";
			}
			$EXTRA_LINKER_ARGS .= $var;
		}
		elsif ((lc ($var) eq "-append-extra-compiler-and-linker-args") or (lc ($var) eq "--append-extra-compiler-and-linker-args")) {
			$i++;
			$var = $ARGV[$i];
			if (not ($EXTRA_COMPILER_ARGS eq "")) {
				$EXTRA_COMPILER_ARGS .= " ";
			}
			$EXTRA_COMPILER_ARGS .= $var;
			if (not ($EXTRA_LINKER_ARGS eq "")) {
				$EXTRA_LINKER_ARGS .= " ";
			}
			$EXTRA_LINKER_ARGS .= $var;
		}
		elsif ((lc ($var) eq "-lto") or (lc ($var) eq "--lto")) {
			$i++;
			$var = $ARGV[$i];
			if ($var eq "enable") {
				$ENABLE_LTO = 1;
			}
			elsif ($var eq "disable") {
				$ENABLE_LTO = 0;
			}
			elsif ($var eq "default") {
				$ENABLE_LTO = DEFAULT_BOOL_OPTIONS;
			}
			else  {
                print ("UNRECOGNIZED assertions ARG: $var\n");
                DoHelp_ (1);
			}
		}
		elsif ((lc ($var) eq "-cross-compiling") or (lc ($var) eq "--cross-compiling")) {
			$i++;
			$var = $ARGV[$i];
			if (not ($var eq "true" || $var eq "false")) {
				die ("Invalid argument to --cross-compiling");
			}
			$CrossCompiling = $var;
		}
		elsif ((lc ($var) eq "-debug-symbols") or (lc ($var) eq "--debug-symbols")) {
			$i++;
			$var = $ARGV[$i];
			if (not ($var eq "true" || $var eq "false")) {
				die ("Invalid argument to --debug-symbols");
			}
			$INCLUDE_SYMBOLS_LIB = ToBool_ ($var);
			$INCLUDE_SYMBOLS_EXE = ToBool_ ($var);
		}
		elsif ((lc ($var) eq "-debug-symbols-lib") or (lc ($var) eq "--debug-symbols-lib")) {
			$i++;
			$var = $ARGV[$i];
			if (not ($var eq "true" || $var eq "false")) {
				die ("Invalid argument to --debug-symbols-lib");
			}
			$INCLUDE_SYMBOLS_LIB = ToBool_ ($var);
		}
		elsif ((lc ($var) eq "-debug-symbols-exe") or (lc ($var) eq "--debug-symbols-exe")) {
			$i++;
			$var = $ARGV[$i];
			if (not ($var eq "true" || $var eq "false")) {
				die ("Invalid argument to --debug-symbols");
			}
			$INCLUDE_SYMBOLS_EXE = ToBool_ ($var);
		}
		elsif ((lc ($var) eq "-malloc-guard") or (lc ($var) eq "--malloc-guard")) {
			$i++;
			$MALLOC_GUARD = ToBool_ ($ARGV[$i]);
			### DISABLE LTO BY DEFAULT IF MALLOC_GAURD TURNED ON BECAUSE FOR NOW THEY DONT WORK TOGETHER
			### -- LGP 2016-07-14
			if ($MALLOC_GUARD == true) {
				$ENABLE_LTO = 0;
			}
		}
		elsif ((lc ($var) eq "-runtime-stack-check") or (lc ($var) eq "--runtime-stack-check")) {
			$i++;
			$var = $ARGV[$i];
			if (not ($var eq "true" || $var eq "false")) {
				die ("Invalid argument to --runtime-stack-check");
			}
			$runtimeStackProtectorFlag = $var;
		}
		elsif ((lc ($var) eq "-sanitize") or (lc ($var) eq "--sanitize")) {
			$i++;
			$var = $ARGV[$i];
			if ($var eq "none") {
				$sanitizerFlags = "";
			}
			else {
				if (not ($sanitizerFlags eq "")) {
					$sanitizerFlags .= ",";
				}
				$sanitizerFlags .= $var;
			}
		}
		elsif ((lc ($var) eq "-no-sanitize") or (lc ($var) eq "--no-sanitize")) {
			$i++;
			$var = $ARGV[$i];
			if ($var eq "none") {
				$noSanitizerFlags = "";
			}
			else {
				if (not ($noSanitizerFlags eq "")) {
					$noSanitizerFlags  .= ",";
				}
				$noSanitizerFlags .= $var;
			}
		}
		elsif ((lc ($var) eq "-only-if-has-compiler") or (lc ($var) eq "--only-if-has-compiler")) {
			$onlyGenerateIfCompilerExists = true;
		}
		elsif ((lc ($var) eq "-pg") or (lc ($var) eq "--pg")) {
			$EXTRA_COMPILER_ARGS .= " -pg";
			$EXTRA_LINKER_ARGS .= " -pg";
		}
		elsif ((lc ($var) eq "-apply-default-debug-flags") or (lc ($var) eq "--apply-default-debug-flags")) {
			#HANDLED EARLIER
		}
		elsif ((lc ($var) eq "-apply-default-release-flags") or (lc ($var) eq "--apply-default-release-flags")) {
			#HANDLED EARLIER
		}
		elsif ((lc ($var) eq "-help") or (lc ($var) eq "--help") or (lc ($var) eq "-?")) {
			DoHelp_ (0);
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
                DoHelp_ (1);
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
			print ("\"$var\"");
			print (" ");
		}
		print (")\n");
	}

	SetInitialDefaults_ ();
	
	if ($#ARGV >= 0) {
		$configurationName  = $ARGV[0];
	}
	else {
        print ("Expected configuration name as first argument\n");
        DoHelp_ (1);
	}
	if ((lc ($configurationName) eq "-h") || (lc ($configurationName) eq "--h") || (lc ($configurationName) eq "-help")  || (lc ($configurationName) eq "--help")) { 
        DoHelp_ (0);
	}
	elsif ($configurationName =~ /^\-/) { 
        print ("Expected configuration name (got $configurationName) as first argument\n");
        DoHelp_ (1);
	}

	ParseCommandLine_Platform_ ();
	SetDefaultForPlatform_ ();

	ParseCommandLine_CompilerDriver_ ();
	SetDefaultForCompilerDriver_();

	ParseCommandLine_Remaining_ ();

	CHECK_OPTIONS_ ();
}



ParseCommandLine_ ();


sub PostProcessOptions_ ()
{
	if ($ENABLE_LTO == true) {
		if (IsGCCOrGPlusPlus_($COMPILER_DRIVER_CPlusPlus) || IsClangOrClangPlusPlus_ ($COMPILER_DRIVER_CPlusPlus)) {
			$EXTRA_COMPILER_ARGS .= " -flto";
			$EXTRA_LINKER_ARGS .= " -flto";
		}
		if (IsMSVCCompiler_($COMPILER_DRIVER_CPlusPlus)) {
			$COPTIMIZE_FLAGS .= " /GL";
		}
	}
	if (not ($sanitizerFlags eq "")) {
		$EXTRA_COMPILER_ARGS .= " -fsanitize=" . $sanitizerFlags;
		$EXTRA_LINKER_ARGS .= " -fsanitize=" . $sanitizerFlags;
	}
	if (not ($noSanitizerFlags eq "")) {
		$EXTRA_COMPILER_ARGS .= " -fno-sanitize=" . $noSanitizerFlags;
		$EXTRA_LINKER_ARGS .= " -fno-sanitize=" . $noSanitizerFlags;
	}
	if ($FEATUREFLAG_OpenSSL eq "") {
		if ($CrossCompiling eq "false") {
			$FEATUREFLAG_OpenSSL = $LIBFEATUREFLAG_UseStaticTPP;
		}
		else {
			#See https://stroika.atlassian.net/browse/STK-427 
			#Dont know how to do this yet...
			$FEATUREFLAG_OpenSSL = $LIBFEATUREFLAG_No;
		}
	}

	if ($INCLUDE_SYMBOLS_EXE == DEFAULT_BOOL_OPTIONS) {
		$INCLUDE_SYMBOLS_EXE = false;
	}

	if ($MALLOC_GUARD eq true) {
		push (@useExtraCDefines, '#define qStroika_Foundation_Debug_MallocGuard 1');
	}
}


PostProcessOptions_ ();




# Eventually  -make this more central - put all configs here, and then
# build C++/dirs etc from this!
sub	WriteConfigFile_
{
    my $configFileName = shift(@_);

	open(OUT,">$configFileName");
	print (OUT "<!--This file autogenerated by the command\n    configure ");
	foreach $argnum (0 .. $#ARGV) {
	   print (OUT "$ARGV[$argnum] ");
	}
	print (OUT "\n-->\n\n");
	print (OUT "<Configuration>\n");
	print (OUT "    <ProjectPlatformSubdir>$PROJECTPLATFORMSUBDIR</ProjectPlatformSubdir>\n");
	#print (OUT "    <Platform>$platform</Platform>\n");

	print (OUT "    <CompilerDriver-C>$COMPILER_DRIVER_C</CompilerDriver-C>\n");
	print (OUT "    <CompilerDriver-C++>$COMPILER_DRIVER_CPlusPlus</CompilerDriver-C++>\n");
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
	if (not ($FEATUREFLAG_OpenSSLExtraArgs eq "")) {
		if ($FEATUREFLAG_OpenSSL eq $LIBFEATUREFLAG_UseStaticTPP) {
			print (OUT "    <qFeatureFlag_OpenSSL_ExtraArgs>$FEATUREFLAG_OpenSSLExtraArgs</qFeatureFlag_OpenSSL_ExtraArgs>\n");
		}
		else {
            die ("Cannot specify OpenSSL-ExtraArgs unless --OpenSSL use\n");
		}
	}
	print (OUT "    <qFeatureFlag_WinHTTP>$FEATUREFLAG_WinHTTP</qFeatureFlag_WinHTTP>\n");
	print (OUT "    <qFeatureFlag_ATLMFC>$FEATUREFLAG_ATLMFC</qFeatureFlag_ATLMFC>\n");
	print (OUT "    <qFeatureFlag_Xerces>$FEATUREFLAG_Xerces</qFeatureFlag_Xerces>\n");
	print (OUT "    <qFeatureFlag_ZLib>$FEATUREFLAG_ZLib</qFeatureFlag_ZLib>\n");
	print (OUT "    <qFeatureFlag_sqlite>$FEATUREFLAG_sqlite</qFeatureFlag_sqlite>\n");
	print (OUT "    <qFeatureFlag_LZMA>$FEATUREFLAG_LZMA</qFeatureFlag_LZMA>\n");

	if (defined $AR) {
		print (OUT "    <AR>$AR</AR>\n");
	}
	if (defined $RANLIB) {
		print (OUT "    <RANLIB>$RANLIB</RANLIB>\n");
	}

	if ($runtimeStackProtectorFlag == true) {
		$EXTRA_COMPILER_ARGS = "-fstack-protector-all " . $EXTRA_COMPILER_ARGS;	# preprend so $EXTRA_COMPILER_ARGS can override
	}
	print (OUT "    <EXTRA_COMPILER_ARGS>$EXTRA_COMPILER_ARGS</EXTRA_COMPILER_ARGS>\n");
	print (OUT "    <EXTRA_LINKER_ARGS>$EXTRA_LINKER_ARGS</EXTRA_LINKER_ARGS>\n");

	print (OUT "    <CrossCompiling>$CrossCompiling</CrossCompiling>\n");

	if ($ENABLE_TRACE2FILE != DEFAULT_BOOL_OPTIONS) {
		print (OUT "    <ENABLE_TRACE2FILE>$ENABLE_TRACE2FILE</ENABLE_TRACE2FILE>\n");
	}
	print (OUT "    <IncludeDebugSymbolsInLibraries>$INCLUDE_SYMBOLS_LIB</IncludeDebugSymbolsInLibraries>\n");
	print (OUT "    <IncludeDebugSymbolsInExecutables>$INCLUDE_SYMBOLS_EXE</IncludeDebugSymbolsInExecutables>\n");
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

my $generate = true;
if ($onlyGenerateIfCompilerExists) {
	if (trim (`./ScriptsLib/HasCompiler.sh $COMPILER_DRIVER_CPlusPlus`) eq "0") {
		$generate = false;
	}
}

if ($generate) {
	my $masterXMLConfigFile	=	"$configurationFiles" . "$configurationName.xml";
	print(`ScriptsLib/PrintLevelLeader.sh $MAKE_INDENT_LEVEL` . "Writing \"$masterXMLConfigFile\"...\n");
	WriteConfigFile_ ($masterXMLConfigFile);
	system ("rm -f IntermediateFiles/APPLIED_CONFIGURATIONS");
}
else {
	print(`ScriptsLib/PrintLevelLeader.sh $MAKE_INDENT_LEVEL` . "Skipping configuration $configurationName because compiler $COMPILER_DRIVER_CPlusPlus not present and configuration invoiked with --only-if-has-compiler\n");
}

