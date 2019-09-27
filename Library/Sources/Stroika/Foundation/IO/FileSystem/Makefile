StroikaRoot=$(abspath ../../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=	$(StroikaRoot)Library/Sources/Stroika/Foundation/IO/FileSystem/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/IO/FileSystem/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=/Fd$(StroikaRoot)Builds/$(CONFIGURATION)/Stroika-Foundation.pdb
endif

vpath %cpp $(SrcDir)


Objs	=	\
	$(ObjDir)Common${OBJ_SUFFIX}\
	$(ObjDir)Directory${OBJ_SUFFIX}\
	$(ObjDir)DirectoryIterable${OBJ_SUFFIX}\
	$(ObjDir)DirectoryIterator${OBJ_SUFFIX}\
	$(ObjDir)Disk${OBJ_SUFFIX}\
	$(ObjDir)Exception${OBJ_SUFFIX}\
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


all:	$(Objs)

