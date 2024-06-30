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
        struct WebServerForDeviceDescription_ {
                WebServerForDeviceDescription_ (uint16_t webServerPortNumber, const DeviceDescription& dd)
                {
                    auto onConnect = [dd] (const ConnectionOrientedStreamSocket::Ptr& acceptedSocketConnection) {
                        Execution::Thread::Ptr runConnectionOnAnotherThread = Execution::Thread::New ([acceptedSocketConnection, dd] () {
                            // If the URLs are served locally, you may want to update the URL based on
                            // IO::Network::GetPrimaryInternetAddress ()
                            Connection conn{acceptedSocketConnection,
                                            Connection::Options{.fInterceptorChain = Sequence<Interceptor>{Interceptor{[=] (Message* m) {
                                                                    RequireNotNull (m);
                                                                    Response& response           = m->rwResponse ();
                                                                    response.rwHeaders ().server = "stroika-ssdp-server-demo"sv;
                                                                    response.contentType         = DataExchange::InternetMediaTypes::kXML;
                                                                    response.write (Stroika::Frameworks::UPnP::Serialize (dd));
                                                                }}}}};
                            conn.remainingConnectionLimits = HTTP::KeepAlive{0, 0s}; // disable keep-alives
                            conn.ReadAndProcessMessage ();
                        });
                        runConnectionOnAnotherThread.SetThreadName ("SSDP Service Connection Thread"sv);
                        runConnectionOnAnotherThread.Start ();
                    };
                    fListener = Listener{SocketAddresses (InternetAddresses_Any (), webServerPortNumber), onConnect};
                }

                optional<Listener> fListener;
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

## TODO
  - Add in notion of Rails-like Router. Something with good defaults but less opaque about
how mapping done. Maybe use std::function and RegExp to match urls and direct requests
to handlers? Or maybe abstract classes bette r than std::function here?

