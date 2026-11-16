StroikaSourceRoot	:=	$(abspath ../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Library/Sources/Stroika/Foundation/IO/FileSystem/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/IO/FileSystem/

vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)DirectoryIterable${OBJ_SUFFIX}\
	$(ObjDir)DirectoryIterator${OBJ_SUFFIX}\
	$(ObjDir)Disk${OBJ_SUFFIX}\
	$(ObjDir)Exception${OBJ_SUFFIX}\
	$(ObjDir)FileInputStream${OBJ_SUFFIX}\
	$(ObjDir)FileOutputStream${OBJ_SUFFIX}\
	$(ObjDir)FileSystem${OBJ_SUFFIX}\
	$(ObjDir)FileUtils${OBJ_SUFFIX}\
	$(ObjDir)MemoryMappedFileReader${OBJ_SUFFIX}\
	$(ObjDir)MountedFilesystem${OBJ_SUFFIX}\
	$(ObjDir)PathName${OBJ_SUFFIX}\
	$(ObjDir)TemporaryFile${OBJ_SUFFIX}\
	$(ObjDir)ThroughTmpFileWriter${OBJ_SUFFIX}\
	$(ObjDir)WellKnownLocations${OBJ_SUFFIX}\

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk

all:	$(Objs)

