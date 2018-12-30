###
### @todo https://stroika.atlassian.net/browse/STK-614 -  Add WIX support to Sample Service installers
###

export StroikaRoot?=$(shell realpath ../../../)/

ifneq ($(CONFIGURATION),)
	include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk

BASIC_NAME_LC=stroika-sampleservice
BASIC_NAME=Stroika-SampleService
OUTDIR_NAME=Samples-Service


ObjDir				=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/$(OUTDIR_NAME)/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk


TARGET_OUT_DIR=			$(StroikaRoot)Builds/$(CONFIGURATION)/$(OUTDIR_NAME)/

ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
BUILT_SERVICE_EXE				=	$(TARGET_OUT_DIR)Service.exe
else
BUILT_SERVICE_EXE				=	$(TARGET_OUT_DIR)Service
endif


VERSION_FILE_IN=$(StroikaRoot)STROIKA_VERSION



MAJOR_DOT_MINOR_VERSION=$(shell $(StroikaRoot)/ScriptsLib/ExtractVersionInformation.sh $(VERSION_FILE_IN) Major.Minor)
INSTALLER_RELEASE_TAG=$(shell $(StroikaRoot)/ScriptsLib/ExtractVersionInformation.sh $(VERSION_FILE_IN) DecoratedStageInfo)
INSTALLER_PLATFORM_TAG?=$(shell uname -m)
INSTALLER_OS_TAG?=$(shell uname -s)

TARGET_OUT_INSTALLER_DEB=$(TARGET_OUT_DIR)$(BASIC_NAME_LC)-$(DEB_VERSION).$(INSTALLER_OS_TAG).$(INSTALLER_PLATFORM_TAG).deb
TARGET_OUT_INSTALLER_RPM=$(TARGET_OUT_DIR)$(BASIC_NAME_LC)-$(RPM_VERSION).$(INSTALLER_OS_TAG).$(INSTALLER_PLATFORM_TAG).rpm
TARGET_OUT_INSTALLER_WIX=$(TARGET_OUT_DIR)$(BASIC_NAME_LC)-$(RPM_VERSION).$(INSTALLER_OS_TAG).$(INSTALLER_PLATFORM_TAG).msi

TARGET_OUT_INSTALLERS=
ifeq ($(shell test -x "`command -v dpkg 2>/dev/null`"; echo $$?),0)
TARGET_OUT_INSTALLERS+=${TARGET_OUT_INSTALLER_DEB}
endif
ifeq ($(shell test -x "`command -v rpmbuild 2>/dev/null`"; echo $$?),0)
TARGET_OUT_INSTALLERS+=${TARGET_OUT_INSTALLER_RPM}
endif
ifneq ($(qFeatureFlag_WIX),no)
TARGET_OUT_INSTALLERS+=${TARGET_OUT_INSTALLER_WIX}
endif

.PHONY:  all installers


all:
ifeq ($(CONFIGURATION),)
	$(error Cannot make installers without a configuration argument)
endif
	@$(MAKE) --silent $(TARGET_OUT_INSTALLERS)


installers:
ifeq ($(CONFIGURATION),)
	$(error Cannot make installers without a configuration argument)
endif
	@$(MAKE) --no-print-directory $(TARGET_OUT_INSTALLERS)

installer-deb:
ifeq ($(CONFIGURATION),)
	$(error Cannot make installers without a configuration argument)
endif
	@$(MAKE) --no-print-directory $(TARGET_OUT_INSTALLER_DEB)

installer-rpm:
ifeq ($(CONFIGURATION),)
	$(error Cannot make installers without a configuration argument)
endif
	@$(MAKE) --no-print-directory $(TARGET_OUT_INSTALLER_RPM)

installer-wix:
ifeq ($(CONFIGURATION),)
	$(error Cannot make installers without a configuration argument)
endif
	@$(MAKE) --no-print-directory $(TARGET_OUT_INSTALLER_WIX)


#The 'deb' package creator requires the directory have particular permissions. on WSL (windows subsystem for linux)
#using the /mnt filesystem, the permissions don't work. So use /tmp for these. But that means creating the directories
#in a temp location (already emppty) and deleting it at the end (bit of a PITA if you want to review the data, but hten just
#comment out the delete line)
#
# NOTE - if you mount the drivefs filesystem with the option "metadata" permissions work, but until msft makes this the default
# just comment out...
# disable compute of workdir (mktemp) on windows / cygwin since not needed and fails and no need to debug now.
ifneq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
#DEB_WORKDIR=$(ObjDir)DEB_TMP/
DEB_WORKDIR:=$(shell mktemp -d)/
endif
DEB_VERSION=$(MAJOR_DOT_MINOR_VERSION)$(INSTALLER_RELEASE_TAG)
DEB_BUILD_OUT=$(ObjDir)DEB-Installer-Build.Out
$(TARGET_OUT_INSTALLER_DEB):
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Building DEB installer: "
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "temp files in $(DEB_WORKDIR); logging to $(DEB_BUILD_OUT)"`
ifeq ($(CONFIGURATION),)
	$(error Cannot make installer-deb without a configuration argument)
endif
	@#NB: This installs control scripts for systemd, so assumes ubuntu 15 or later --LGP 2017-07-29
	@#Make directories to be created by the installer
	@mkdir -p $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)/DEBIAN
	@mkdir -p $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)/opt/$(BASIC_NAME)/
	@mkdir -p $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)/usr/lib/systemd/system
	@mkdir -p $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)/var/opt/$(BASIC_NAME)/
	@chmod 755 -R $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)
	
	@echo "Version: $(DEB_VERSION)" >> $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)/DEBIAN/control
	@cat $(BASIC_NAME_LC).control >> $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)/DEBIAN/control

	@echo "systemctl enable $(BASIC_NAME_LC)" >> $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)/DEBIAN/postinst
	@echo "systemctl start $(BASIC_NAME_LC)" >> $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)/DEBIAN/postinst
	@chmod 755 $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)/DEBIAN/postinst

	@echo "(systemctl stop $(BASIC_NAME_LC)) || true" >> $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)/DEBIAN/prerm
	@echo "(systemctl disable $(BASIC_NAME_LC)) || true" >> $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)/DEBIAN/prerm
	@chmod 755 $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)/DEBIAN/prerm

	@#List files to actually install and where
	@install -m 755 --strip --strip-program=$(STRIP) $(BUILT_SERVICE_EXE) $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)/opt/$(BASIC_NAME)/$(BASIC_NAME)
	@install -m 644 $(BASIC_NAME_LC).service $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)/usr/lib/systemd/system/$(BASIC_NAME_LC).service

	@dpkg --build $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION) 2>&1 > $(DEB_BUILD_OUT)
	@cp $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION).deb $(TARGET_OUT_INSTALLER_DEB)
	
	@rm -rf $(DEB_WORKDIR)
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) "Built: "`$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "$(TARGET_OUT_INSTALLER_DEB)"`


RPM_WORKDIR=$(ObjDir)RPM_TMP/
RPM_VERSION=$(MAJOR_DOT_MINOR_VERSION)$(INSTALLER_RELEASE_TAG)
RPM_BUILD_OUT=$(ObjDir)RPM-Installer-Build.Out
$(TARGET_OUT_INSTALLER_RPM):
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Building RPM installer: "
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "temp files in $(RPM_WORKDIR); logging to $(RPM_BUILD_OUT)"`"..."
ifeq ($(CONFIGURATION),)
	$(error Cannot make installer-rpm without a configuration argument)
endif
	@rm -rf $(RPM_WORKDIR)
	@#Make directories to be created by the installer
	@mkdir -p $(RPM_WORKDIR)RPMS $(RPM_WORKDIR)SRPMS $(RPM_WORKDIR)BUILD $(RPM_WORKDIR)SOURCES $(RPM_WORKDIR)SPECS $(RPM_WORKDIR)tmp
	@mkdir -p $(RPM_WORKDIR)$(BASIC_NAME_LC)-$(MAJOR_DOT_MINOR_VERSION)/opt/$(BASIC_NAME)/
	@mkdir -p $(RPM_WORKDIR)$(BASIC_NAME_LC)-$(MAJOR_DOT_MINOR_VERSION)/usr/lib/systemd/system
	@mkdir -p $(RPM_WORKDIR)$(BASIC_NAME_LC)-$(MAJOR_DOT_MINOR_VERSION)/var/opt/$(BASIC_NAME)/

	@install -m 755 --strip --strip-program=$(STRIP) $(BUILT_SERVICE_EXE) $(RPM_WORKDIR)/$(BASIC_NAME_LC)-$(MAJOR_DOT_MINOR_VERSION)/opt/$(BASIC_NAME)/$(BASIC_NAME)
	@install -m 644 $(BASIC_NAME_LC).service $(RPM_WORKDIR)$(BASIC_NAME_LC)-$(MAJOR_DOT_MINOR_VERSION)/usr/lib/systemd/system/$(BASIC_NAME_LC).service
	
	@cd $(RPM_WORKDIR) && tar -zcf $(BASIC_NAME_LC)-$(MAJOR_DOT_MINOR_VERSION).tar.gz $(BASIC_NAME_LC)-$(MAJOR_DOT_MINOR_VERSION)/
	@cd $(RPM_WORKDIR) && cp $(BASIC_NAME_LC)-$(MAJOR_DOT_MINOR_VERSION).tar.gz SOURCES/
	@echo "%define _topdir "$(RPM_WORKDIR) > $(RPM_WORKDIR)/SPECS/$(BASIC_NAME_LC).spec
	@echo "Version: $(MAJOR_DOT_MINOR_VERSION)" >> $(RPM_WORKDIR)SPECS/$(BASIC_NAME_LC).spec
	@echo "Release: $(INSTALLER_RELEASE_TAG)" >> $(RPM_WORKDIR)SPECS/$(BASIC_NAME_LC).spec
	@cat $(BASIC_NAME_LC).rpm.spec >> $(RPM_WORKDIR)SPECS/$(BASIC_NAME_LC).spec
	@cd $(RPM_WORKDIR) && rpmbuild -ba --quiet SPECS/$(BASIC_NAME_LC).spec > $(RPM_BUILD_OUT) 2>&1

	@cp $(RPM_WORKDIR)RPMS/*/*.rpm $(TARGET_OUT_INSTALLER_RPM)
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) "Built: "`$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "$(TARGET_OUT_INSTALLER_RPM)"`


WIX_WORKDIR=$(ObjDir)WIX_TMP/
WIX_BUILD_OUT=$(ObjDir)WIX-Installer-Build.Out
$(TARGET_OUT_INSTALLER_WIX):
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Building WIX installer: "
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "temp files in $(WIX_WORKDIR); logging to $(WIX_BUILD_OUT)"`"..."
ifeq ($(CONFIGURATION),)
	$(error Cannot make installer-rpm without a configuration argument)
endif
	@rm -rf $(WIX_WORKDIR)
	@#Make directories to be created by the installer
	@mkdir -p $(WIX_WORKDIR)
	@cp $(BUILT_SERVICE_EXE) $(WIX_WORKDIR)$(BASIC_NAME)-Service.exe
