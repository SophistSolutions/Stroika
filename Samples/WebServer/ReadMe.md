TONS TODO.

>	Create WebServer Demo here

>	Based on simple XML Config file

>	Demo the 'Service' Framework - kill -hup, and make that work with windows services

>	On HUP, re-read config file (or windows equiv signal)

>	Serve up a simple directory of content

>	Embellish the WebServer functionality to work with streams.

>	MAYBE EVENTUALLY (not needed on first draft) add support for server cert, so does
	SSL server-side
	
>	Enable TESTING - by creating a TEST in the Tests folder (../), and have that just
	RUN this demo.
	>> Use NEW Execution::SubProcess module (with streams)
	
>	That implies Top-level Demos\build.pl script, and call from top level build before
	tests
	
>	Try to get webserver working on BOTH windows / unix (and tested from each)

>	Embellish sevice framework so can have abstract service base, and then
	have one IMPL for native UNIX (done), one for native WinService, and one for
	unix-style appraoch but on windows service (exe running but not using service interace)
	
>	Do simple WS admin section - so we have webservices controlling

>	If no web page root - have DEFAULT baked in webpage root (trivial - points to admin page).

>	Do basic Admin webpage - which does jquery, and ajax, to WS front end.

