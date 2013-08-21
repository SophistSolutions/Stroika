>	Redo build scripts so more overtly top-level built around make

>	Scripting will be in build-scripts and just for utilities hard to write
	in make, but all will be driven by make rules
	
>	Rewrite build scripts in either ruby or python. Decided i dont like perl

>	Configurations and ConfiguraitonNames.
	Each configuration will be stored in a single xml file named
		Configuration-CONFIGURATIONNAME.xml
		what goes in CONFIGURATIONNAME is up to the user. But it could be
		"Platform_Linux"
		"Windows64-debug"
		"arm-ubuntu-gcc45"
		etc
		
	Each 
		Configuration-CONFIGURATIONNAME.xml will define the stuff used for the build
		
	All 
		Configuration-XXX.xml files at the top level of Stroika/Configurations
		will be assumed in place (and library scripts and make list-configurations will
		list them)
	
>	Inside IntermediateFiles will be
	Configuration-CONFIGURATIONNAME.APPLIED
	(if its been applied). No data in the file - jstu for make timestamps
	
>	Inside IntermediateFiles and Build will be the CONFIGURARIONNAME subdirs that
	now are called "Platform_Linux". All the builds go in there.

>	top-level make will depend on special target configurations-applied
	which will use those Configuration-CONFIGURATIONNAME.APPLIED as a datestamp
	along with the source configuration.xml file to autorebuild as needed
	
>	Eventually the ThirdPartyProducts stuff all needs to be redone to use
	these configuaritons (so builds respect the target compiler etc)
	
