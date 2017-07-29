include ../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir				=	$(StroikaRoot)Samples/SimpleService/
ObjDir				=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples_SimpleService/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk

SHELL=/bin/bash


MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=$(shell $(StroikaRoot)ScriptsLib/GetDefaultShellVariable.sh ECHO)
ECHO_BUILD_LINES?=0

BUILD_OUT=Installer-Build.Out


VERSION_FILE_IN=$(StroikaRoot)STROIKA_VERSION
VERSION_FILE_OUT=$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples_SimpleService/AppVersion.h

all:	installers


MAJOR_DOT_MINOR_VERSION=$(shell $(StroikaRoot)/ScriptsLib/ExtractVersionInformation.sh $(StroikaRoot)STROIKA_VERSION Major.Minor)
INSTALLER_RELEASE_TAG=$(shell $(StroikaRoot)/ScriptsLib/ExtractVersionInformation.sh $(StroikaRoot)STROIKA_VERSION DecoratedStageInfo)
DEBVERSION=$(MAJOR_DOT_MINOR_VERSION)$(INSTALLER_RELEASE_TAG)
INSTALLER_PLATFORM_TAG?=$(shell uname --machine)

installers:	installer-deb
#installers:	installer-deb installer-rpm installer-wix

BASIC_NAME=stroika-simple-service

WORKDIR=$(ObjDir)/DEB_TMP/

installer-deb:
	@$(StroikaRoot)/ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) -n "Building DEB installer (temp files in $(ObjDir)/DEB_TMP); output to $(BUILD_OUT)..."
	@rm -rf $(WORKDIR)
	@mkdir -p $(Building)/DEB_TMP/stroika-simpleservice-$(DEBVERSION)/opt/Stroika-SimpleService/
	@mkdir -p $(Building)/DEB_TMP/stroika-simpleservice-$(DEBVERSION)/etc/init.d/
	
	@touch $(WORKDIR)$(BASIC_NAME)-$(DEBVERSION)/DEBIAN/control
	@echo "Version: $DEBVERSION" >> $(WORKDIR)$(BASIC_NAME)-$(DEBVERSION)/DEBIAN/control

	@###List files to actually install and where
	@install -m 755 --strip $(ObjDir)Samples_SimpleService $(WORKDIR)$(BASIC_NAME)-$(DEBVERSION)/opt/Stroika-SimpleService/Stroika-SimpleService

	@dpkg --build $(WORKDIR)$(BASIC_NAME)-$(DEBVERSION) 2>&1 > $BUILD_OUT
	@cp $(WORKDIR)$(BASIC_NAME)-$(DEBVERSION)/$(BASIC_NAME)-$(DEBVERSION).deb $(ObjDir)$(BASIC_NAME)-$(DEBVERSION).$(INSTALLER_OS_TAG).$(INSTALLER_PLATFORM_TAG).deb
	
	@$(ECHO) "done"