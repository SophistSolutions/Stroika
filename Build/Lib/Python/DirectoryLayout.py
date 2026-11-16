"""
The directory layout of a Stroika build - the Python side of the build root's DirectoryLayout.mk. The directories are
documented at the top of Build/Lib/Make/SharedMakeVariables-Default.mk.

The build root is StroikaIntermediateFilesRoot: the environment variable (make exports it), else TOP_ROOT's (the
environment's, else the Stroika source tree's) IntermediateFiles/, as in make. Build/Scripts/MakeBuildRoot makes one, and
nothing else does, writing its DirectoryLayout.mk, from which BUILDS_ROOT and CONFIGURATION_FILES_DIR are read, when first
used.

Each directory is an absolute path ending in "/" - on Windows, written C:/... (cygpath --mixed), which Cygwin and MSYS both
read, and native Windows tools too.
"""

import os
import re
import shutil
import subprocess
import sys

STROIKA_SOURCE_ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..")) + "/"

DIRECTORY_LAYOUT_FILE_NAME = "DirectoryLayout.mk"

# The first line of the Makefile MakeBuildRoot writes in a build root - so it never overwrites one it did not write
_MAKEFILE_FIRST_LINE = "# Written by Stroika's Build/Scripts/MakeBuildRoot"


def _IsAbsolute(path):
    """
    os.path.isabs, but on Windows taking C:/... as absolute too - as the Cygwin and MSYS Pythons 3.12 do, but not Cygwin's
    3.9 or MSYS's 3.14 (as of 3.14.10): their posixpath sees a relative path, which os.path.abspath puts under the
    current directory
    """
    return os.path.isabs(path) or (sys.platform in ("cygwin", "msys") and re.match("[A-Za-z]:/", path) is not None)


def _CheckDir(name, value):
    if not (_IsAbsolute(value) and value.endswith("/")):  # as SharedMakeVariables-Default.mk requires
        sys.exit(name + " must be an absolute path ending in / (got \"" + value + "\")")
    return value


def AbsoluteDir(path):
    """path as the build writes it: absolute, ending in /, and on Windows (Cygwin and MSYS python report "cygwin") C:/..."""
    path = os.path.normpath(path) if _IsAbsolute(path) else os.path.abspath(path)
    if sys.platform in ("cygwin", "msys"):
        path = subprocess.run(["cygpath", "--mixed", path], stdout=subprocess.PIPE, text=True, check=True).stdout.strip()
    return path.rstrip("/") + "/"


def ShellPath(path):
    """
    path as the shell spells it: on Windows /c/... (MSYS) or /cygdrive/c/... (Cygwin), not C:/... - for a :-separated list
    such as PKG_CONFIG_PATH or PATH, which cannot hold a drive letter's colon
    """
    if sys.platform in ("cygwin", "msys"):
        return subprocess.run(["cygpath", "--unix", path], stdout=subprocess.PIPE, text=True, check=True).stdout.strip()
    return path


INTERMEDIATE_FILES_ROOT = _CheckDir("StroikaIntermediateFilesRoot",
                                    os.environ.get("StroikaIntermediateFilesRoot",
                                                   os.environ.get("TOP_ROOT", STROIKA_SOURCE_ROOT) + "IntermediateFiles/"))


def ReadDirectoryLayout(intermediateFilesRoot):
    """The build root's DirectoryLayout.mk, as a dict - None if it has none"""
    try:
        with open(intermediateFilesRoot + DIRECTORY_LAYOUT_FILE_NAME) as f:
            lines = f.read().splitlines()
    except FileNotFoundError:
        return None
    return dict(line.split("=", 1) for line in lines if line.strip() != "" and not line.startswith("#"))


def _MakeBuildRootCommand():
    """(TOP_ROOT, the MakeBuildRoot command line, run there, that makes the build root INTERMEDIATE_FILES_ROOT) - as make's"""
    topRoot = AbsoluteDir(os.environ.get("TOP_ROOT", STROIKA_SOURCE_ROOT))
    sourceRoot = AbsoluteDir(STROIKA_SOURCE_ROOT)
    command = (sourceRoot[len(topRoot):] if sourceRoot.startswith(topRoot) else sourceRoot) + "Build/Scripts/MakeBuildRoot"
    if topRoot != sourceRoot:
        command += " --top-root ."
    if AbsoluteDir(INTERMEDIATE_FILES_ROOT) != topRoot + "IntermediateFiles/":
        command += " --intermediate-files-root " + INTERMEDIATE_FILES_ROOT
    return (topRoot, command)


_layout = None


def _Layout():
    global _layout
    if _layout is None:
        layout = ReadDirectoryLayout(INTERMEDIATE_FILES_ROOT)
        if layout is None:
            (topRoot, command) = _MakeBuildRootCommand()
            sys.exit("No build root at " + INTERMEDIATE_FILES_ROOT + " (no " + DIRECTORY_LAYOUT_FILE_NAME + "). To make it, run in " +
                     topRoot + ":\n    " + command)
        for name in ("StroikaBuildsRoot", "StroikaConfigurationFilesDir"):
            given = os.environ.get(name)  # as SharedMakeVariables-Default.mk: the build root says, not the caller
            if given is not None and given != layout[name]:
                sys.exit(name + " is " + given + " here, but " + INTERMEDIATE_FILES_ROOT + DIRECTORY_LAYOUT_FILE_NAME +
                         " says " + layout[name])
        _layout = layout
    return _layout


def __getattr__(name):
    """BUILDS_ROOT and CONFIGURATION_FILES_DIR, read from the build root only when first used - so importing this reads nothing"""
    if name == "BUILDS_ROOT":
        return _Layout()["StroikaBuildsRoot"]
    if name == "CONFIGURATION_FILES_DIR":
        return _Layout()["StroikaConfigurationFilesDir"]
    raise AttributeError(name)


def _WriteFile(path, text):
    # through a temporary and a rename, so no reader ever sees half of it
    tmp = path + ".tmp." + str(os.getpid())
    with open(tmp, "w", newline="\n") as f:
        f.write(text)
    os.replace(tmp, path)


def _CopyOldConfigurations(configurationFilesDir, intermediateFilesRoot):
    """
    Before Stroika v3.0d26, the configurations were in ConfigurationFiles/ beside IntermediateFiles/: copy any there in.
    Copied with a new time, so each configuration's Configuration.mk - which may name the old variables - is re-made.
    """
    old = os.path.dirname(intermediateFilesRoot.rstrip("/")) + "/ConfigurationFiles/"
    copied = 0
    if os.path.isdir(old):
        for f in sorted(os.listdir(old)):
            if f.endswith(".xml") and not os.path.exists(configurationFilesDir + f):
                shutil.copy(old + f, configurationFilesDir + f)
                copied += 1
    return (old, copied)


def MakeBuildRoot(intermediateFilesRoot, buildsRoot=None, topRoot=None):
    """
    Make intermediateFilesRoot a build root: its ConfigurationFiles/, its Makefile (make run there builds topRoot - by
    default this Stroika - into it), and last its DirectoryLayout.mk (StroikaBuildsRoot: buildsRoot, by default Builds/
    beside it). Each argument may be relative. One that is a build root already is left as it is - though a buildsRoot or
    topRoot given must match it.
    """
    intermediateFilesRoot = AbsoluteDir(intermediateFilesRoot)
    buildsRoot = AbsoluteDir(buildsRoot) if buildsRoot else None
    topRoot = AbsoluteDir(topRoot or STROIKA_SOURCE_ROOT)
    layout = ReadDirectoryLayout(intermediateFilesRoot)
    if layout is not None:
        for name, given in (("StroikaBuildsRoot", buildsRoot), ("StroikaDirectoryLayoutTopRoot", topRoot)):
            if given is not None and given != layout.get(name):
                sys.exit(intermediateFilesRoot + " is a build root already, with " + name + "=" + str(layout.get(name)) +
                         ", not " + given + ": to change that, delete it first")
        return
    buildsRoot = buildsRoot or AbsoluteDir(os.path.dirname(intermediateFilesRoot.rstrip("/")) + "/Builds/")
    configurationFilesDir = intermediateFilesRoot + "ConfigurationFiles/"
    makefile = intermediateFilesRoot + "Makefile"
    if os.path.exists(makefile):
        with open(makefile) as f:
            if f.readline().rstrip("\n") != _MAKEFILE_FIRST_LINE:
                sys.exit(intermediateFilesRoot + " has a Makefile MakeBuildRoot did not write: is it a source directory?")
    os.makedirs(configurationFilesDir, exist_ok=True)
    (oldConfigurationsDir, copied) = _CopyOldConfigurations(configurationFilesDir, intermediateFilesRoot)
    _WriteFile(makefile,
               _MAKEFILE_FIRST_LINE + "\n"
               "# make run here builds " + topRoot + " into this build root\n"
               "export StroikaIntermediateFilesRoot := " + intermediateFilesRoot + "\n"
               ".DEFAULT_GOAL := all\n"
               "Makefile: ;\n"
               "%::\n"
               "\t@$(MAKE) --no-print-directory --directory " + topRoot + " $@\n")
    # last, as its presence is what says this is a build root
    _WriteFile(intermediateFilesRoot + DIRECTORY_LAYOUT_FILE_NAME,
               "# The directory layout of this Stroika build root - written by Build/Scripts/MakeBuildRoot; read by make\n"
               "# (Build/Lib/Make/SharedMakeVariables-Default.mk), Python (Build/Lib/Python/DirectoryLayout.py) and sh:\n"
               "# NAME=value lines only. StroikaDirectoryLayoutTopRoot is the TOP_ROOT it builds - checked against TOP_ROOT.\n"
               "StroikaIntermediateFilesRoot=" + intermediateFilesRoot + "\n"
               "StroikaConfigurationFilesDir=" + configurationFilesDir + "\n"
               "StroikaBuildsRoot=" + buildsRoot + "\n"
               "StroikaDirectoryLayoutTopRoot=" + topRoot + "\n")
    message = "Made build root " + intermediateFilesRoot + " (StroikaBuildsRoot=" + buildsRoot + ")"
    if copied != 0:
        message += "; copied " + str(copied) + " configurations from " + oldConfigurationsDir + ", no longer used - delete it"
    print(message, file=sys.stderr)


def ForDisplay(path):
    """path as a message shows it - as Build/Scripts/SubstituteBackVariables does: Builds/..., wherever StroikaBuildsRoot is"""
    # INTERMEDIATE_FILES_ROOT too, since run by hand it may spell the build root differently (/c/... not C:/...)
    for outDir, name in ((_Layout()["StroikaBuildsRoot"], "Builds/"), (_Layout()["StroikaIntermediateFilesRoot"], "IntermediateFiles/"),
                         (INTERMEDIATE_FILES_ROOT, "IntermediateFiles/")):
        if path.startswith(outDir):
            return name + path[len(outDir):]
    cwd = os.getcwd() + "/"
    if path.startswith(cwd):
        return path[len(cwd):]
    return path
