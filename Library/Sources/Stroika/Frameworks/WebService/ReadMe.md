# Stroika::[Frameworks](../)::WebService

This Folder contains the [Frameworks](../)::WebService Framework source code.

## Overview

The Stroika WebService Framework allows you to expose programatic APIs through web services (SOAP, JSON, REST).

This framework depends on the [Web Server Framework](../WebServer)

## Highlights
  - Easy to add programatic API (web services control) to any Stroika based application
  - Leverages [ObjectVariantMapper](../../../../Sources/Stroika/Foundation/DataExchange/ObjectVariantMapper.h) to easily serialize, and WebServer framework to serve-up content via http
  - Early draft support for things like JSON-RPC, and OpenAPI, and a few utility functions layered on top of ObjectVariantMapper to map web-payloads to XML/JSON as appropriate.

## Getting Started
  - [Sample Web Services App](../../../../../Samples/WebService/)

## Modules

- [Basic.h](Basic.h)
- [Client/](Client/) - proxies
- [JSON-RPC.h](JSON-RPC.h)
- [OpenAPI/](OpenAPI/)
- [Server/](Server/) - stubs
