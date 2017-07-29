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

BASIC_NAME_LC=stroika-simpleservice
BASIC_NAME=Stroika-SimpleService

WORKDIR=$(ObjDir)/DEB_TMP/

installer-deb:
	@###NB: This omits the init.d stuff, update-init.d etc - which I dont think is needed anymore (pre-systemd) --LGP 2017-07-29
	@$(StroikaRoot)/ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) -n "Building DEB installer (temp files in $(ObjDir)/DEB_TMP); output to $(BUILD_OUT)..."
	@rm -rf $(WORKDIR)
	@#Make directories to be created by the installer
	@mkdir -p $(Building)/DEB_TMP/$(BASIC_NAME_LC)-$(DEBVERSION)/opt/$(BASIC_NAME)/
	@mkdir -p $(Building)/DEB_TMP/$(BASIC_NAME_LC)-$(DEBVERSION)/var/opt/$(BASIC_NAME)/
	
	@echo "Version: $DEBVERSION" >> $(WORKDIR)$(BASIC_NAME_LC)-$(DEBVERSION)/DEBIAN/control
	@cat $(BASIC_NAME_LC).control >> $(WORKDIR)$(BASIC_NAME_LC)-$(DEBVERSION)/DEBIAN/control

	@#??? should  I use servicectl?
	@echo "service $(BASIC_NAME_LC) start" >> $(WORKDIR)$(BASIC_NAME_LC)-$(DEBVERSION)/DEBIAN/postinst

	@#??? should  I use servicectl?
	@echo "(service $(BASIC_NAME_LC) stop) || true" > $(WORKDIR)$(BASIC_NAME_LC)-$(DEBVERSION)/DEBIAN/prerm

	@#List files to actually install and where
	@install -m 755 --strip $(ObjDir)Samples_SimpleService $(WORKDIR)$(BASIC_NAME_LC)-$(DEBVERSION)/opt/$(BASIC_NAME)/$(BASIC_NAME)
	@install -m 644 $(BASIC_NAME_LC).service $(WORKDIR)$(BASIC_NAME_LC)-$(DEBVERSION)/etc/systemd/system/$(BASIC_NAME_LC).service

	@dpkg --build $(WORKDIR)$(BASIC_NAME_LC)-$(DEBVERSION) 2>&1 > $BUILD_OUT
	@cp $(WORKDIR)$(BASIC_NAME_LC)-$(DEBVERSION)/$(BASIC_NAME_LC)-$(DEBVERSION).deb $(ObjDir)$(BASIC_NAME_LC)-$(DEBVERSION).$(INSTALLER_OS_TAG).$(INSTALLER_PLATFORM_TAG).deb
	
	@$(ECHO) "done"