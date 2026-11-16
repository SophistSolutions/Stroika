"""
The host OS a Stroika build runs on, and the names the build's scripts test it with - the one definition of what each
name means. Python scripts import this; sh and make use Build/Scripts/HostOSMatches, which runs it.

The names, matched ignoring case:
    MSYS, Cygwin, Linux, Darwin, ...    the host's DETECTED_HOST_OS - Build/Scripts/DetectedHostOS's names, which are
                                        CMake's CMAKE_HOST_SYSTEM_NAME (uname -s), but MSYS for MINGW too
    Windows                             MSYS or Cygwin: Windows runs the build, whichever POSIX layer the shell is
    Unix                                any other

Windows and Unix are not CMake's CMAKE_HOST_WIN32 and CMAKE_HOST_UNIX, which put Cygwin with UNIX: those say what kind of
program a cmake builds (Cygwin's own cmake builds Cygwin programs), while Stroika runs MSVC from either shell, building the
same Windows programs.

Build/Scripts/DetectedHostOS (sh) stays the one definition of the host's own name - DetectedHostOS () below runs it - as
it must work before python3 is known to be there: it is how Build/Scripts/CheckPrerequisiteTools, and
GetMessageForMissingTool's hint for installing python3, know the host.
"""

import os
import subprocess

_DETECTED_HOST_OS_SCRIPT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "Scripts",
                                                         "DetectedHostOS"))

_WINDOWS_SHELLS = ("MSYS", "Cygwin")

_detectedHostOS = None


def DetectedHostOS():
    """
    The host's own name: DETECTED_HOST_OS from the environment (make exports it; a caller may set it, as for
    Build/Scripts/MakeDefaultConfigurations), else Build/Scripts/DetectedHostOS's answer
    """
    global _detectedHostOS
    if _detectedHostOS is None:
        _detectedHostOS = os.environ.get("DETECTED_HOST_OS") or \
            subprocess.run([_DETECTED_HOST_OS_SCRIPT], stdout=subprocess.PIPE, text=True, check=True).stdout.strip()
    return _detectedHostOS


def Names():
    """The names the host matches: its own, then its group - eg ["MSYS", "Windows"], ["Linux", "Unix"]"""
    hostOS = DetectedHostOS()
    return [hostOS, "Windows" if hostOS in _WINDOWS_SHELLS else "Unix"]


def Matches(*names):
    """Is the host any of names (ignoring case)?"""
    mine = [n.lower() for n in Names()]
    return any(n.lower() in mine for n in names)
