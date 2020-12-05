# This folder contains all the Stroika Library [Foundation](../ReadMe.md)::[IO](../ReadMe.md)::FileSystem source code

## Overview

This directory contains code relating to filesystem access. Before Stroika 2.1, there were lots of classes for things like filesystem operations but most of these have been deprecated in favor of using the std::filesystem. Probably much of the remaining code will be deprecated, but some will remain (e.g. code that integrates with the the Stroika Stream classes, for example).

## Modules

- [Common.h](Common.h)
- [Directory.h](Directory.h)
- [DirectoryIterable.h](DirectoryIterable.h)
- [DirectoryIterator.h](DirectoryIterator.h)
- [Disk.h](Disk.h)
- [Exception.h](Exception.h)
- [FileInputStream.h](FileInputStream.h)
- [FileOutputStream.h](FileOutputStream.h)
- [FileStream.h](FileStream.h)
- [FileUtils.h](FileUtils.h)
- [MemoryMappedFileReader.h](MemoryMappedFileReader.h)
- [MountedFilesystem.h](MountedFilesystem.h)
- [PathName.h](PathName.h)
- [TemporaryFile.h](TemporaryFile.h)
- [ThroughTmpFileWriter.h](ThroughTmpFileWriter.h) - utility to atomically update/write a file
- [WellKnownLocations.h](WellKnownLocations.h) - portably access standard filesystem locations (like /tmp)
