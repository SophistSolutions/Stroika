StroikaRoot	:=	$(abspath ../../../../../)/
SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Frameworks/Auth/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/Auth/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk



vpath %cpp $(SrcDir)


SubDirs	:=	\
	OAuth	\



Objs	=	\

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)


.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
