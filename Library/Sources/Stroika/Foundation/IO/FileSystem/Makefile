include ../../../../../../IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir					=       $(StroikaRoot)Library/Sources/Stroika/Foundation/IO/FileSystem/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/IO/FileSystem/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk



vpath %cpp $(SrcDir)


Objs	=	\
	$(ObjDir)Common.o\
	$(ObjDir)Directory.o\
	$(ObjDir)DirectoryIterable.o\
	$(ObjDir)DirectoryIterator.o\
	$(ObjDir)Disk.o\
	$(ObjDir)FileInputStream.o\
	$(ObjDir)FileOutputStream.o\
	$(ObjDir)FileStream.o\
	$(ObjDir)FileSystem.o\
	$(ObjDir)FileUtils.o\
	$(ObjDir)MemoryMappedFileReader.o\
	$(ObjDir)MountedFilesystem.o\
	$(ObjDir)PathName.o\
	$(ObjDir)ThroughTmpFileWriter.o\
	$(ObjDir)WellKnownLocations.o\


all:	$(ObjDir) $(Objs)

