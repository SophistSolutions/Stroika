# Stroika::[Frameworks](../ReadMe.md)::WebServer

This Folder contains the [Frameworks](../ReadMe.md)::WebServer Framework source code.

## Overview

The Stroika WebServer Framework is a module devoted to building web server functionality.

This could be used to build a real web-server (such as Apache) - but the real focus is less
on that sort of thing, and more building light-weight web-service/web-server type functionality
into any application.

This framework can also be used as an alternative to stuff like cgi, and hosting inside of Apache,
where you build a standalone app that has - in essence - the part of apache you care about -
builtin - as part of your application.

TODO: > Add in notion of Rails-like Router. Something with good defaults but less opaque about
how mapping done. Maybe use std::function and RegExp to match urls and direct requests
to handlers? Or maybe abstract classes bette r than std::function here?

    >	Consider migrating most (all?) classes down a level under "HTTP" folder

## Miscelaneous Features status

Very roughly, this web server supports HTTP 1.1 protocol version.
What this means - is that unless you need HTTPS - this will do everything you need, and work just fine interoperating with web browsers and web applications.

| Feature                        | Supported?                 |
| ---------------------          | ----------                 |
| Chuncked Transfers             | yes                        |
| Cookies                        | yes (partly - not storage) |
| HTTPS                          | no                         |
| Compression                    | no                         |
| HTTP Keep Alives               | yes                        |
| CORS                           | yes                        |
| filesystem or dynamic source   | yes                        |
| HEAD/Conditional Get/ETags     | yes                        |


## Modules

- [Connection.h](Connection.h)
- [ConnectionManager.h](ConnectionManager.h)
- [DefaultFaultInterceptor.h](DefaultFaultInterceptor.h)
- [FileSystemRouter.h](FileSystemRouter.h)
- [Interceptor.h](Interceptor.h) - central concept
- [InterceptorChain.h](InterceptorChain.h)
- [LoggingInterceptor.h](LoggingInterceptor.h)
- [Message.h](Message.h)
- [Request.h](Request.h)
- [RequestHandler.h](RequestHandler.h)
- [Response.h](Response.h)
- [Router.h](Router.h)
