"""
Reads a configuration - ConfigurationFiles/<name>.xml, as configure writes it: each element of <Configuration> is a
parameter, its text the value (unescaped, as for any XML), and <ExtraMakeDefines> the list of its <MakeDefine>s.

Python 3.9 or later (the oldest a supported build host has).
"""

import os
import re
import sys
import xml.etree.ElementTree as ET

if sys.platform == "win32":  # (run as python3 SCRIPT, say - each script's #! line names a POSIX one)
    sys.exit("Stroika's build scripts need a POSIX python3 - on Windows, MSYS2's (pacman -S python) or Cygwin's - not a native Windows one")

_THIS_DIR = os.path.dirname(os.path.abspath(__file__))
_CONFIGURATION_FILES = os.path.normpath(os.path.join(_THIS_DIR, "..", "..", "..", "ConfigurationFiles"))

# Bytes are decoded as latin-1, which maps each byte to one character and back, so values round-trip exactly - the file
# names no encoding, and what is written from its values (Configuration.mk, say) is encoded the same way
ENCODING = "latin-1"


class _State:
    lastReadConfig = None
    configuration = {}
    extraMakeDefines = []


def _ReadConfigFile(path):
    try:
        with open(path, "rb") as f:
            data = f.read().decode(ENCODING)
    except OSError:
        sys.stderr.write("Cannot GetConfigurationParameter: Unable to open " + path + "\n")
        sys.exit(1)
    try:
        root = ET.fromstring(data)
    except ET.ParseError as e:
        sys.stderr.write("Cannot read " + path + " (" + str(e) + "): re-run configure for it (make reconfigure)\n")
        sys.exit(1)
    for element in root:
        if element.tag == "ExtraMakeDefines":
            _State.extraMakeDefines = [d.text or "" for d in element if d.tag == "MakeDefine"]
        else:
            _State.configuration[element.tag] = element.text or ""


def GetConfigurationParameter(configName, paramName):
    """
    The value of paramName in configuration configName, as a str ("" if unset) - or, for "ExtraMakeDefines", a list of
    them. Exits the process (status 1, after printing why to stderr) if that configuration's file cannot be opened, or is not
    well-formed XML.
    """
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
