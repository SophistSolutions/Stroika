include ../../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir					=       $(StroikaRoot)Library/Sources/Stroika/Foundation/Cryptography/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Cryptography/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk


SubDirs	=		\
	Digest		\
	Encoding	\
	OpenSSL		\
	SSL			\

.PHONY:			$(SubDirs)


vpath %cpp $(SrcDir)


Objs	=	\
	$(ObjDir)Hash.o\
	$(ObjDir)Format.o\


all:	$(ObjDir) $(Objs) $(SubDirs)


list-objs::
	@echo \
		$(shell $(MAKE) --directory Digest -s list-objs CONFIGURATION=$(CONFIGURATION))\
		$(shell $(MAKE) --directory Encoding -s list-objs CONFIGURATION=$(CONFIGURATION))\
		$(shell $(MAKE) --directory OpenSSL -s list-objs CONFIGURATION=$(CONFIGURATION))\
		$(shell $(MAKE) --directory SSL -s list-objs CONFIGURATION=$(CONFIGURATION))


$(SubDirs):
	@$(MAKE) --directory $@ -s CONFIGURATION=$(CONFIGURATION)
