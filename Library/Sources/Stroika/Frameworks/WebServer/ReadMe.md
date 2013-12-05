The Stroika WebServer Framework is a module devoted to building web server functionality.

This could be used to build a real web-server (such as Apache) - but the real focus is less
on that sort of thing, and more building light-weight web-service/web-server type functionality
into any application.

This framework can also be used as an alternative to stuff like cgi, and hosting inside of Apache,
where you build a standalone app that has - in essence - the part of apache you care about -
builtin - as part of your application.

TODO:
	>	Add in notion of Rails-like Router. Something with good defaults but less opaque about
		how mapping done. Maybe use std::function and RegExp to match urls and direct requests
		to handlers? Or maybe abstract classes bette r than std::function here?

	>	Consider migrating most (all?) classes down a level under "HTTP" folder
