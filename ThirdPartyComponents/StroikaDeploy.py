"""
Conan's deployer (conan install --deployer) for Stroika's third-party components (conanfile.py): copies each package into the
deployer folder - the configuration's Builds directory - with the directory it was installed to (stroika-install-prefix.txt)
replaced, in each text file, by that folder. A file already there with the same contents is left alone, keeping its time, so
nothing built from it is built again. Not the tools a component's build runs (StrawberryPerl): only what is built into
Stroika, the host context's packages.

CopyPackage does the copying, for conanfile.py too - which copies the components one is built on into its install directory.

A binary file naming the install directory - a path compiled in, as a component's default directory for its configuration
files or modules might be (its debug information does not name it: StroikaThirdPartyComponent.cmake) - cannot be changed
so: it is copied as it is, and named in a warning.
"""

import hashlib
import os
import re
import shutil

INSTALL_PREFIX_FILE = "stroika-install-prefix.txt"
_CONAN_FILES = ("conaninfo.txt", "conanmanifest.txt", INSTALL_PREFIX_FILE)


def Digest(data):
    """A digest of a file's contents, as CopyPackage returns it"""
    return hashlib.sha256(data).hexdigest()


def CopyPackage(package, target, output):
    """
    The package - a folder Conan's cache holds, of one of Stroika's components - copied into the folder target, the directory
    it was installed to replaced by target in each text file; a file already there with the same contents left alone.
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


def deploy(graph, output_folder, **kwargs):
    for node in graph.nodes[1:]:  # [0] is the consumer, the command line
        if node.context == "host":
            CopyPackage(node.conanfile.package_folder, output_folder, node.conanfile.output)
