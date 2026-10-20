"""
Reads a configuration - ConfigurationFiles/<name>.xml, as configure writes it - the way Build/Lib/Perl/ConfigurationReader.pl
does, byte for byte: by matching tags in each line, not by parsing XML.

That is deliberate, and the quirks are kept (@see https://github.com/SophistSolutions/Stroika/issues/1240, which keeps the
Perl as a check until the two always agree):
    - a value runs from its tag to the next '<' on that line - or, with none, to the end of the line, newline included;
    - a tag is found anywhere in a line (a substring match), and when it appears on several lines the last one wins;
    - nothing is unescaped (&lt; stays &lt;);
    - the FIRST lookup in a configuration whose file cannot be opened returns "" - only a later lookup reports the error.

Python 3.9 or later (the oldest a supported build host has).
"""

import os
import re
import sys

_THIS_DIR = os.path.dirname(os.path.abspath(__file__))
# (the one difference from the Perl: its "Unable to open" message names a path relative to itself, so it cannot match)
_CONFIGURATION_FILES = os.path.normpath(os.path.join(_THIS_DIR, "..", "..", "..", "ConfigurationFiles"))

# The tags ConfigurationReader.pl reads, each stored under its own name
_TAGS = [
    "ConfigureCommandLine", "BuildPlatform", "TargetPlatforms", "BuildByDefault", "BuildToolsRoot", "TOOLS_PATH_ADDITIONS",
    "ARCH", "CMAKE", "ConfigTags", "SanitizerFlags", "LINKER", "AS", "CXX", "CC", "qCompiler_HasNoMisleadingIndentation_Flag",
    "qFeatureFlag_boost", "qFeatureFlag_LibCurl", "qFeatureFlag_StrawberryPerl", "qFeatureFlag_OpenSSL",
    "qFeatureFlag_OpenSSL_ExtraArgs", "qFeatureFlag_Xerces", "qFeatureFlag_libxml2", "qFeatureFlag_ZLib", "qFeatureFlag_zstd",
    "qFeatureFlag_GoogleTest", "qFeatureFlag_sqlite", "qFeatureFlag_mongocxxdriver", "qFeatureFlag_LZMA", "qFeatureFlag_fmtlib",
    "qFeatureFlag_WinHTTP", "qFeatureFlag_WIX", "qFeatureFlag_ATLMFC", "IncludeDebugSymbolsInLibraries",
    "IncludeDebugSymbolsInExecutables", "AssertionsEnabled", "CWARNING_FLAGS", "AR", "MIDL", "RC", "LIBTOOL", "RANLIB", "STRIP",
    "PLATFORM_CPPFLAGS_NOTINCLUDES", "FOUNDATION_CPPFLAGS_NOTINCLUDES", "PLATFORM_CFLAGS", "PLATFORM_CXXFLAGS",
    "VSVARS_PLATFORM_INCLUDES_PATH", "VSVARS_MSVC_RUNTIME_LIBRARY", "PLATFORM_INCLUDES_PATH", "FOUNDATION_INCLUDES_PATH",
    "Platform_LinkerArgs_LibPath", "LinkTime_CopyFilesToEXEDir", "Platform_LinkerArgs_LibDependencies", "CrossCompiling",
    "Platform_LinkerArgs_ExtraPrefix", "Platform_LinkerArgs_ExtraSuffix", "RunPrefix",
]
# Read too, but kept as a list - one entry per line it appears on
_LIST_TAG = "MakeDefine"
# Read after the others, in the Perl - which only matters if a line holds both it and an earlier tag, and then not at all
_LAST_TAG = "Foundation_PkgConfigNames"

# Bytes are decoded as latin-1, which maps each byte to one character and back, so values round-trip exactly
ENCODING = "latin-1"


def _ReadValue(line, tag):
    start = line.find(tag)
    if start == -1:
        return None
    rest = line[start + len(tag):]
    eos = rest.find("<")
    return rest if eos == -1 else rest[:eos]


class _State:
    lastReadConfig = None
    fileErr = ""
    configuration = {}
    extraMakeDefines = []


def _ReadConfigFile(path):
    try:
        with open(path, "rb") as f:
            data = f.read().decode(ENCODING)
    except OSError:
        _State.fileErr = "Unable to open " + path
        return
    # Perl's <FILE> in list context: lines split after each "\n" (only - not str.splitlines' many separators), each keeping it
    for line in re.split(r"(?<=\n)", data):
        if line == "":
            continue
        for tag in _TAGS:
            v = _ReadValue(line, "<" + tag + ">")
            if v is not None:
                _State.configuration[tag] = v
        v = _ReadValue(line, "<" + _LIST_TAG + ">")
        if v is not None:
            _State.extraMakeDefines.append(v)
        v = _ReadValue(line, "<" + _LAST_TAG + ">")
        if v is not None:
            _State.configuration[_LAST_TAG] = v


def GetConfigurationParameter(configName, paramName):
    """
    The value of paramName in configuration configName, as a str ("" if unset) - or, for "ExtraMakeDefines", a list of
    them. Exits the process (status 1, after printing why) if an EARLIER lookup could not open that configuration's file.
    """
    if _State.fileErr != "":
        sys.stdout.write("Cannot GetConfigurationParameter: " + _State.fileErr)
        sys.stdout.flush()
        sys.exit(1)

    if _State.lastReadConfig != configName:
        _State.extraMakeDefines = []
        _State.configuration = {}
        _ReadConfigFile(_CONFIGURATION_FILES + "/" + configName + ".xml")
        _State.lastReadConfig = configName

    if paramName == "ExtraMakeDefines":
        return list(_State.extraMakeDefines)

    result = _State.configuration.get(paramName, "")

    # aliases (deprecated)
    if result == "":
        if paramName == "CompilerDriver-C":
            return GetConfigurationParameter(configName, "CC")
        if paramName == "CompilerDriver-C++":
            return GetConfigurationParameter(configName, "CXX")

    # defaults
    if result == "" and paramName in ("LIB_SUFFIX", "OBJ_SUFFIX", "EXE_SUFFIX"):
        buildPlatform = GetConfigurationParameter(configName, "BuildPlatform")
        unix, visualStudio = {"LIB_SUFFIX": (".a", ".lib"), "OBJ_SUFFIX": (".o", ".obj"), "EXE_SUFFIX": ("", ".exe")}[paramName]
        if buildPlatform == "Unix":
            return unix
        if re.match(r"VisualStudio.Net", buildPlatform):
            return visualStudio
    return result


def GetBuildPlatform(configName):
    return GetConfigurationParameter(configName, "BuildPlatform")


def GetBuildPlatformIfAny(configName):
    return GetConfigurationParameter(configName, "BuildPlatform")
