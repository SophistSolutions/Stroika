"""
One of Stroika's third-party components, stroika-<directory> (in lower case, as Conan requires): ThirdPartyComponents/
<directory>, built by its own recipe there as ThirdPartyComponents/CMakeLists.txt builds it (StroikaThirdPartyComponent.cmake)
- Conan only keeping what it builds, in its cache, so a configuration builds each component once, and every build tree on the
machine finds it there after that (StroikaComponentFromConan, in ThirdPartyComponents/CMakeLists.txt, which runs Conan).

What identifies a package:
  - its version: its recipe's (set (version_ ...)), given as Conan's --version;
  - its recipe revision: a digest of the files that build it (export_sources) - StroikaThirdPartyComponent.cmake,
    RemoveFiles.cmake and its directory, and StroikaDeploy.py - so editing them builds it again;
  - the option "configuration": a digest of the configuration's toolchain file and Stroika settings. No settings: the
    compilers and flags are the configuration's;
  - the options "depends" and "tools": the components it is built on, and the tools its build runs (StroikaComponent's
    DEPENDS and TOOLS, in ThirdPartyComponents/CMakeLists.txt), each "stroika-<directory>/<version>", comma-separated - and
    each of those components' packages: built on another, it is built again.
Where the configuration and the build are - its toolchain file, the archives, cmake and its generator - are conf values
(user.stroika:...), not part of a package's identity.

Built in Conan's cache, it is installed into a directory there - which first holds the components it is built on, and the
tools its build runs (as the configuration's Builds directory does, for a component built there), and gives its package
just what its own install adds - packaged with that directory's path (stroika-install-prefix.txt), which StroikaDeploy.py,
copying the package into a configuration's Builds directory, replaces in each text file by that directory. With the
configuration's STROIKA_THIRD_PARTY_SOURCES, the package carries the component's sources too (src/<directory>), for its debug
information to name once deployed (StroikaThirdPartyComponent.cmake).
"""

import json
import os
import shutil

from conan import ConanFile
from conan.tools.files import copy, save

from StroikaDeploy import DEPLOY_DIR_PLACEHOLDER, INSTALL_PREFIX_FILE, CopyPackage, Digest

_STAGED_FILE = "stroika-staged.json"


class StroikaThirdPartyComponent(ConanFile):
    options = {"configuration": ["ANY"], "depends": ["ANY"], "tools": ["ANY"]}
    default_options = {"depends": "", "tools": ""}
    exports = ("StroikaDeploy.py",)

    def _directory(self, folder):
        # its directory in folder: the package's name is stroika-<directory> in lower case, as Conan requires of names
        name = self.name[len("stroika-"):]
        return next(d for d in os.listdir(folder) if d.lower() == name and os.path.isdir(os.path.join(folder, d)))

    @property
    def _installPrefix(self):
        return os.path.join(self.build_folder, "install").replace("\\", "/")

    def export_sources(self):
        # what builds it: StroikaThirdPartyComponent.cmake, RemoveFiles.cmake, and its directory - less make's own build of
        # it (Makefile-Legacy's: its Makefile, and the sources that extracts, CURRENT and ORIGS)
        copy(self, "StroikaThirdPartyComponent.cmake", self.recipe_folder, self.export_sources_folder)
        copy(self, "RemoveFiles.cmake", self.recipe_folder, self.export_sources_folder)
        directory = self._directory(self.recipe_folder)
        copy(self, "*", os.path.join(self.recipe_folder, directory), os.path.join(self.export_sources_folder, directory),
             excludes=("Makefile", "CURRENT*", "ORIGS*", "ORIGs*"))

    def requirements(self):
        for reference in filter(None, str(self.options.depends).split(",")):
            self.requires(reference)

    def build_requirements(self):
        for reference in filter(None, str(self.options.tools).split(",")):
            self.tool_requires(reference)

    def package_id(self):
        # the packages of the components it is built on, not just their versions: their headers and libraries are built in
        self.info.requires.full_mode()

    def build(self):
        # the components it is built on, and the tools its build runs, in its install directory, where its build finds them
        staged = {}
        for _, dependency in self.dependencies.items():
            staged.update(CopyPackage(dependency.package_folder, self._installPrefix, self.output, deploying=False))
        save(self, os.path.join(self.build_folder, _STAGED_FILE), json.dumps(staged))
        # as ThirdPartyComponents/CMakeLists.txt builds it, with that build's cmake and generator (STROIKA_CONAN_CONF)
        def conf(name):
            return self.conf.get("user.stroika:" + name, check_type=str)
        cmake = conf("cmake")
        generator = f'-G "{conf("generator")}"'
        for option, name in (("-A", "generator_platform"), ("-T", "generator_toolset")):
            if conf(name):
                generator += f' {option} "{conf(name)}"'
        for name in ("generator_instance", "make_program"):
            if conf(name):
                generator += f' "-DCMAKE_{name.upper()}={conf(name)}"'
        # a project of just this component, as ThirdPartyComponents/CMakeLists.txt adds each
        save(self, os.path.join(self.source_folder, "CMakeLists.txt"),
             "cmake_minimum_required (VERSION 3.22)\n"
             f"project ({self.name} LANGUAGES NONE)\n"
             'include ("${CMAKE_CURRENT_SOURCE_DIR}/StroikaThirdPartyComponent.cmake")\n'
             f"add_subdirectory ({self._directory(self.source_folder)})\n")
        build = os.path.join(self.build_folder, "build").replace("\\", "/")
        # --no-warn-unused-cli: every component is given these, and not every one uses them all (StrawberryPerl and WIX map
        # no paths; MSVC's builds take no placeholder - StroikaThirdPartyComponent.cmake)
        self.run(f'"{cmake}" {generator} -S "{self.source_folder}" -B "{build}" --no-warn-unused-cli'
                 f' "-DCMAKE_TOOLCHAIN_FILE={conf("toolchain_file")}" "-DSTROIKA_ORIGS_CACHE={conf("origs_cache")}"'
                 f' "-DSTROIKA_THIRD_PARTY_PREFIX={self._installPrefix}/" "-DSTROIKA_THIRD_PARTY_DEPLOY_PLACEHOLDER={DEPLOY_DIR_PLACEHOLDER}"')
        # a multi-configuration generator's (Visual Studio's) build type, and its processors (msbuild -m), as the top
        # CMakeLists.txt builds the components
        self.run(f'"{cmake}" --build "{build}"' + (f' --config {conf("build_config")} --parallel' if conf("build_config") else ""))

    def package(self):
        # what its install added: each file but those of the components it is built on, and the tools, as they were copied in
        with open(os.path.join(self.build_folder, _STAGED_FILE)) as f:
            staged = json.load(f)
        for directory, _, files in os.walk(self._installPrefix):
            for name in files:
                source = os.path.join(directory, name)
                relative = os.path.relpath(source, self._installPrefix)
                if relative in staged:
                    with open(source, "rb") as f:
                        if Digest(f.read()) == staged[relative]:
                            continue
                destination = os.path.join(self.package_folder, relative)
                os.makedirs(os.path.dirname(destination), exist_ok=True)
                shutil.copy2(source, destination, follow_symlinks=False)
        # its sources, as src/<directory>, where its debug information names them once deployed - those of its binary
        # directory's files that the build listed (StroikaThirdPartyComponent.cmake: none, without STROIKA_THIRD_PARTY_SOURCES)
        directory = self._directory(self.source_folder)
        binary = os.path.join(self.build_folder, "build", directory)
        listed = os.path.join(binary, "package-sources.txt")
        if os.path.isfile(listed):
            with open(listed) as f:
                for pattern in filter(None, (line.strip() for line in f)):
                    copy(self, pattern, binary, os.path.join(self.package_folder, "src", directory), excludes=("*/CMakeFiles/*",))
        save(self, os.path.join(self.package_folder, INSTALL_PREFIX_FILE), self._installPrefix)
