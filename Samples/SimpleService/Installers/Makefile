###
### @todo https://stroika.atlassian.net/browse/STK-614 -  Add WIX support to Sample SimpleService installers
###

-include ../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

StroikaRoot ?= ../../../
TOP_ROOT=$(shell realpath ../../..)/

ObjDir				=	$(TOP_ROOT)IntermediateFiles/$(CONFIGURATION)/Samples_SimpleService/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk

SHELL=/bin/bash

TARGET_OUT_DIR=			$(TOP_ROOT)Builds/$(CONFIGURATION)/


MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=$(shell $(StroikaRoot)ScriptsLib/GetDefaultShellVariable.sh ECHO)
ECHO_BUILD_LINES?=0



VERSION_FILE_IN=$(StroikaRoot)STROIKA_VERSION

all:	installers



MAJOR_DOT_MINOR_VERSION=$(shell $(StroikaRoot)/ScriptsLib/ExtractVersionInformation.sh $(VERSION_FILE_IN) Major.Minor)
INSTALLER_RELEASE_TAG=$(shell $(StroikaRoot)/ScriptsLib/ExtractVersionInformation.sh $(VERSION_FILE_IN) DecoratedStageInfo)
INSTALLER_PLATFORM_TAG?=$(shell uname --machine)
INSTALLER_OS_TAG?=$(shell uname --kernel-name)


installers:	
ifeq ($(CONFIGURATION),)
	$(error Cannot make installers without a configuration argument)
endif
	@if [ -x "$$(command -v dpkg)" ] ; then \
		$(MAKE) --no-print-directory installer-deb CONFIGURATION=$(CONFIGURATION);\
	fi
	@if [ -x "$$(command -v rpmbuild)" ] ; then \
		$(MAKE) --no-print-directory installer-rpm CONFIGURATION=$(CONFIGURATION);\
	fi


BASIC_NAME_LC=stroika-simpleservice
BASIC_NAME=Stroika-SimpleService


DEB_WORKDIR=$(ObjDir)DEB_TMP/
DEB_VERSION=$(MAJOR_DOT_MINOR_VERSION)$(INSTALLER_RELEASE_TAG)
DEB_BUILD_OUT=$(ObjDir)DEB-Installer-Build.Out
installer-deb:
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Building DEB installer: $(shell $(StroikaRoot)ScriptsLib/SubsctituteBackVariables.sh $(TARGET_OUT_DIR)$(BASIC_NAME_LC)-$(DEB_VERSION).$(INSTALLER_OS_TAG).$(INSTALLER_PLATFORM_TAG).deb; temp files in $(DEB_WORKDIR); logging to $(DEB_BUILD_OUT))..."
ifeq ($(CONFIGURATION),)
	$(error Cannot make installer-deb without a configuration argument)
endif
	@#NB: This installs control scripts for systemd, so assumes ubuntu 15 or later --LGP 2017-07-29
	@rm -rf $(DEB_WORKDIR)
	@#Make directories to be created by the installer
	@mkdir -p $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)/DEBIAN
	@mkdir -p $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)/opt/$(BASIC_NAME)/
	@mkdir -p $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)/usr/lib/systemd/system
	@mkdir -p $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)/var/opt/$(BASIC_NAME)/
	
	@echo "Version: $(DEB_VERSION)" >> $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)/DEBIAN/control
	@cat $(BASIC_NAME_LC).control >> $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)/DEBIAN/control

	@echo "systemctl enable $(BASIC_NAME_LC)" >> $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)/DEBIAN/postinst
	@echo "systemctl start $(BASIC_NAME_LC)" >> $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)/DEBIAN/postinst
	@chmod 755 $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)/DEBIAN/postinst

	@echo "(systemctl stop $(BASIC_NAME_LC)) || true" >> $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)/DEBIAN/prerm
	@echo "(systemctl disable $(BASIC_NAME_LC)) || true" >> $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)/DEBIAN/prerm
	@chmod 755 $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)/DEBIAN/prerm

	@#List files to actually install and where
	@install -m 755 --strip --strip-program=$(shell perl $(StroikaRoot)ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) STRIP) $(TARGET_OUT_DIR)Samples_SimpleService $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)/opt/$(BASIC_NAME)/$(BASIC_NAME)
	@install -m 644 $(BASIC_NAME_LC).service $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION)/usr/lib/systemd/system/$(BASIC_NAME_LC).service

	@dpkg --build $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION) 2>&1 > $(DEB_BUILD_OUT)
	@cp $(DEB_WORKDIR)$(BASIC_NAME_LC)-$(DEB_VERSION).deb $(TARGET_OUT_DIR)$(BASIC_NAME_LC)-$(DEB_VERSION).$(INSTALLER_OS_TAG).$(INSTALLER_PLATFORM_TAG).deb
	
	@$(ECHO) "done"


RPM_WORKDIR=$(ObjDir)RPM_TMP/
RPM_VERSION=$(MAJOR_DOT_MINOR_VERSION)$(INSTALLER_RELEASE_TAG)
RPM_BUILD_OUT=$(ObjDir)RPM-Installer-Build.Out
installer-rpm:
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Building RPM installer:  $(shell $(StroikaRoot)ScriptsLib/SubsctituteBackVariables.sh $(TARGET_OUT_DIR)$(BASIC_NAME_LC)-$(RPM_VERSION).$(INSTALLER_OS_TAG).$(INSTALLER_PLATFORM_TAG).rpm; temp files in $(RPM_WORKDIR); logging to $(RPM_BUILD_OUT))..."
ifeq ($(CONFIGURATION),)
	$(error Cannot make installer-rpm without a configuration argument)
endif
	@#
	@rm -rf $(RPM_WORKDIR)
	@#Make directories to be created by the installer
	@mkdir -p $(RPM_WORKDIR)RPMS $(RPM_WORKDIR)SRPMS $(RPM_WORKDIR)BUILD $(RPM_WORKDIR)SOURCES $(RPM_WORKDIR)SPECS $(RPM_WORKDIR)tmp
	@mkdir -p $(RPM_WORKDIR)$(BASIC_NAME_LC)-$(MAJOR_DOT_MINOR_VERSION)/opt/$(BASIC_NAME)/
	@mkdir -p $(RPM_WORKDIR)$(BASIC_NAME_LC)-$(MAJOR_DOT_MINOR_VERSION)/usr/lib/systemd/system
	@mkdir -p $(RPM_WORKDIR)$(BASIC_NAME_LC)-$(MAJOR_DOT_MINOR_VERSION)/var/opt/$(BASIC_NAME)/

	@install -m 755 --strip --strip-program=$(shell perl $(StroikaRoot)ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) STRIP) $(TARGET_OUT_DIR)Samples_SimpleService $(RPM_WORKDIR)/$(BASIC_NAME_LC)-$(MAJOR_DOT_MINOR_VERSION)/opt/$(BASIC_NAME)/$(BASIC_NAME)
	@install -m 644 $(BASIC_NAME_LC).service $(RPM_WORKDIR)$(BASIC_NAME_LC)-$(MAJOR_DOT_MINOR_VERSION)/usr/lib/systemd/system/$(BASIC_NAME_LC).service
	
	@cd $(RPM_WORKDIR) && tar -zcf $(BASIC_NAME_LC)-$(MAJOR_DOT_MINOR_VERSION).tar.gz $(BASIC_NAME_LC)-$(MAJOR_DOT_MINOR_VERSION)/
	@cd $(RPM_WORKDIR) && cp $(BASIC_NAME_LC)-$(MAJOR_DOT_MINOR_VERSION).tar.gz SOURCES/
	@echo "%define _topdir "$(RPM_WORKDIR) > $(RPM_WORKDIR)/SPECS/$(BASIC_NAME_LC).spec
	@echo "Version: $(MAJOR_DOT_MINOR_VERSION)" >> $(RPM_WORKDIR)SPECS/$(BASIC_NAME_LC).spec
	@echo "Release: $(INSTALLER_RELEASE_TAG)" >> $(RPM_WORKDIR)SPECS/$(BASIC_NAME_LC).spec
	@cat $(BASIC_NAME_LC).rpm.spec >> $(RPM_WORKDIR)SPECS/$(BASIC_NAME_LC).spec
	@cd $(RPM_WORKDIR) && rpmbuild -ba --quiet SPECS/$(BASIC_NAME_LC).spec > $(RPM_BUILD_OUT)

	@cp $(RPM_WORKDIR)RPMS/*/*.rpm $(TARGET_OUT_DIR)/$(BASIC_NAME_LC)-$(RPM_VERSION).$(INSTALLER_OS_TAG).$(INSTALLER_PLATFORM_TAG).rpm
	@echo "done"
