# Stroika::[Frameworks](../)::WebService

This Folder contains the [Frameworks](../)::WebService Framework source code.

## Overview

The Stroika WebService Framework allows you to expose programatic APIs through web services (SOAP, JSON, REST).

This framework depends on the Web-Server framework

## Highlights
  - Easy to add programatic API (web services control) to any Stroika based application
  - Leverages [ObjectVariantMapper](../../../../Foundation/DataExchange/ObjectVariantMapper.h) to easily serialize, and WebServer framework to serve-up content via http
  - Early draft support for things like JSON-RPC, and OpenAPI

## Getting Started
  - [Sample Web Services App](../../../../../Samples/WebServices/)

## Status

[[[This is just at the early prototyping phase]]]

## Modules

- [Basic.h](Basic.h)
- [Client/](Client/) - proxies
- [JSON-RPC.h](JSON-RPC.h)
- [OpenAPI/](OpenAPI/)
- [Server/](Server/) - stubs
