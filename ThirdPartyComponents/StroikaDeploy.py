"""
Conan's deployer (conan install --deployer) for Stroika's third-party components (conanfile.py): copies each package into the
deployer folder - the configuration's Builds directory - with the directory it was installed to (stroika-install-prefix.txt)
replaced, in each text file, by that folder. A file already there with the same contents is left alone, keeping its time, so
nothing built from it is built again.

A binary file naming the install directory - OpenSSL's libraries name their OPENSSLDIR - cannot be changed so: it is copied as
it is, and named in a warning.
"""

import os
import shutil

_INSTALL_PREFIX_FILE = "stroika-install-prefix.txt"
_CONAN_FILES = ("conaninfo.txt", "conanmanifest.txt", _INSTALL_PREFIX_FILE)


def deploy(graph, output_folder, **kwargs):
    target = output_folder.replace("\\", "/").rstrip("/").encode()
    for node in graph.nodes[1:]:  # [0] is the consumer, the command line
        package = node.conanfile.package_folder
        with open(os.path.join(package, _INSTALL_PREFIX_FILE)) as f:
            prefix = f.read().strip().rstrip("/")
        # as CMake writes paths (/), and as a Windows program may (\)
        replacements = {prefix.encode(): target}
        if os.sep == "\\":
            replacements[prefix.replace("/", "\\").encode()] = target.replace(b"/", b"\\")
        for directory, _, files in os.walk(package):
            for name in files:
                source = os.path.join(directory, name)
                relative = os.path.relpath(source, package)
                if relative in _CONAN_FILES:
                    continue
                with open(source, "rb") as f:
                    data = f.read()
                for old, new in replacements.items():
                    if old in data:
                        if b"\0" in data:
                            node.conanfile.output.warning(f"{relative} names the directory it was installed to, {old.decode()}, and is binary: copied unchanged")
                        else:
                            data = data.replace(old, new)
                destination = os.path.join(output_folder, relative)
                if os.path.isfile(destination):
                    with open(destination, "rb") as f:
                        if f.read() == data:
                            continue
                os.makedirs(os.path.dirname(destination), exist_ok=True)
                with open(destination, "wb") as f:
                    f.write(data)
                shutil.copymode(source, destination)
