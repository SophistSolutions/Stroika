﻿Stroika v2.1
----------

Stroika is a C++ application framework designed to make high performance,
modern, reliable and threaded applications easier to write.

  *  Nearly all public interfaces and methods are well documented; implementations are clear and well commented.
  *  An elaborate system of built-in assertions (including thread-safety assertions) helps document and enforce usage patterns (without performance cost in production builds).
  *  Each release is tested with Valgrind, google sanitizers, and a variety of platforms and compilers, as well as a large body of regression tests (including most documentation examples) to help assure library correctness.
  *  [Sample applications](Samples/ReadMe.md) help show common use cases (like building a web server into your app to deliver web-service content and APIs). Quick start using pre-built docker containers.
  
Stroika provides
  *  [Container classes](Library/Sources/Stroika/Foundation/Containers/ReadMe.md) (Collection, Queue, Mapping, Sequence, Set, and much more with rich variety of backend implementations).
  *  [DataExchange](Library/Sources/Stroika/Foundation/DataExchange/ReadMe.md) (VariantValue, rich collection of data format serializer/deserializers - like JSON, object to/from serialization  mappings, and much more).
  *  Execution pattern classes (external process runner, synchronization patterns, blockingqueue, threadpools, and much more).
  *  [Networking](Library/Sources/Stroika/Foundation/IO/Networking/ReadMe.md) support (high level socket wrappers, Web client fetching).
  *  Simple, elegant [streams library](Library/Sources/Stroika/Foundation/Streams/ReadMe.md), with adapters for cryptography, compression, text transforms, etc.
  *  Seamless integration with many important libraries, including: boost, libcurl, lzma sdk, openssl, sqlite, xerces, zlib (e.g. wrap a compression stream - using  for example zlib).
  *  Framework libraries for [web server](Library/Sources/Stroika/Frameworks/WebServer/ReadMe.md), [web services](Library/Sources/Stroika/Frameworks/WebService/ReadMe.md), [UPnP](Library/Sources/Stroika/Frameworks/UPnP/ReadMe.md), SSDP, text editing/word processor, system performance monitoring, and much more.
  *  and much more...


Trade-offs
  *  Stroika's biggest strenght is also its biggest weakness. There exist single purpose libraries that do many of the things Stroika does, and that allows you to pick and choose. But libraries that do anything of any complexity must create their own infrastructure for a wide variety of building block problems (e.g. Xerces has its own stream classes). And if you have two such libraries, they won't interact very seemlessly. Stroika takes the approach of building its functionality in layers, off other components in lower layers. This increases the cost of using Stroika for something small (because pulling one thing in pulls many things in), but then pays huge dividends as you use it to accomplish a second or third task. 
  
Versions
  *  Stroika v2.0 is very stable, nearly complete, and requires C++14 or later. Stroika v2.0 is used to power a wide variety of open source and commercial applications.

  *  Stroika v2.1 is more actively evolving, and requires C++17 or later.

Compilers / Platforms tested
  *  Stroika v2.0
     <br/>Regularly tested on x86, arm, gcc5 thru gcc8, clang 3 thru clang 6, XCode 8 thru 9.
  *  Stroika v2.1
     <br/>Regularly tested on x86, arm, gcc7 thru gcc8, clang 6 thru clang 7, XCode 10.

For more details on individual release changes, see:
	[Release-Notes.md](Release-Notes.md)

"Getting Started" build instructions can be found in:
	[Documentation/Building Stroika.pdf](Documentation/Building%20Stroika.pdf)

Looking through the samples is also a good way to start:
	[Samples/ReadMe.md](Samples/ReadMe.md)

Please report bugs at:
	[http://stroika-bugs.sophists.com](http://stroika-bugs.sophists.com)
