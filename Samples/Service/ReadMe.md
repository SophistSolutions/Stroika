Sample Simple Service Application

A service is a program that runs in the background on your computer, and has no user interface.

This is often very handy - and often useful to mix together with a related non-service (UI) program component.

This concept exists on both UNIX and Windows, but is implemented differently.

This sample application shows how to use the Stroika Service Framework to build portable code
that can run as either a Windows Service, or a UNIX service.
 
This sample demonstrates a few Stroika features.

      o   Creating a service application (one that can be automatically started/stopped by
          the OS, and one where you can query the status, check process ID, etc)

      o   Simple example of command line processing

      o   Simple example of Logging (to syslog or windows log or other)

Read the documentation on Stroika::Frameworks::Service::Main for more details.


Special platform instructions:
WINDOWS:
	o	Quick summary
		..\..\Builds\Debug-U-32\Samples-Service\Service.exe --install; (must be done in administrator shell)
		..\..\Builds\Debug-U-32\Samples-Service\Service.exe --start
		..\..\Builds\Debug-U-32\Samples-Service\Service.exe --stop
		..\..\Builds\Debug-U-32\Samples-Service\Service.exe --uninstall
		Run Windows EventViewer and examine 'Windows Logs/Application', and note the entries from source Stroika-Sample-SimpleServer
	o	Install
		..\..\Builds\Debug-U-32\Samples-Service\Service.exe --install
		(this maybe required as administrator)
	o	Then either run from the console using
		Service.exe --run-directly			***RECOMMENDED***
	o	Or use the service manager gui, and start and stop the service with that
	o	Or use 
		net start Test-Service
	o	Un-Install
		..\..\Builds\Debug-U-32\Samples-Service\Service.exe --uninstall



UNIX:
	o	Build (no install is needed).
	o	Run XXX --help (to see options)
	o	Run XXX --start
		to start the service
	o	Run XXX --status
		to see the process id
	o	Run XXX --stop 
		to stop the serivce
	o	Run XXX --run-as-service
		to run the service immediately (not as a deaemon). This is mostly handy for
		debugging.
