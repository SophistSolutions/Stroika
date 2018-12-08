#!/usr/bin/perl
#use strict;
#use warnings;

### GIVE UP ON PERL - CANNOT EASILY INSTALL XML READER - SO JUST USE 
### tesxt reading..

BEGIN{ @INC = ( "./", @INC ); }

my $intermediateFiles	=	GetThisScriptDir() . "/../IntermediateFiles";
my $configurationFiles	=	GetThisScriptDir() . "/../ConfigurationFiles";
my $masterXMLConfigFile	=	"";

require (GetThisScriptDir () + "StringUtils.pl");

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
		my $pps = ReadValue_($line, "<ProjectPlatformSubdir>");
		if (defined $pps) {
			$configuration {'ProjectPlatformSubdir'} = $pps;
		}
		my $pps = ReadValue_($line, "<ARCH>");
		if (defined $pps) {
			$configuration {'ARCH'} = $pps;
		}
		my $pps = ReadValue_($line, "<ConfigTags>");
		if (defined $pps) {
			$configuration {'ConfigTags'} = $pps;
		}
		#my $pps = ReadValue_($line, "<Platform>");
		#if (defined $pps) {
		#	$configuration {'Platform'} = $pps;
		#}
		my $pps = ReadValue_($line, "<OptimizerFlag>");
		if (defined $pps) {
			$configuration {'OptimizerFlag'} = $pps;
		}
		my $pps = ReadValue_($line, "<CompilerDriver-C++>");
		if (defined $pps) {
			$configuration {'CompilerDriver-C++'} = $pps;
		}
		my $pps = ReadValue_($line, "<CompilerDriver-C>");
		if (defined $pps) {
			$configuration {'CompilerDriver-C'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_boost>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_boost'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_LibCurl>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_LibCurl'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_ActivePerl>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_ActivePerl'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_PrivateOverrideOfCMake>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_PrivateOverrideOfCMake'} = $pps;
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
		my $pps = ReadValue_($line, "<qFeatureFlag_ZLib>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_ZLib'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_sqlite>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_sqlite'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_LZMA>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_LZMA'} = $pps;
		}
		my $pps = ReadValue_($line, "<qFeatureFlag_WinHTTP>");
		if (defined $pps) {
			$configuration {'qFeatureFlag_WinHTTP'} = $pps;
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
		my $pps = ReadValue_($line, "<ENABLE_TRACE2FILE>");
		if (defined $pps) {
			$configuration {'ENABLE_TRACE2FILE'} = $pps;
		}
		my $pps = ReadValue_($line, "<ENABLE_ASSERTIONS>");
		if (defined $pps) {
			$configuration {'ENABLE_ASSERTIONS'} = $pps;
		}
		my $pps = ReadValue_($line, "<ENABLE_GLIBCXX_DEBUG>");
		if (defined $pps) {
			$configuration {'ENABLE_GLIBCXX_DEBUG'} = $pps;
		}
		my $pps = ReadValue_($line, "<CWARNING_FLAGS>");
		if (defined $pps) {
			$configuration {'CWARNING_FLAGS'} = $pps;
		}
		my $pps = ReadValue_($line, "<AR>");
		if (defined $pps) {
			$configuration {'AR'} = $pps;
		}
		my $pps = ReadValue_($line, "<RANLIB>");
		if (defined $pps) {
			$configuration {'RANLIB'} = $pps;
		}
		my $pps = ReadValue_($line, "<STRIP>");
		if (defined $pps) {
			$configuration {'STRIP'} = $pps;
		}
		my $pps = ReadValue_($line, "<EXTRA_COMPILER_ARGS>");
		if (defined $pps) {
			$configuration {'EXTRA_COMPILER_ARGS'} = $pps;
		}
		my $pps = ReadValue_($line, "<INCLUDES_PATH>");
		if (defined $pps) {
			$configuration {'INCLUDES_PATH'} = $pps;
		}
		my $pps = ReadValue_($line, "<LIBS_PATH>");
		if (defined $pps) {
			$configuration {'LIBS_PATH'} = $pps;
		}
		my $pps = ReadValue_($line, "<LIB_DEPENDENCIES>");
		if (defined $pps) {
			$configuration {'LIB_DEPENDENCIES'} = $pps;
		}
		my $pps = ReadValue_($line, "<CrossCompiling>");
		if (defined $pps) {
			$configuration {'CrossCompiling'} = $pps;
		}
		my $pps = ReadValue_($line, "<EXTRA_PREFIX_LINKER_ARGS>");
		if (defined $pps) {
			$configuration {'EXTRA_PREFIX_LINKER_ARGS'} = $pps;
		}
		my $pps = ReadValue_($line, "<EXTRA_SUFFIX_LINKER_ARGS>");
		if (defined $pps) {
			$configuration {'EXTRA_SUFFIX_LINKER_ARGS'} = $pps;
		}
		my $pps = ReadValue_($line, "<RUN_PREFIX>");
		if (defined $pps) {
			$configuration {'RUN_PREFIX'} = $pps;
		}
		my $pps = ReadValue_($line, "<CDefine>");
		if (defined $pps) {
			$useExtraCDefines[@useExtraCDefines] = $pps;
		}
		my $pps = ReadValue_($line, "<MakeDefine>");
		if (defined $pps) {
			push (@useExtraMakeDefines, $pps);
		}
	}
	$configuration {'ExtraCDefines'} = \@useExtraCDefines;
	$configuration {'ExtraMakeDefines'} = \@useExtraMakeDefines;
}




### PUBLIC APIS
sub	GetProjectPlatformSubdir {
    my $configName =  $_[0];
	return GetConfigurationParameter($configName, 'ProjectPlatformSubdir');
}

sub	GetProjectPlatformSubdirIfAny {
    my $configName =  $_[0];
	return GetConfigurationParameter($configName, 'ProjectPlatformSubdir');
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
		@useExtraCDefines = ();
		@useExtraMakeDefines = ();
		%configuration = ();

		$masterXMLConfigFile	=	"$configurationFiles/$configName.xml";

		ReadConfigFile_ ();
		$lastReadConfig = $configName;
	}

	my $paramName = $_[1];
	
	# crazy hack cuz storing arrays into a hash screws them up in perl. What a terrible
	# language  choice!
	if ($paramName eq "ExtraCDefines") {
		return @useExtraCDefines;
	}
	if ($paramName eq "ExtraMakeDefines") {
		return @useExtraMakeDefines;
	}
	
	my $result = $configuration{$paramName};
	if ($result eq "") {
		if (index ($configuration{"ProjectPlatformSubdir"}, "VisualStudio") != -1) {
			my $script = GetThisScriptDir() . "/PrintEnvVarFromCommonBuildVars.pl";
			if ($paramName eq "CompilerDriver-C") {
				return `$script "$configName" CC`;
			}
			if ($paramName eq "CompilerDriver-C++") {
				return `$script "$configName" CC`;
			}
			if ($paramName eq "AR") {
				return `$script "$configName" AR`;
			}
			if ($paramName eq "AS") {
				return `$script "$configName" AS`;
			}
		}
	}

	# aliases
	if ($result eq "") {
		if ($paramName eq "CompilerDriver-AS") {
			return GetConfigurationParameter ($configName, $paramName);
		}
	}

	#print ("RETURNING paramname=$paramName: $configuration{$paramName}\n");
	return $result;
}

1