# Stroika::[Frameworks](../)::WebService

This Folder contains the [Frameworks](../)::WebService Framework source code.

## Overview

The Stroika WebService Framework allows you to expose programatic APIs through web services (SOAP, JSON, REST).

This framework depends on the [Web Server Framework](../WebServer)

## Highlights
  - Easy to add programatic API (web services control) to any Stroika based application
  - Leverages [ObjectVariantMapper](../../../../Sources/Stroika/Foundation/DataExchange/ObjectVariantMapper.h) to easily serialize, and WebServer framework to serve-up content via http
  - A service takes its arguments, and returns its result, as C++ objects, mapped through
    [VariantValue](../../../../Sources/Stroika/Foundation/DataExchange/VariantValue.h) - Stroika's lingua franca for data
    ([why](../../../../Sources/Stroika/Foundation/DataExchange/ReadMe.md#why-variantvalue)) - so one mapping can answer in
    any format a request asks for: JSON today, and XML is https://github.com/SophistSolutions/Stroika/issues/955
  - Early draft support for things like JSON-RPC, and OpenAPI, and a few utility functions layered on top of ObjectVariantMapper to map web-payloads to XML/JSON as appropriate.

## Getting Started
  - [Sample Web Services App](../../../../../Samples/WebService/)

## Modules

- [Basic.h](Basic.h)
- [Client/](Client/) - proxies
- [JSON-RPC.h](JSON-RPC.h)
- [OpenAPI/](OpenAPI/)
- [Server/](Server/) - stubs
- [SOAP.h](SOAP.h) - SOAP 1.1 RPC: its messages, a client call (Invoke), and a server's answer (HandleRequest)
