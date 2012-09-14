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
	>>> THIS - IN TURN - Requries embellishing the STREAMS code to support
		TIED IN/OUT streams (like iostream)
		
		AND - new runsubprocess code that takes input stream (stdin), and 2 output streams
		(stdout, and stderr) and make it trivial in Stroika to run these subproxceses,
		and use that in my test.
	
>	That implies Top-level Demos\build.pl script, and call from top level build before
	tests
	
>	Try to get webserver working on BOTH windows / unix (and tested from each)

