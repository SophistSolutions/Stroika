include ../../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir					=       $(StroikaRoot)Library/Sources/Stroika/Foundation/Cryptography/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Cryptography/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk




SubDirs	=	\
	Digest		\
	Encoding	\
	OpenSSL		\
	SSL			\

.PHONY:			$(SubDirs)


vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)Hash.o\
	$(ObjDir)Format.o\

all:	$(Objs) $(SubDirs)


Digest_OBJS := $(addprefix "Digest/", $(shell $(MAKE) --directory Digest -s list-objs CONFIGURATION=$(CONFIGURATION)))
Encoding_OBJS := $(addprefix "Encoding/", $(shell $(MAKE) --directory Encoding -s list-objs CONFIGURATION=$(CONFIGURATION)))
OpenSSL_OBJS := $(addprefix "OpenSSL/", $(shell $(MAKE) --directory OpenSSL -s list-objs CONFIGURATION=$(CONFIGURATION)))
SSL_OBJS := $(addprefix "SSL/", $(shell $(MAKE) --directory SSL -s list-objs CONFIGURATION=$(CONFIGURATION)))


list-objs::
	@echo $(Digest_OBJS) $(Encoding_OBJS) $(SSL_OBJS) $(OpenSSL_OBJS)

$(SubDirs):
	@$(MAKE) --directory $@ -s CONFIGURATION=$(CONFIGURATION)
