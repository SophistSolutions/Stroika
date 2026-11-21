"""
One of Stroika's third-party components, stroika-<directory> (in lower case, as Conan requires): ThirdPartyComponents/
<directory>, built by its own recipe there as ThirdPartyComponents/CMakeLists.txt builds it (StroikaThirdPartyComponent.cmake)
- Conan only keeping what it builds, in its cache, so a configuration builds each component once, and every build tree on the
machine finds it there after that (StroikaComponentFromConan, in ThirdPartyComponents/CMakeLists.txt, which runs Conan).

What identifies a package:
  - its version: its recipe's (set (version_ ...)), given as Conan's --version;
  - its recipe revision: a digest of the files that build it (export_sources) - StroikaThirdPartyComponent.cmake,
    RemoveFiles.cmake and its directory - so editing them builds it again;
  - the option "configuration": a digest of the configuration's toolchain file and Stroika settings. No settings: the
    compilers and flags are the configuration's.
Where the configuration and the build are - its toolchain file, the archives, cmake and its generator - are conf values
(user.stroika:...), not part of a package's identity.

Built in Conan's cache, it is installed into a directory there, and packaged with that directory's path
(stroika-install-prefix.txt) - which StroikaDeploy.py, copying the package into a configuration's Builds directory,
replaces in each text file by that directory.
"""

import os

from conan import ConanFile
from conan.tools.files import copy, save

_INSTALL_PREFIX_FILE = "stroika-install-prefix.txt"


class StroikaThirdPartyComponent(ConanFile):
    options = {"configuration": ["ANY"]}

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

    def build(self):
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
        self.run(f'"{cmake}" {generator} -S "{self.source_folder}" -B "{build}"'
                 f' "-DCMAKE_TOOLCHAIN_FILE={conf("toolchain_file")}" "-DSTROIKA_ORIGS_CACHE={conf("origs_cache")}"'
                 f' "-DSTROIKA_THIRD_PARTY_PREFIX={self._installPrefix}/"')
        # a multi-configuration generator's (Visual Studio's) build type, and its processors (msbuild -m), as the top
        # CMakeLists.txt builds the components
        self.run(f'"{cmake}" --build "{build}"' + (f' --config {conf("build_config")} --parallel' if conf("build_config") else ""))

    def package(self):
        copy(self, "*", self._installPrefix, self.package_folder)
        save(self, os.path.join(self.package_folder, _INSTALL_PREFIX_FILE), self._installPrefix)
