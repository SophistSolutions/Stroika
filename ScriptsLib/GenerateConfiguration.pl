#!/usr/bin/perl

#TRY THESE AGAIN, but for now generate too many problems...
use strict;
use warnings;
no warnings 'experimental::smartmatch';

BEGIN{ @INC = ( "./", @INC ); }

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
my $ARCH='';
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
my $CWARNING_FLAGS = undef;
my $ADD2CWARNING_FLAGS = "";

my $runtimeStackProtectorFlag = DEFAULT_BOOL_OPTIONS;
my $sanitizerFlagsIsDefined = false;		#sadly perl arrays cannot be 'undefined'
my $sanitizerFlagsNoneSet = false;
my @sanitizerFlags = ();
my $noSanitizerFlags = "";

my $ApplyDebugFlags = DEFAULT_BOOL_OPTIONS;
my $ApplyReleaseFlags = DEFAULT_BOOL_OPTIONS;

my $DEFAULT_CWARNING_FLAGS_SAFE_COMMON_ = '-Wall ';
$DEFAULT_CWARNING_FLAGS_SAFE_COMMON_	= $DEFAULT_CWARNING_FLAGS_SAFE_COMMON_ . '-Wno-switch ';
$DEFAULT_CWARNING_FLAGS_SAFE_COMMON_	= $DEFAULT_CWARNING_FLAGS_SAFE_COMMON_ . '-Wno-sign-compare ';
$DEFAULT_CWARNING_FLAGS_SAFE_COMMON_	= $DEFAULT_CWARNING_FLAGS_SAFE_COMMON_ . '-Wno-unused-variable ';
$DEFAULT_CWARNING_FLAGS_SAFE_COMMON_	= $DEFAULT_CWARNING_FLAGS_SAFE_COMMON_ . '-Wno-unused-value ';
$DEFAULT_CWARNING_FLAGS_SAFE_COMMON_	= $DEFAULT_CWARNING_FLAGS_SAFE_COMMON_ . '-Wno-strict-aliasing ';
$DEFAULT_CWARNING_FLAGS_SAFE_COMMON_	= $DEFAULT_CWARNING_FLAGS_SAFE_COMMON_ . '-Wno-comment ';
$DEFAULT_CWARNING_FLAGS_SAFE_COMMON_	= $DEFAULT_CWARNING_FLAGS_SAFE_COMMON_ . '-Wno-unused-function ';

my $DEFAULT_CWARNING_FLAGS_GCC		= $DEFAULT_CWARNING_FLAGS_SAFE_COMMON_;
$DEFAULT_CWARNING_FLAGS_GCC			= $DEFAULT_CWARNING_FLAGS_GCC . '-Wno-unused-but-set-variable ';
$DEFAULT_CWARNING_FLAGS_GCC			= $DEFAULT_CWARNING_FLAGS_GCC . '-Wno-unused-local-typedefs ';

my $DEFAULT_CWARNING_FLAGS_CLANG	= $DEFAULT_CWARNING_FLAGS_SAFE_COMMON_;
$DEFAULT_CWARNING_FLAGS_CLANG		= $DEFAULT_CWARNING_FLAGS_CLANG . '-Wno-unused-const-variable ';
$DEFAULT_CWARNING_FLAGS_CLANG		= $DEFAULT_CWARNING_FLAGS_CLANG . '-Wno-unused-local-typedef ';
$DEFAULT_CWARNING_FLAGS_CLANG		= $DEFAULT_CWARNING_FLAGS_CLANG . '-Wno-future-compat ';

my $FEATUREFLAG_ActivePerl = undef;
my $FEATUREFLAG_PrivateOverrideOfCMake = undef;
my $FEATUREFLAG_LIBCURL = $LIBFEATUREFLAG_No;		#$LIBFEATUREFLAG_UseStaticTPP; tricky some places because of dependencies - resolve that first
my $FEATUREFLAG_boost = $LIBFEATUREFLAG_UseStaticTPP;
my $FEATUREFLAG_OpenSSL = "";
my $FEATUREFLAG_OpenSSLExtraArgs = "";
my $FEATUREFLAG_WinHTTP = $LIBFEATUREFLAG_No;
my $FEATUREFLAG_ATLMFC = $LIBFEATUREFLAG_No;
my $FEATUREFLAG_Xerces = $LIBFEATUREFLAG_UseStaticTPP;
my $FEATUREFLAG_ZLib = $LIBFEATUREFLAG_UseStaticTPP;
my $FEATUREFLAG_sqlite = $LIBFEATUREFLAG_UseStaticTPP;
my $FEATUREFLAG_LZMA = $LIBFEATUREFLAG_UseStaticTPP;
my $FEATUREFLAG_librt = undef;
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
my $STRIP = undef;
my $EXTRA_COMPILER_ARGS = "";
my $EXTRA_PREFIX_LINKER_ARGS = "";
my $EXTRA_SUFFIX_LINKER_ARGS = "";
my $RUN_PREFIX = "";			# for now just used as prefix for stuff like LD_PRELOAD=/usr/lib/arm-linux-gnueabihf/libasan.so.3, esp for running tests
my $CrossCompiling = "false";
my $onlyGenerateIfCompilerExists = false;
my $INCLUDES_PATH = undef;		# space separated list
my @INCLUDES_PATH_ADD  = qw();
my $LIBS_PATH = undef;			# space separated list
my @LIBS_PATH_ADD  = qw();
my $LIB_DEPENDENCIES  = undef;	# space separated list
my @LIB_DEPENDENCIES_ADD  = qw();




sub	DoHelp_
{
    my $x = shift(@_);
    print("Usage:\n");
        print("  configure CONFIGURATION-NAME [OPTIONS]* where options can be:\n");
        print("	    --arch {ARCH}                                   /* for high level architecture - first section of gcc -machine - e.g. x86, x86_64, arm - usually auto-detected */\n");
        print("	    --platform {PLATFORM}                           /* Specifies the ProjectPlatformSubdir (Unix, VisualStudio.Net-2017) - usually auto-detected */\n");
        print("	    --assertions { enable|disable|default }         /* Enables/disable assertion feature (setting qDebug) */\n");
        print("	    --block-allocation { enable|disable|default }   /* Enables/disable block-allocation (a feature that improves performance, but messes up valgrind) */\n");
        print("	    --valgrind { enable|disable|default }           /* Enables/disable valgrind-specific runtime code (so far only needed for clean helgrind use) */\n");
        print("	    --GLIBCXX_DEBUG { enable|disable|default }      /* Enables/Disables GLIBCXX_DEBUG (G++-specific) */\n");
        print("	    --cppstd-version-flag {FLAG}                    /* Sets \$CPPSTD_VERSION_FLAG (empty str means default, but can be --std=c++14, --std=c++17,\n");
        print("	                                                       or --std=c++1z, etc) - UNIX ONLY */\n");
        print("	    --ActivePerl {use|no}                           /* Enables/disables use of ActivePerl (Windows Only) - JUST USED TO BUILD OPENSSL for Windows*/\n");
        print("	    --private-cmake-override {use|no}               /* Enables/disables use of private cmake replacement (Windows/cygwin Only) - JUST USED TO BUILD Xerces for Windows*/\n");
        print("	    --LibCurl {build-only|use|use-system|no}        /* Enables/disables use of LibCurl for this configuration [default TBD]*/\n");
        print("	    --boost {build-only|use|use-system|no}          /* Enables/disables use of boost for this configuration [default use] */\n");
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
        print("	    --c-define {ARG}                                /* Define C++ pre-processor define for the given configuration: arg appears as a line in Stroika-Configuration.h\n");
        print("	                                                       (e.g. --c-define '\\#define qCompilerAndStdLib_quick_exit_Buggy 1')*/\n");
        print("	    --make-define {ARG}                             /* Define makefile define for the given configuration: text of arg appears as line in Configuration.mk */\n");
        print("	    --compiler-driver {ARG}                         /* default is gcc */\n");
        print("	    --ar {ARG}                                      /* default is undefined, but if compiler-driver is gcc or g++, this is gcc-ar */\n");
        print("	    --ranlib {ARG}                                  /* default is undefined, but if compiler-driver is gcc or g++, this is gcc-ranlib */\n");
        print("	    --strip {ARG}                                   /* sets program to do stripping; default is undefined, but for POSIX, defaults to strip */\n");
        print("	    --compiler-warning-args {ARG}                   /* Sets variable with compiler warnings flags */\n");
        print("	    --append-compiler-warning-args {ARG}            /* Appends ARG to 'compiler warning flags */\n");
        print("	    --extra-compiler-args {ARG}                     /* Sets variable with extra args for compiler */\n");
        print("	    --append-extra-compiler-args {ARG}              /* Appends ARG to 'extra compiler */\n");
        print("	    --extra-linker-args {ARG}                       /* Sets variable with extra args for linker */\n");
        print("	    --append-extra-linker-args {ARG}                /* Appends ARG to 'extra linker */\n");
        print("	    --append-extra-compiler-and-linker-args {ARG}   /* Appends ARG to 'extra compiler' and 'extra linker' args */\n");
        print("	    --includes-path {ARG}                           /* Sets INCLUDES_PATH variable (space separated) */\n");
        print("	    --append-2-includes-path {ARG}                  /* Appends ARG to 'INCLUDES_PATH */\n");
        print("	    --libs-path {ARG}                               /* Sets LIBS_PATH variable (space separated) */\n");
        print("	    --append-2-libs-path {ARG}                      /* Appends ARG to 'LIBS_PATH */\n");
        print("	    --lib-dependencies {ARG}                        /* Sets LIB_DEPENDENCIES variable (space separated) */\n");
        print("	    --append-2-lib-dependencies {ARG}               /* Appends ARG to 'LIB_DEPENDENCIES */\n");
        print("	    --run-prefix {ARG}                              /* Sets variable RUN_PREFIX with stuff injected before run for built executables,\n");
        print("	                                                       such as LD_PRELOAD=/usr/lib/arm-linux-gnueabihf/libasan.so.3 */\n");
        print("	    --append-run-prefix {ARG}                       /* Appends ARG to 'extra linker */\n");
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
        print("	    --sanitize {none|thread|address|undefined|leak} /* if arg none, reset to none, else adds arg to sanitized feature (gcc/clang only) -\n");
        print("	                                                       any arg you can pass to -fsanitize=XXXX */\n");
        print("	                                                    /* see https://gcc.gnu.org/onlinedocs/gcc-6.1.0/gcc.pdf (search -fsanitize=; eg. --sanitize address,undefined */\n");
        print("	    --no-sanitize {thread|vptr|etc...}              /* any from --sanitize or all */\n");
		
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

#return number
sub     GetGCCVersion_
{
	no warnings;	#@todo sometimes gives warning about use of  uninitialized variable - not sure why - debug later
    my $x = shift(@_);
	return trim (`($x --version 2>/dev/null) | head -1 | sed 's/(.*)/x/' | awk '{print \$3;}'`) * 1;
}


#return number
sub     GetClangVersion_
{
    my $x = trim(shift(@_));
	my $firstLine = trim (`($x --version 2>/dev/null) | head -1`);
	no warnings 'numeric';
	if (index($firstLine, "Apple LLVM") != -1) {
		my $ver = trim(`echo "$firstLine" | awk '{print \$4}'`);
		$ver = $ver * 1;
		return $ver;
	}
	else {
		my $ver = trim (`echo "$firstLine" |  sed 's/.*LLVM/x/' | sed 's/)//' |  awk '{print \$3;}'`);
		$ver = $ver * 1;
		return $ver;
	}
}

sub     IsMSVCCompiler_
{
   #bad way but best I know for now
	if ("$^O" eq "cygwin") {
		return true;
	}
	return false
}




sub     FillDefaultIncludesPathIfNeeded_
{
	#IF we set include path  explicitly, then suppress adding include path automatically (based on features etc) - just explicitly added ones from the commandline
	my $includeAutomaticDependencies = !defined ($INCLUDES_PATH);
	if (!defined ($INCLUDES_PATH)) {
		$INCLUDES_PATH = "";
	}
	my $var;
	foreach $var (@INCLUDES_PATH_ADD) {
		if (not ($INCLUDES_PATH eq "")) {
			$INCLUDES_PATH .= " ";
		}
		$INCLUDES_PATH .= $var;
	}
	if ($includeAutomaticDependencies) {
		if (
			$FEATUREFLAG_Xerces eq $LIBFEATUREFLAG_UseStaticTPP ||
			$FEATUREFLAG_LIBCURL eq $LIBFEATUREFLAG_UseStaticTPP ||
			$FEATUREFLAG_boost eq $LIBFEATUREFLAG_UseStaticTPP ||
			$FEATUREFLAG_OpenSSL eq $LIBFEATUREFLAG_UseStaticTPP ||
			$FEATUREFLAG_ZLib eq $LIBFEATUREFLAG_UseStaticTPP ||
			$FEATUREFLAG_sqlite eq $LIBFEATUREFLAG_UseStaticTPP ||
			$FEATUREFLAG_LZMA eq $LIBFEATUREFLAG_UseStaticTPP
		) {
			if (not ($INCLUDES_PATH eq "")) {
				$INCLUDES_PATH .= " ";
			}
			$INCLUDES_PATH .= trim (`realpath --canonicalize-missing Builds/$configurationName/ThirdPartyComponents/include/`) . "/";
		}

		if (not ($INCLUDES_PATH eq "")) {
			$INCLUDES_PATH .= " ";
		}
		$INCLUDES_PATH .= trim (`realpath --canonicalize-missing Library/Sources/`) . "/";

		$INCLUDES_PATH .= " ";
		$INCLUDES_PATH .= trim (`realpath --canonicalize-missing IntermediateFiles/$configurationName`) . "/";
	}
}
sub     FillDefaultLibsPathIfNeeded_
{
	my $includeAutomaticDependencies = !defined ($LIBS_PATH);
	if (!defined ($LIBS_PATH)) {
		$LIBS_PATH = "";
	}
	my $var;
	foreach $var (@LIBS_PATH_ADD) {
		if (not ($LIBS_PATH eq "")) {
			$LIBS_PATH .= " ";
		}
		$LIBS_PATH .= $var;
	}
	if ($includeAutomaticDependencies) {
		if (
			$FEATUREFLAG_Xerces eq $LIBFEATUREFLAG_UseStaticTPP ||
			$FEATUREFLAG_LIBCURL eq $LIBFEATUREFLAG_UseStaticTPP ||
			$FEATUREFLAG_boost eq $LIBFEATUREFLAG_UseStaticTPP ||
			$FEATUREFLAG_OpenSSL eq $LIBFEATUREFLAG_UseStaticTPP ||
			$FEATUREFLAG_ZLib eq $LIBFEATUREFLAG_UseStaticTPP ||
			$FEATUREFLAG_sqlite eq $LIBFEATUREFLAG_UseStaticTPP ||
			$FEATUREFLAG_LZMA eq $LIBFEATUREFLAG_UseStaticTPP
		) {
			$LIBS_PATH = trim (`realpath --canonicalize-missing Builds/$configurationName/ThirdPartyComponents/lib/`) . "/";
		}
	}
}
sub     FillDefaultLibDependencies_
{
	#IF we set lib dependencies explicitly, then suppress adding libs automatically (based on features etc) - just explicitly added ones from the commandline
	my $includeAutomaticLibDependencies = !defined ($LIB_DEPENDENCIES);
	if (!defined ($LIB_DEPENDENCIES)) {
		$LIB_DEPENDENCIES = "";
	}
	my $var;
	foreach $var (@LIB_DEPENDENCIES_ADD) {
		if (not ($LIB_DEPENDENCIES eq "")) {
			$LIB_DEPENDENCIES .= " ";
		}
		$LIB_DEPENDENCIES .= $var;
	}

	if ($includeAutomaticLibDependencies) {
		if ($FEATUREFLAG_Xerces ne $LIBFEATUREFLAG_No) {
			if (not ($LIB_DEPENDENCIES eq "")) {
				$LIB_DEPENDENCIES .= " ";
			}
			$LIB_DEPENDENCIES .= "-lxerces-c"
		}

		if ($FEATUREFLAG_LZMA ne $LIBFEATUREFLAG_No) {
			if (not ($LIB_DEPENDENCIES eq "")) {
				$LIB_DEPENDENCIES .= " ";
			}
			$LIB_DEPENDENCIES .= "-llzma"
		}

		if ($FEATUREFLAG_sqlite ne $LIBFEATUREFLAG_No) {
			if (not ($LIB_DEPENDENCIES eq "")) {
				$LIB_DEPENDENCIES .= " ";
			}
			$LIB_DEPENDENCIES .= "-lsqlite"
		}

		if ($FEATUREFLAG_ZLib ne $LIBFEATUREFLAG_No) {
			if (not ($LIB_DEPENDENCIES eq "")) {
				$LIB_DEPENDENCIES .= " ";
			}
			$LIB_DEPENDENCIES .= "-lz"
		}

		if (not ($LIB_DEPENDENCIES eq "")) {
			$LIB_DEPENDENCIES .= " ";
		}
		$LIB_DEPENDENCIES .= "-lpthread";

		if (not ($LIB_DEPENDENCIES eq "")) {
			$LIB_DEPENDENCIES .= " ";
		}
		$LIB_DEPENDENCIES .= "-lm";

		if ($FEATUREFLAG_librt eq $LIBFEATUREFLAG_UseSystem) {
			if (not ($LIB_DEPENDENCIES eq "")) {
				$LIB_DEPENDENCIES .= " ";
			}
			$LIB_DEPENDENCIES .= "-lrt";
		}
	}
}




### Initial defaults before looking at command-line arguments
sub	SetInitialDefaults_
{
	if (("$^O" eq "linux") or ("$^O" eq "darwin")) {
		$PROJECTPLATFORMSUBDIR = 'Unix';
	}
	if ("$^O" eq "cygwin") {
		# try vs 2k15
		if ($PROJECTPLATFORMSUBDIR eq "") {
			my $PROGRAMFILESDIR= trim (`cygpath \"$ENV{'PROGRAMFILES'}\"`);
			my $PROGRAMFILESDIR2= "/cygdrive/c/Program Files (x86)/";
			system ('ls -l "/cygdrive/c/Program Files (x86)/Microsoft Visual Studio/"2017/*/VC >/dev/null 2> /dev/null');
			if ($? == 0) {
				$PROJECTPLATFORMSUBDIR = 'VisualStudio.Net-2017';
			}
			system ('ls -l "/cygdrive/c/Program Files (x86)/Microsoft Visual Studio/"2017/*/VC >/dev/null 2> /dev/null');
			if ($? == 0) {
				$PROJECTPLATFORMSUBDIR = 'VisualStudio.Net-2017';
			}
			#autodetect ATLMFC (Express verison missing it)
			if ($PROJECTPLATFORMSUBDIR eq "VisualStudio.Net-2017") {
				system ('ls -l "/cygdrive/c/Program Files (x86)//Microsoft Visual Studio/2017/"*/VC/Tools/MSVC/*/atlmfc/include/afxext.h >/dev/null 2> /dev/null');
				if ($? == 0) {
					$FEATUREFLAG_ATLMFC = $LIBFEATUREFLAG_UseSystem;
				}
				else  {
					$FEATUREFLAG_ATLMFC = $LIBFEATUREFLAG_No;
				}
			}
		}
	}

	if (("$^O" eq "linux") or ("$^O" eq "darwin")) {
		$FEATUREFLAG_LIBCURL = $LIBFEATUREFLAG_UseStaticTPP;
	}
	if ("$^O" eq "cygwin") {
		$FEATUREFLAG_WinHTTP = $LIBFEATUREFLAG_UseSystem;
	}
	if ("$^O" eq "darwin") {
		# hacks so can do initial port/compile
		#$FEATUREFLAG_OpenSSL = $LIBFEATUREFLAG_No;
		$FEATUREFLAG_librt = $LIBFEATUREFLAG_No;
		$STATIC_LINK_GCCRUNTIME = 0;
		$COMPILER_DRIVER_C = "clang";
		$COMPILER_DRIVER_CPlusPlus = "clang++";
		$CPPSTD_VERSION_FLAG="--std=c++14";
		$CWARNING_FLAGS = $DEFAULT_CWARNING_FLAGS_CLANG;
		$AR = "ar";
		$RANLIB = "ranlib";
	}
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
	no warnings;	#@todo fix - not sure why we get warning on use of $CPPSTD_VERSION_FLAG
	if ($CPPSTD_VERSION_FLAG eq '') {
		if (IsGCCOrGPlusPlus_ ($COMPILER_DRIVER)) {
			if (GetGCCVersion_ ($COMPILER_DRIVER) >= 6.0) {
				$CPPSTD_VERSION_FLAG="--std=c++17"
			}
			elsif (GetGCCVersion_ ($COMPILER_DRIVER) >= 4.9) {
				$CPPSTD_VERSION_FLAG="--std=c++14"
			}
			else {
				#$CPPSTD_VERSION_FLAG="--std=c++11"  ### soon losing c++11 support
			}
		}
		elsif (IsClangOrClangPlusPlus_ ($COMPILER_DRIVER)) {
			if (GetClangVersion_ ($COMPILER_DRIVER) >= '5.0') {	### not 4.0 not working with clang - could be my bug but dont default to not working
				$CPPSTD_VERSION_FLAG="--std=c++17"
			}
			else {
				$CPPSTD_VERSION_FLAG="--std=c++14"
			}
		}
	}
	if ($PROJECTPLATFORMSUBDIR eq 'Unix') {
		if ($STATIC_LINK_GCCRUNTIME == DEFAULT_BOOL_OPTIONS) {
			$STATIC_LINK_GCCRUNTIME = 1;
		}
	}
	if (! defined $CWARNING_FLAGS) {
		$CWARNING_FLAGS = '';
		if (IsGCCOrGPlusPlus_ ($COMPILER_DRIVER)) {
			$CWARNING_FLAGS = $CWARNING_FLAGS . $DEFAULT_CWARNING_FLAGS_GCC;
			if (GetGCCVersion_ ($COMPILER_DRIVER) >= 5.2 && GetGCCVersion_ ($COMPILER_DRIVER) < 6) {
				#This is broken in gcc 5.2 - #https://gcc.gnu.org/ml/gcc-bugs/2015-08/msg01811.html
				$EXTRA_PREFIX_LINKER_ARGS = $EXTRA_PREFIX_LINKER_ARGS . " -Wno-odr"
			}
		}
		elsif (IsClangOrClangPlusPlus_($COMPILER_DRIVER)) {
			$CWARNING_FLAGS = $CWARNING_FLAGS . $DEFAULT_CWARNING_FLAGS_CLANG;
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
			if ($runtimeStackProtectorFlag == DEFAULT_BOOL_OPTIONS) {
				$runtimeStackProtectorFlag = true;
			}
			if (trim (`uname -r`) eq "4.4.0-43-Microsoft") {
				#LEAVE empty default, cuz for this version of WSL, asan doesn't work - insufficient procfs support
			}
			elsif (!$sanitizerFlagsNoneSet) {
				my %already = map { $_ => 1 } @sanitizerFlags;
				if(!exists($already{'address'})) {
					push @sanitizerFlags, 'address';
				}
				if(!exists($already{'undefined'})) {
					push @sanitizerFlags, 'undefined';
				}
				$sanitizerFlagsIsDefined = true;
			}
			# @see https://stroika.atlassian.net/browse/STK-601 for details on why this is needed (ObjectVariantMapper) - qCompiler_SanitizerFunctionPtrConversionSuppressionBug
			if (IsClangOrClangPlusPlus_ ($COMPILER_DRIVER_CPlusPlus) && GetClangVersion_ ($COMPILER_DRIVER_CPlusPlus) >= '4.0') {
				my $test2Suppress = "function";
				if ("$^O" eq "darwin") {
					$test2Suppress = "vptr";
				}
				if ($noSanitizerFlags eq "") {
					$noSanitizerFlags = $test2Suppress;
				}
				else {
					$noSanitizerFlags = $test2Suppress . "," . $noSanitizerFlags;
				}
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

		#helpful to print stack traces in log (not critical, and has performance overhead)
		$EXTRA_PREFIX_LINKER_ARGS .= " -rdynamic";
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

	if (!(defined $AR) and (IsGCCOrGPlusPlus_($COMPILER_DRIVER_CPlusPlus))) {
		my $ccLessArgs = $COMPILER_DRIVER_C;
		$ccLessArgs  =~ s/\ .*//;
		$AR = ReplaceLast_ ($ccLessArgs, 'gcc', 'gcc-ar');
	}
	if (!(defined $AR) and (!("$^O" eq "cygwin"))) {
		$AR = "ar";
	}
	if (!(defined $RANLIB) and (IsGCCOrGPlusPlus_($COMPILER_DRIVER_CPlusPlus))) {
		my $ccLessArgs = $COMPILER_DRIVER_C;
		$ccLessArgs  =~ s/\ .*//;
		$RANLIB = ReplaceLast_ ($ccLessArgs, 'gcc', 'gcc-ranlib');
	}
	if (!(defined $STRIP) and (IsGCCOrGPlusPlus_($COMPILER_DRIVER_CPlusPlus))) {
		my $ccLessArgs = $COMPILER_DRIVER_C;
		$ccLessArgs  =~ s/\ .*//;
		$STRIP = trim (`$ccLessArgs -print-multiarch`) . "-strip";
	}
	if (!(defined $RANLIB) and (!("$^O" eq "cygwin"))) {
		$RANLIB = "ranlib";
	}
	if (!(defined $FEATUREFLAG_librt) and (!("$^O" eq "cygwin"))) {
		$FEATUREFLAG_librt = $LIBFEATUREFLAG_UseSystem;
	}
	if (!(defined $FEATUREFLAG_librt)) {
		$FEATUREFLAG_librt = $LIBFEATUREFLAG_No;
	}
	if (!(defined $STRIP) and  $PROJECTPLATFORMSUBDIR eq 'Unix') {
		$STRIP = "strip";
	}
}

sub	SetDefaultForPlatform_
{
	if ($PROJECTPLATFORMSUBDIR eq 'Unix') {
		$COMPILER_DRIVER = "gcc";
		#$COMPILER_DRIVER = "clang++";
		#$COMPILER_DRIVER = "gcc";
		#$COMPILER_DRIVER = "g++ -V5.0";
	}
}




### Do initial pass, just looking for platform
sub	ParseCommandLine_Platform_
{
	for (my $i = 0; $i <= $#ARGV; $i++) {
		my $var = $ARGV[$i];
		if ((lc ($var) eq "-platform") or (lc ($var) eq "--platform")) {
			$i++;
			$var = $ARGV[$i];
			$PROJECTPLATFORMSUBDIR = $var;
			SetDefaultForPlatform_ ();
		}
	}
}

### Do initial pass, just looking for platform
sub	ParseCommandLine_CompilerDriver_
{
	for (my $i = 0; $i <= $#ARGV; $i++) {
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
	for (my $i = 1; $i <= $#ARGV; $i++) {
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
		elsif (lc ($var) eq "-platform" or lc ($var) eq "--platform") {
			$i++;
			$var = $ARGV[$i];
			$PROJECTPLATFORMSUBDIR = $var;
		}
		elsif (lc ($var) eq "-arch" or lc ($var) eq "--arch") {
			$i++;
			$var = $ARGV[$i];
			$ARCH = $var;
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
        elsif ((lc ($var) eq "-activeperl") or (lc ($var) eq "--activeperl")) {
            $i++;
            $var = $ARGV[$i];
            $FEATUREFLAG_ActivePerl = $var;
        }
        elsif ((lc ($var) eq "-private-cmake-override") or (lc ($var) eq "--private-cmake-override")) {
            $i++;
            $var = $ARGV[$i];
            $FEATUREFLAG_PrivateOverrideOfCMake = $var;
        }
        elsif ((lc ($var) eq "-libcurl") or (lc ($var) eq "--libcurl")) {
            $i++;
            $var = $ARGV[$i];
            $FEATUREFLAG_LIBCURL = $var;
        }
        elsif ((lc ($var) eq "-boost") or (lc ($var) eq "--boost")) {
            $i++;
            $var = $ARGV[$i];
            $FEATUREFLAG_boost = $var;
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
		elsif ((lc ($var) eq "-strip") or (lc ($var) eq "--strip")) {
			$i++;
			$var = $ARGV[$i];
			$STRIP = $var;
		}
		elsif ((lc ($var) eq "-compiler-warning-args") or (lc ($var) eq "--compiler-warning-args")) {
			$i++;
			$var = $ARGV[$i];
			$CWARNING_FLAGS = $var;
		}
		elsif ((lc ($var) eq "-append-compiler-warning-args") or (lc ($var) eq "--append-compiler-warning-args")) {
			$i++;
			$var = $ARGV[$i];
			if (not ($ADD2CWARNING_FLAGS eq "")) {
				$ADD2CWARNING_FLAGS .= " ";
			}
			$ADD2CWARNING_FLAGS .= $var;
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
			$EXTRA_PREFIX_LINKER_ARGS = $var;
		}
		elsif ((lc ($var) eq "-append-extra-linker-args") or (lc ($var) eq "--append-extra-linker-args")) {
			$i++;
			$var = $ARGV[$i];
			if (not ($EXTRA_PREFIX_LINKER_ARGS eq "")) {
				$EXTRA_PREFIX_LINKER_ARGS .= " ";
			}
			$EXTRA_PREFIX_LINKER_ARGS .= $var;
		}
		elsif ((lc ($var) eq "-extra-linker-args") or (lc ($var) eq "--extra-linker-args")) {
			$i++;
			$var = $ARGV[$i];
			$EXTRA_SUFFIX_LINKER_ARGS = $var;
		}
		elsif ((lc ($var) eq "-append-extra-linker-args") or (lc ($var) eq "--append-extra-linker-args")) {
			$i++;
			$var = $ARGV[$i];
			if (not ($EXTRA_SUFFIX_LINKER_ARGS eq "")) {
				$EXTRA_SUFFIX_LINKER_ARGS .= " ";
			}
			$EXTRA_SUFFIX_LINKER_ARGS .= $var;
		}
		elsif ((lc ($var) eq "-includes-path") or (lc ($var) eq "--includes-path")) {
			$i++;
			$var = $ARGV[$i];
			$INCLUDES_PATH = $var;
		}
		elsif ((lc ($var) eq "-append-2-includes-path") or (lc ($var) eq "--append-2-includes-path")) {
			$i++;
			$var = $ARGV[$i];
			push @INCLUDES_PATH_ADD, $var;
		}
		elsif ((lc ($var) eq "-libs-path") or (lc ($var) eq "--libs-path")) {
			$i++;
			$var = $ARGV[$i];
			$LIBS_PATH = $var;
		}
		elsif ((lc ($var) eq "-append-2-libs-path") or (lc ($var) eq "--append-2-libs-path")) {
			$i++;
			$var = $ARGV[$i];
			push @LIBS_PATH_ADD, $var;
		}
		elsif ((lc ($var) eq "-lib-dependencies") or (lc ($var) eq "--lib-dependencies")) {
			$i++;
			$var = $ARGV[$i];
			$LIB_DEPENDENCIES = $var;
		}
		elsif ((lc ($var) eq "-append-2-lib-dependencies") or (lc ($var) eq "--append-2-lib-dependencies")) {
			$i++;
			$var = $ARGV[$i];
			push @LIB_DEPENDENCIES_ADD, $var;
		}
		elsif ((lc ($var) eq "-run-prefix") or (lc ($var) eq "--run-prefix")) {
			$i++;
			$var = $ARGV[$i];
			$RUN_PREFIX = $var;
		}
		elsif ((lc ($var) eq "-append-run-prefix") or (lc ($var) eq "--append-run-prefix")) {
			$i++;
			$var = $ARGV[$i];
			if (not ($RUN_PREFIX eq "")) {
				$RUN_PREFIX .= " ";
			}
			$RUN_PREFIX .= $var;
		}
		elsif ((lc ($var) eq "-append-extra-compiler-and-linker-args") or (lc ($var) eq "--append-extra-compiler-and-linker-args")) {
			$i++;
			$var = $ARGV[$i];
			if (not ($EXTRA_COMPILER_ARGS eq "")) {
				$EXTRA_COMPILER_ARGS .= " ";
			}
			$EXTRA_COMPILER_ARGS .= $var;
			if (not ($EXTRA_PREFIX_LINKER_ARGS eq "")) {
				$EXTRA_PREFIX_LINKER_ARGS .= " ";
			}
			$EXTRA_PREFIX_LINKER_ARGS .= $var;
			if (not ($EXTRA_SUFFIX_LINKER_ARGS eq "")) {
				$EXTRA_SUFFIX_LINKER_ARGS .= " ";
			}
			$EXTRA_SUFFIX_LINKER_ARGS .= $var;
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
			my @splitSanFlags = split(/,/, $var);
			foreach my $ii (0 .. $#splitSanFlags) {
				my $flag = $splitSanFlags[$ii];
				if ($flag eq "none") {
					@sanitizerFlags = ();
					$sanitizerFlagsNoneSet = true;
				}
				else {
					my %already = map { $_ => 1 } @sanitizerFlags;
					if(!exists($already{$flag})) {
						push @sanitizerFlags, $flag;
					}
				}
				$sanitizerFlagsIsDefined = true;
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
			$EXTRA_PREFIX_LINKER_ARGS .= " -pg";
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
	if ($PROJECTPLATFORMSUBDIR eq "VisualStudio.Net-2012") {
		die ("WE NO LONGER SUPPORT VISUAL STUDIO.Net 2012\n");
	}
	if ($PROJECTPLATFORMSUBDIR eq "VisualStudio.Net-2015") {
		die ("WE NO LONGER SUPPORT VISUAL STUDIO.Net 2015\n");
	}
  	CHECK_FEATURE_OPTION($FEATUREFLAG_LIBCURL);
}

sub	ParseCommandLine_
{
	if (false) {
		# Helpful to debug scripts...
		print "Entering GenerateConfiguration.pl (";
		for (my $i = 0; $i <= $#ARGV; $i++) {
			my $var = $ARGV[$i];
			print ("\"$var\"");
			print (" ");
		}
		print ")\n";
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

	ParseCommandLine_Remaining_ ();

	SetDefaultForCompilerDriver_();

	CHECK_OPTIONS_ ();
}



ParseCommandLine_ ();


sub PostProcessOptions_ ()
{
	if ($ENABLE_LTO == true) {
		if (IsGCCOrGPlusPlus_($COMPILER_DRIVER_CPlusPlus) || IsClangOrClangPlusPlus_ ($COMPILER_DRIVER_CPlusPlus)) {
			$EXTRA_COMPILER_ARGS .= " -flto";
			$EXTRA_PREFIX_LINKER_ARGS .= " -flto";
		}
		if (IsMSVCCompiler_($COMPILER_DRIVER_CPlusPlus)) {
			$COPTIMIZE_FLAGS .= " /GL";
		}
	}
	if ($sanitizerFlagsIsDefined and ($#sanitizerFlags != -1)) {
		$EXTRA_COMPILER_ARGS .= " -fsanitize=";
		$EXTRA_PREFIX_LINKER_ARGS .= " -fsanitize=";
		foreach my $i (0 .. $#sanitizerFlags) {
			$EXTRA_COMPILER_ARGS .= $sanitizerFlags[$i];
			$EXTRA_PREFIX_LINKER_ARGS .= $sanitizerFlags[$i];
			if ($i < $#sanitizerFlags) {
				$EXTRA_COMPILER_ARGS .= ",";
				$EXTRA_PREFIX_LINKER_ARGS .= ",";
			}
		}
	}
	if (not ($noSanitizerFlags eq "")) {
		$EXTRA_COMPILER_ARGS .= " -fno-sanitize=" . $noSanitizerFlags;
		$EXTRA_PREFIX_LINKER_ARGS .= " -fno-sanitize=" . $noSanitizerFlags;
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

	
	
	if (! defined $FEATUREFLAG_ActivePerl) {
		if ($FEATUREFLAG_OpenSSL eq "use" && ("$^O" eq "cygwin")) {
			$FEATUREFLAG_ActivePerl = "use";
		}
		else {
			$FEATUREFLAG_ActivePerl = $LIBFEATUREFLAG_No;
		}
	}
	if (! defined $FEATUREFLAG_PrivateOverrideOfCMake) {
		if ($FEATUREFLAG_Xerces eq "use" && ("$^O" eq "cygwin")) {
			$FEATUREFLAG_PrivateOverrideOfCMake = "use";
		}
		else {
			$FEATUREFLAG_PrivateOverrideOfCMake = $LIBFEATUREFLAG_No;
		}
	}

	$CWARNING_FLAGS .= $ADD2CWARNING_FLAGS;

	if ($ARCH eq "") {
		if ($COMPILER_DRIVER_CPlusPlus eq "") {
			###tmphack needed for windows where we dont find the compiler variable here (SHOULD FIX THAT) @todo - LGP 2017-12-17
			if ($configurationName =~ /64$/) {
				$ARCH = "x86_64";
			}
			else {
				$ARCH = "x86";
			}
		}
		else {
			$ARCH = trim (`./ScriptsLib/GetCompilerArch.sh $COMPILER_DRIVER_CPlusPlus`);
		}
	}


	if ($PROJECTPLATFORMSUBDIR eq 'Unix') {
		#if ($STATIC_LINK_GCCRUNTIME == DEFAULT_BOOL_OPTIONS) {
		#	$STATIC_LINK_GCCRUNTIME = 1;
		#}
		if ($STATIC_LINK_GCCRUNTIME == 1) {
			my $IF_STATIC_LINK_GCCRUNTIME_USE_PRINTPATH_METHOD = 1;
			if ($IF_STATIC_LINK_GCCRUNTIME_USE_PRINTPATH_METHOD == 1) {
				my $lib = trim (`$COMPILER_DRIVER_CPlusPlus -print-file-name=libstdc++.a 2>/dev/null`);
				if (defined $lib) {
					push @LIB_DEPENDENCIES_ADD, " $lib";
				}
			}
			else {
				push @LIB_DEPENDENCIES_ADD, " -lstdc++";
			}
		}
		if ($STATIC_LINK_GCCRUNTIME == 1) {
			$EXTRA_SUFFIX_LINKER_ARGS .= " -static-libstdc++";
		}

		if (IsGCCOrGPlusPlus_ ($COMPILER_DRIVER_CPlusPlus)) {
			if (GetGCCVersion_ ($COMPILER_DRIVER_CPlusPlus) < '8') {
				push @LIB_DEPENDENCIES_ADD, "-lstdc++fs";
			}
		}
		elsif (IsClangOrClangPlusPlus_ ($COMPILER_DRIVER_CPlusPlus)) {
			if ("$^O" eq "darwin") {
				#xcode not supporting filesystem API (so use boost)
				#$EXTRA_SUFFIX_LINKER_ARGS .= " -lc++experimental";
				if ($FEATUREFLAG_boost ne $LIBFEATUREFLAG_No) {
					push @LIB_DEPENDENCIES_ADD, "-lboost_system";
					push @LIB_DEPENDENCIES_ADD, "-lboost_filesystem";
				}
			}
			else {
				if (GetClangVersion_ ($COMPILER_DRIVER_CPlusPlus) < '7.0') {
					if (/-stdlib=libc++/i ~~ @LIB_DEPENDENCIES_ADD) {
					#if (index ($EXTRA_SUFFIX_LINKER_ARGS, "-stdlib=libc++") != -1) {
						$EXTRA_SUFFIX_LINKER_ARGS .= " ";
						push @LIB_DEPENDENCIES_ADD, "-lc++experimental";
					}
					else {
						push @LIB_DEPENDENCIES_ADD, "-lstdc++fs";
					}
				}
			}
		}


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
	foreach my $argnum (0 .. $#ARGV) {
	   print (OUT "$ARGV[$argnum] ");
	}
	print (OUT "\n-->\n\n");
	print (OUT "<Configuration>\n");
	print (OUT "    <ProjectPlatformSubdir>$PROJECTPLATFORMSUBDIR</ProjectPlatformSubdir>\n");
	print (OUT "    <ARCH>$ARCH</ARCH>\n");
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
	
	
	print (OUT "    <qFeatureFlag_ActivePerl>$FEATUREFLAG_ActivePerl</qFeatureFlag_ActivePerl>\n");
	print (OUT "    <qFeatureFlag_boost>$FEATUREFLAG_boost</qFeatureFlag_boost>\n");
	print (OUT "    <qFeatureFlag_PrivateOverrideOfCMake>$FEATUREFLAG_PrivateOverrideOfCMake</qFeatureFlag_PrivateOverrideOfCMake>\n");
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
	print (OUT "    <qFeatureFlag_librt>$FEATUREFLAG_librt</qFeatureFlag_librt>\n");

	if (defined $AR) {
		print (OUT "    <AR>$AR</AR>\n");
	}
	if (defined $RANLIB) {
		print (OUT "    <RANLIB>$RANLIB</RANLIB>\n");
	}
	if (defined $STRIP) {
		print (OUT "    <STRIP>$STRIP</STRIP>\n");
	}

	if ($runtimeStackProtectorFlag == true) {
		$EXTRA_COMPILER_ARGS = "-fstack-protector-all " . $EXTRA_COMPILER_ARGS;	# preprend so $EXTRA_COMPILER_ARGS can override
	}
	print (OUT "    <EXTRA_COMPILER_ARGS>$EXTRA_COMPILER_ARGS</EXTRA_COMPILER_ARGS>\n");
	print (OUT "    <EXTRA_PREFIX_LINKER_ARGS>$EXTRA_PREFIX_LINKER_ARGS</EXTRA_PREFIX_LINKER_ARGS>\n");
	print (OUT "    <EXTRA_SUFFIX_LINKER_ARGS>$EXTRA_SUFFIX_LINKER_ARGS</EXTRA_SUFFIX_LINKER_ARGS>\n");

	print (OUT "    <RUN_PREFIX>$RUN_PREFIX</RUN_PREFIX>\n");

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

	FillDefaultIncludesPathIfNeeded_();
	FillDefaultLibsPathIfNeeded_();
	FillDefaultLibDependencies_();
	print (OUT "    <INCLUDES_PATH>$INCLUDES_PATH</INCLUDES_PATH>\n");
	print (OUT "    <LIBS_PATH>$LIBS_PATH</LIBS_PATH>\n");
	print (OUT "    <LIB_DEPENDENCIES>$LIB_DEPENDENCIES</LIB_DEPENDENCIES>\n");

	print (OUT "    <ExtraCDefines>\n");
		foreach my $var (@useExtraCDefines)
		{
			print (OUT "       <CDefine>$var</CDefine>\n");
		}
	print (OUT "    </ExtraCDefines>\n");
	
	print (OUT "    <ExtraMakeDefines>\n");
		foreach my $var (@useExtraMakeDefines)
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
	print(`ScriptsLib/PrintLevelLeader.sh $MAKE_INDENT_LEVEL` . "Writing \"$masterXMLConfigFile\" ... ");
	WriteConfigFile_ ($masterXMLConfigFile);
	system ("rm -f IntermediateFiles/APPLIED_CONFIGURATIONS");
	print("done\n");
}
else {
	print(`ScriptsLib/PrintLevelLeader.sh $MAKE_INDENT_LEVEL` . "Skipping configuration $configurationName because compiler $COMPILER_DRIVER_CPlusPlus not present and configuration invoked with --only-if-has-compiler\n");
}

