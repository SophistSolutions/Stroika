include ../../../../../../IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir					=	$(StroikaRoot)Library/Sources/Stroika/Foundation/IO/FileSystem/
ObjDir					=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/IO/FileSystem/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk



vpath %cpp $(SrcDir)


Objs	=	\
	$(ObjDir)Common${OBJ_SUFFIX}\
	$(ObjDir)Directory${OBJ_SUFFIX}\
	$(ObjDir)DirectoryIterable${OBJ_SUFFIX}\
	$(ObjDir)DirectoryIterator${OBJ_SUFFIX}\
	$(ObjDir)Disk${OBJ_SUFFIX}\
	$(ObjDir)FileInputStream${OBJ_SUFFIX}\
	$(ObjDir)FileOutputStream${OBJ_SUFFIX}\
	$(ObjDir)FileStream${OBJ_SUFFIX}\
	$(ObjDir)FileSystem${OBJ_SUFFIX}\
	$(ObjDir)FileUtils${OBJ_SUFFIX}\
	$(ObjDir)MemoryMappedFileReader${OBJ_SUFFIX}\
	$(ObjDir)MountedFilesystem${OBJ_SUFFIX}\
	$(ObjDir)PathName${OBJ_SUFFIX}\
	$(ObjDir)ThroughTmpFileWriter${OBJ_SUFFIX}\
	$(ObjDir)WellKnownLocations${OBJ_SUFFIX}\


all:	$(ObjDir) $(Objs)

