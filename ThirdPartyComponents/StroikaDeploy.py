"""
Conan's deployer (conan install --deployer) for Stroika's third-party components (conanfile.py): copies each package into the
deployer folder - the configuration's Builds directory - with the directory it was installed to (stroika-install-prefix.txt)
replaced, in each text file, by that folder. A file already there with the same contents is left alone, keeping its time, so
nothing built from it is built again. Not the tools a component's build runs (StrawberryPerl): only what is built into
Stroika, the host context's packages.

CopyPackage does the copying, for conanfile.py too - which copies the components one is built on into its install directory.

A component's debug information names where its sources (src/<directory>, which conanfile.py packages) and the headers it
includes will be once deployed, by DEPLOY_DIR_PLACEHOLDER standing for the deployer folder (StroikaThirdPartyComponent.cmake).
Deploying writes the folder over it, in each binary file - padded with separators to the placeholder's length, so nothing
else in the file moves - and a debugger finds them, with nothing set up. A folder longer than the placeholder is not written
(a warning says so), nor where a file holds it in a form not written plainly - as gcc's LTO intermediate code, compressed.

A binary file naming the install directory - a path compiled in, as a component's default directory for its configuration
files or modules might be (its debug information names DEPLOY_DIR_PLACEHOLDER instead) - cannot be changed so: it is copied
as it is, and named in a warning.
"""

import hashlib
import os
import re
import shutil

INSTALL_PREFIX_FILE = "stroika-install-prefix.txt"
_CONAN_FILES = ("conaninfo.txt", "conanmanifest.txt", INSTALL_PREFIX_FILE)

# The deployer folder, as a component's debug information names it: the same on every machine, so that a package built on one
# deploys to any. Long enough for the folder; and short enough that on Windows a path naming it - the component's deepest
# source after it - stays under MAX_PATH (260), written over or not.
DEPLOY_DIR_PLACEHOLDER = "/StroikaDeployDirectory".ljust(128, "_")


def Digest(data):
    """A digest of a file's contents, as CopyPackage returns it"""
    return hashlib.sha256(data).hexdigest()


def CopyPackage(package, target, output, deploying=True):
    """
    The package - a folder Conan's cache holds, of one of Stroika's components - copied into the folder target, the directory
    it was installed to replaced by target in each text file; a file already there with the same contents left alone. When
    deploying, DEPLOY_DIR_PLACEHOLDER is replaced by target in each binary file too, as the debug information names it - not
    when copying a package into the install directory of a component built on it (conanfile.py), whose programs, linking
    its libraries, take their debug information as it is, to be deployed with them.
    Returns the relative path of each file, with a digest of its contents as copied.
    """
    target = target.replace("\\", "/").rstrip("/")
    with open(os.path.join(package, INSTALL_PREFIX_FILE)) as f:
        prefix = f.read().strip().rstrip("/")
    # as CMake writes paths (/), and as a Windows program may (\) - there in any letter case, as Windows compares paths: CMake
    # writes a path as the file system spells it (C:/stroika/.conan-home), which need not be as Conan was given it (CONAN_HOME
    # c:/Stroika/.conan-home)
    flags = re.IGNORECASE if os.sep == "\\" else 0
    replacements = [(re.compile(re.escape(prefix.encode()), flags), target.encode())]
    if os.sep == "\\":
        replacements.append((re.compile(re.escape(prefix.replace("/", "\\").encode()), flags), target.replace("/", "\\").encode()))
    # the placeholder - on Windows also as MSVC's debug information writes it, its / a \ (StroikaThirdPartyComponent.cmake) -
    # and its replacement in a binary file: target, with the platform's separators, and more of them after it to the
    # placeholder's length - /a/b////src/x.c is /a/b/src/x.c
    placeholders = [DEPLOY_DIR_PLACEHOLDER.encode()]
    if os.sep == "\\":
        placeholders.append(DEPLOY_DIR_PLACEHOLDER.replace("/", "\\").encode())
    nativeTarget = target.replace("/", os.sep)
    placeholderReplacement = None
    if len(nativeTarget) <= len(DEPLOY_DIR_PLACEHOLDER):
        placeholderReplacement = nativeTarget.ljust(len(DEPLOY_DIR_PLACEHOLDER), os.sep).encode()
    copied = {}
    for directory, _, files in os.walk(package):
        for name in files:
            source = os.path.join(directory, name)
            relative = os.path.relpath(source, package)
            if relative in _CONAN_FILES:
                continue
            with open(source, "rb") as f:
                data = f.read()
            for old, new in replacements:
                found = old.search(data)
                if found:
                    if b"\0" in data:
                        output.warning(f"{relative} names the directory it was installed to, {found[0].decode()}, and is binary: copied unchanged")
                    else:
                        data = old.sub(lambda _: new, data)
            for placeholder in placeholders if deploying else []:
                if placeholder in data:
                    if b"\0" not in data:
                        data = data.replace(placeholder, target.encode())
                    elif placeholderReplacement:
                        data = data.replace(placeholder, placeholderReplacement)
                    else:
                        output.warning(f"{relative}: its debug information names its sources' directory as {DEPLOY_DIR_PLACEHOLDER}, not {target} - longer than that")
            copied[relative] = Digest(data)
            destination = os.path.join(target, relative)
            if os.path.isfile(destination):
                with open(destination, "rb") as f:
                    if f.read() == data:
                        continue
            os.makedirs(os.path.dirname(destination), exist_ok=True)
            with open(destination, "wb") as f:
                f.write(data)
            shutil.copymode(source, destination)
    return copied


# For a program whose debug information names the components' sources by DEPLOY_DIR_PLACEHOLDER still (gcc's LTO keeps them
# in its compressed code, which deploying cannot write over): files mapping it to the deployer folder, for a debugger to read
# - gdb: source <folder>/ThirdPartySources.gdb; lldb: command source <folder>/ThirdPartySources.lldb
_DEBUGGER_FILES = {
    "ThirdPartySources.gdb": "set substitute-path {placeholder} {folder}\n",
    "ThirdPartySources.lldb": "settings set target.source-map {placeholder} {folder}\n",
}


def deploy(graph, output_folder, **kwargs):
    for node in graph.nodes[1:]:  # [0] is the consumer, the command line
        if node.context == "host":
            CopyPackage(node.conanfile.package_folder, output_folder, node.conanfile.output)
    folder = output_folder.replace("\\", "/").rstrip("/")
    for name, line in _DEBUGGER_FILES.items():
        text = line.format(placeholder=DEPLOY_DIR_PLACEHOLDER, folder=folder)
        path = os.path.join(output_folder, name)
        existing = None
        if os.path.isfile(path):
            with open(path) as f:
                existing = f.read()
        if existing != text:
            with open(path, "w") as f:
                f.write(text)
