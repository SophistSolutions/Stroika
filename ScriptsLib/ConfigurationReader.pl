#!/usr/bin/perl
#use strict;
#use warnings;

### GIVE UP ON PERL - CANNOT EASILY INSTALL XML READER - SO JUST USE 
### tesxt reading..

BEGIN{ @INC = ( "./", @INC ); }

my $intermediateFiles	=	GetThisScriptDir() . "/../IntermediateFiles";
my $configurationFiles	=	GetThisScriptDir() . "/../ConfigurationFiles";
my $masterXMLConfigFile	=	"";

my @useExtraMakeDefines;

#
# BUILD iff LIBFEATUREFLAG_BuildOnly OR LIBFEATUREFLAG_UseStaticTPP
# HAS_FEATURE iff LIBFEATUREFLAG_UseStaticTPP OR LIBFEATUREFLAG_UseSystem
#
my $LIBFEATUREFLAG_BuildOnly = "build-only";
my $LIBFEATUREFLAG_UseStaticTPP = "use";
my $LIBFEATUREFLAG_UseSystem = "use-system";
my $LIBFEATUREFLAG_No = "no";

my $fileErr = "";

# Declare the subroutines
sub	GetThisScriptDir {
	use File::Basename;
	use Cwd 'abs_path';
	my $p = __FILE__;
	my $A = abs_path ($p);
	my $dirname = dirname($A);
	return $dirname;
}

sub	GetAllOfStringToEndTag($) {
	my $string = shift;
	my $eos = index($string, "<");
	if ($eos == -1) {
		$string;
	}
	else {
		return substr ($string, 0, $eos);
	}
}

my %configuration = ();



sub	ReadValue_ {
	my $line = shift;
	my $tag = shift;
	my $startOff = index($line, $tag);
	if ($startOff != -1) {
		my $tmp = substr ($line, $startOff + length($tag));
		return GetAllOfStringToEndTag ($tmp);
	}
	return undef;
}

sub	ReadConfigFile_ {
	my $myDirName	=	GetThisScriptDir ();

	#print ("ENTERING ReadConfigFile_ ($masterXMLConfigFile)\n");
	open (FILE, "$masterXMLConfigFile") or $fileErr = "Unable to open $masterXMLConfigFile";
	if ($fileErr ne '') {
		return;
	}
	my @data = <FILE>;
	close(FILE);
	foreach $line (@data) {
		my $pps = ReadValue_($line, "<ConfigureCommandLine>");
		if (defined $pps) {
			$configuration {'ConfigureCommandLine'} = $pps;
		}
		my $pps = ReadValue_($line, "<BuildPlatform>");
		if (defined $pps) {
			$configuration {'BuildPlatform'} = $pps;
		}
		my $pps = ReadValue_($line, "<TargetPlatforms>");
		if (defined $pps) {
			$configuration {'TargetPlatforms'} = $pps;
		}
		my $pps = ReadValue_($line, "<BuildByDefault>");
		if (defined $pps) {
			$configuration {'BuildByDefault'} = $pps;
		}
		my $pps = ReadValue_($line, "<BuildToolsRoot>");
		if (defined $pps) {
			$configuration {'BuildToolsRoot'} = $pps;
		}
		my $pps = ReadValue_($line, "<TOOLS_PATH_ADDITIONS>");
		if (defined $pps) {
			$configuration {'TOOLS_PATH_ADDITIONS'} = $pps;
		}
		my $pps = ReadValue_($line, "<ARCH>");
		if (defined $pps) {
			$configuration {'ARCH'} = $pps;
		}
		my $pps = ReadValue_($line, "<CMAKE>");
		if (defined $pps) {
			$configuration {'CMAKE'} = $pps;
		}
		my $pps = ReadValue_($line, "<ConfigTags>");
		if (defined $pps) {
			$configuration {'ConfigTags'} = $pps;
		}
		my $pps = ReadValue_($line, "<SanitizerFlags>");
		if (defined $pps) {
			$configuration {'SanitizerFlags'} = $pps;
		}
		my $pps = ReadValue_($line, "<LINKER>");
		if (defined $pps) {
			$configuration {'LINKER'} = $pps;
		}
		my $pps = ReadValue_($line, "<AS>");
		if (defined $pps) {
			$configuration {'AS'} = $pps;
		}
		my $pps = ReadValue_($line, "<CXX>");
		if (defined $pps) {
			$configuration {'CXX'} = $pps;
		}
		my $pps = ReadValue_($line, "<CC>");
		if (defined $pps) {
			$configuration {'CC'} = $pps;
		}
		my $pps = ReadValue_($line, "<HasMakefileBugWorkaround_lto_skipping_undefined_incompatible>");
		if (defined $pps) {
			$configuration {'HasMakefileBugWorkaround_lto_skipping_undefined_incompatible'} = $pps;
		}
		my $pps = ReadValue_($line, "<qCompiler_HasNoMisleadingIndentation_Flag>");
		if (defined $pps) {
			$configuration {'qCompiler_HasNoMisleadingIndentation_Flag'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_boost>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_boost'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_LibCurl>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_LibCurl'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_StrawberryPerl>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_StrawberryPerl'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_OpenSSL>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_OpenSSL'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_OpenSSL_ExtraArgs>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_OpenSSL_ExtraArgs'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_Xerces>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_Xerces'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_libxml2>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_libxml2'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_ZLib>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_ZLib'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_GoogleTest>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_GoogleTest'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_sqlite>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_sqlite'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_LZMA>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_LZMA'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_fmtlib>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_fmtlib'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_WinHTTP>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_WinHTTP'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_WIX>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_WIX'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_ATLMFC>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_ATLMFC'} = $pps;
		}
		my $pps = ReadValue_($line, "<IncludeDebugSymbolsInLibraries>");
		if (defined $pps) {
			$configuration {'IncludeDebugSymbolsInLibraries'} = $pps;
		}
		my $pps = ReadValue_($line, "<IncludeDebugSymbolsInExecutables>");
		if (defined $pps) {
			$configuration {'IncludeDebugSymbolsInExecutables'} = $pps;
		}
		my $pps = ReadValue_($line, "<AssertionsEnabled>");
		if (defined $pps) {
			$configuration {'AssertionsEnabled'} = $pps;
		}
		my $pps = ReadValue_($line, "<CWARNING_FLAGS>");
		if (defined $pps) {
			$configuration {'CWARNING_FLAGS'} = $pps;
		}
		my $pps = ReadValue_($line, "<AR>");
		if (defined $pps) {
			$configuration {'AR'} = $pps;
		}
		my $pps = ReadValue_($line, "<MIDL>");
		if (defined $pps) {
			$configuration {'MIDL'} = $pps;
		}
		my $pps = ReadValue_($line, "<RC>");
		if (defined $pps) {
			$configuration {'RC'} = $pps;
		}
		my $pps = ReadValue_($line, "<LIBTOOL>");
		if (defined $pps) {
			$configuration {'LIBTOOL'} = $pps;
		}
		my $pps = ReadValue_($line, "<RANLIB>");
		if (defined $pps) {
			$configuration {'RANLIB'} = $pps;
		}
		my $pps = ReadValue_($line, "<STRIP>");
		if (defined $pps) {
			$configuration {'STRIP'} = $pps;
		}
		my $pps = ReadValue_($line, "<CPPFLAGS>");
		if (defined $pps) {
			$configuration {'CPPFLAGS'} = $pps;
		}
		my $pps = ReadValue_($line, "<CFLAGS>");
		if (defined $pps) {
			$configuration {'CFLAGS'} = $pps;
		}
		my $pps = ReadValue_($line, "<CXXFLAGS>");
		if (defined $pps) {
			$configuration {'CXXFLAGS'} = $pps;
		}
		my $pps = ReadValue_($line, "<EXTRA_COMPILER_ARGS>");
		if (defined $pps) {
			$configuration {'EXTRA_COMPILER_ARGS'} = $pps;
		}
		my $pps = ReadValue_($line, "<INCLUDES_PATH>");
		if (defined $pps) {
			$configuration {'INCLUDES_PATH'} = $pps;
		}
		my $pps = ReadValue_($line, "<LinkerArgs_LibPath>");
		if (defined $pps) {
			$configuration {'LinkerArgs_LibPath'} = $pps;
		}
		my $pps = ReadValue_($line, "<LinkTime_CopyFilesToEXEDir>");
		if (defined $pps) {
			$configuration {'LinkTime_CopyFilesToEXEDir'} = $pps;
		}
		my $pps = ReadValue_($line, "<LinkerArgs_LibDependencies>");
		if (defined $pps) {
			$configuration {'LinkerArgs_LibDependencies'} = $pps;
		}
		my $pps = ReadValue_($line, "<CrossCompiling>");
		if (defined $pps) {
			$configuration {'CrossCompiling'} = $pps;
		}
		my $pps = ReadValue_($line, "<LinkerArgs_ExtraPrefix>");
		if (defined $pps) {
			$configuration {'LinkerArgs_ExtraPrefix'} = $pps;
		}
		my $pps = ReadValue_($line, "<LinkerArgs_ExtraSuffix>");
		if (defined $pps) {
			$configuration {'LinkerArgs_ExtraSuffix'} = $pps;
		}
		my $pps = ReadValue_($line, "<RunPrefix>");
		if (defined $pps) {
			$configuration {'RunPrefix'} = $pps;
		}
		my $pps = ReadValue_($line, "<qCompiler_ValgrindLTO_Buggy>");
		if (defined $pps) {
			$configuration {'qCompiler_ValgrindLTO_Buggy'} = $pps;
		}
		my $pps = ReadValue_($line, "<qCompiler_ValgrindDirectSignalHandler_Buggy>");
		if (defined $pps) {
			$configuration {'qCompiler_ValgrindDirectSignalHandler_Buggy'} = $pps;
		}
		my $pps = ReadValue_($line, "<MakeDefine>");
		if (defined $pps) {
			push (@useExtraMakeDefines, $pps);
		}
		my $pps = ReadValue_($line, "<PkgConfigNames>");
		if (defined $pps) {
			$configuration {'PkgConfigNames'} = $pps;
		}
	}
	$configuration {'ExtraMakeDefines'} = \@useExtraMakeDefines;
}




### PUBLIC APIS
sub	GetBuildPlatform {
    my $configName =  $_[0];
	return GetConfigurationParameter($configName, 'BuildPlatform');
}

sub	GetBuildPlatformIfAny {
    my $configName =  $_[0];
	return GetConfigurationParameter($configName, 'BuildPlatform');
}

my $lastReadConfig	=	null;

sub	GetConfigurationParameter {
    my $configName =  $_[0];
	#print ("ENTERING GetConfigurationParameter (configName = $configName)\n");

	if ($fileErr ne "") {
		print ("Cannot GetConfigurationParameter: $fileErr");
		exit (1);
	}

	if ($lastReadConfig ne $configName) {
		@useExtraMakeDefines = ();
		@useExtraMakeDefines = ();
		%configuration = ();

		$masterXMLConfigFile	=	"$configurationFiles/$configName.xml";

		ReadConfigFile_ ();
		$lastReadConfig = $configName;
	}

	my $paramName = $_[1];
	
	# crazy hack cuz storing arrays into a hash screws them up in perl. What a terrible
	# language  choice!
	if ($paramName eq "ExtraMakeDefines") {
		return @useExtraMakeDefines;
	}
	
	my $result = $configuration{$paramName};

	# aliases
	if ($result eq "") {
		if ($paramName eq "CompilerDriver-C") {
			## ALIAS DEPRECATED
			return GetConfigurationParameter ($configName, "CC");
		}
		if ($paramName eq "CompilerDriver-C++") {
			## ALIAS DEPRECATED
			return GetConfigurationParameter ($configName, "CXX");
		}
	}

	#default values
	if ($result eq "") {
		if ($paramName eq "LIB_SUFFIX") {
			my $BuildPlatform = GetConfigurationParameter($configName, "BuildPlatform");
			if ($BuildPlatform eq "Unix") {
				return ".a";
			}
			elsif ($BuildPlatform =~ /^VisualStudio.Net/) {
				return ".lib";
			}
		}
		if ($paramName eq "OBJ_SUFFIX") {
			my $BuildPlatform = GetConfigurationParameter($configName, "BuildPlatform");
			if ($BuildPlatform eq "Unix") {
				return ".o";
			}
			elsif ($BuildPlatform =~ /^VisualStudio.Net/) {
				return ".obj";
			}
		}
		if ($paramName eq "EXE_SUFFIX") {
			my $BuildPlatform = GetConfigurationParameter($configName, "BuildPlatform");
			if ($BuildPlatform eq "Unix") {
				return "";
			}
			elsif ($BuildPlatform =~ /^VisualStudio.Net/) {
				return ".exe";
			}
		}
	}


	#print ("RETURNING paramname=$paramName: $configuration{$paramName}\n");
	return $result;
}

1