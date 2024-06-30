# Stroika::[Frameworks](../)::WebServer

This Folder contains the [Frameworks](../)::WebServer Framework source code.

## Overview

The Stroika WebServer Framework is a module devoted to building web server functionality.

This could be used to build a real web-server (such as Apache) - but the real focus is less
on that sort of thing, and more building light-weight web-service/web-server type functionality
into any application.

This framework can also be used as an alternative to stuff like cgi, and hosting inside of Apache,
where you build a standalone app that has - in essence - the part of apache you care about -
builtin - as part of your application.

## Highlights
  - Easy to add HTTP / web-server into any application, so it can be programatically operated (typically via webservices) - or so it can serve-up content viewable in a web-browser
  - Rails-like message router

## Getting Started
  - [Sample Web Server](../../../../../Samples/WebServer/)
  - Or even simpler - http responses for SSDP
    ~~~
    struct WebServerForDeviceDescription_ : WebServer::ConnectionManager {
        static inline const HTTP::Headers kDefaultResponseHeaders_{[] () {
            HTTP::Headers h;
            h.server = "stroika-ssdp-server-demo"sv;
            return h;
        }()};
        WebServerForDeviceDescription_ (uint16_t webServerPortNumber, const DeviceDescription& dd)
            : ConnectionManager{SocketAddresses (InternetAddresses_Any (), webServerPortNumber),
                                Sequence<Route>{
                                    Route{""_RegEx,
                                          [dd] (Message* m) {
                                              RequireNotNull (m);
                                              Response& response   = m->rwResponse ();
                                              response.contentType = DataExchange::InternetMediaTypes::kXML;
                                              response.write (Stroika::Frameworks::UPnP::Serialize (dd));
                                          }},
                                },
                                Options{.fMaxConnections = 3, .fDefaultResponseHeaders = kDefaultResponseHeaders_}}
        {
        }
    };
    ~~~


## Miscelaneous Features status

Very roughly, this web server supports HTTP 1.1 protocol version.
What this means - is that unless you need HTTPS - this will do everything you need, and work just fine interoperating with web browsers and web applications.

| Feature                        | Supported?                 |
| ---------------------          | ----------                 |
| Chuncked Transfers             | yes                        |
| Cookies                        | yes (partly - not storage) |
| HTTPS                          | no (planned v3)            |
| Compression                    | yes                        |
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
