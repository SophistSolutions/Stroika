## Stroika v2.1
----------

Stroika is a C++ application framework designed to make high performance,
modern, reliable and threaded applications easier to write.

* Nearly all public interfaces and methods are well documented; implementations are clear and well commented.
* An elaborate system of built-in assertions (including thread-safety assertions) helps document and enforce usage patterns (without performance cost in production builds).
* Each release is tested with Valgrind, Google Sanitizers, and a variety of platforms and compilers, as well as a large body of regression tests (including most documentation examples) to help assure library correctness.
* [Sample applications](Samples/ReadMe.md) help show common use cases (like building a web server into your app to deliver web-service content and APIs). Quick start using pre-built Docker containers.
  
### Stroika provides

* [Container classes](Library/Sources/Stroika/Foundation/Containers/ReadMe.md) (Collection, Queue, Mapping, Sequence, Set, and much more with rich variety of backend implementations).
* [DataExchange](Library/Sources/Stroika/Foundation/DataExchange/ReadMe.md) (VariantValue, rich collection of data format serializer/deserializers - like JSON, object to/from serialization  mappings, and much more).
* [Execution](Library/Sources/Stroika/Foundation/Execution/ReadMe.md) pattern classes (external process runner, synchronization patterns, blockingqueue, threadpools, and much more).
* [Networking](Library/Sources/Stroika/Foundation/IO/Network/ReadMe.md) support (high level socket wrappers, Web client fetching).
* Simple, elegant [streams library](Library/Sources/Stroika/Foundation/Streams/ReadMe.md), with adapters for cryptography, compression, text transforms, etc.
* Seamless integration with many important libraries, including: boost, libcurl, lzma sdk, openssl, sqlite, xerces, zlib (e.g. wrap a compression stream - using  for example zlib).
* [Framework libraries](Library/Sources/Stroika/Frameworks/ReadMe.md) for [network measurement](Library/Sources/Stroika/Frameworks/NetworkMonitor/ReadMe.md), SSDP/[UPnP](Library/Sources/Stroika/Frameworks/UPnP/ReadMe.md), [system performance monitoring](Library/Sources/Stroika/Frameworks/SystemPerformance/ReadMe.md), [text editing/word processor](Library/Sources/Stroika/Frameworks/Led/ReadMe.md),  [web server](Library/Sources/Stroika/Frameworks/WebServer/ReadMe.md), [web services](Library/Sources/Stroika/Frameworks/WebService/ReadMe.md),  and much more.
* and much more...

### Trade-offs

* Stroika's biggest strength is also its biggest weakness:
  * There are smaller, more focused libraries that do many of the things Stroika does. Being single purpose is a form of modularity (good) and allows for easy adoption/integration. But libraries that do anything of any complexity must create their own infrastructure for a wide variety of building block problems (e.g. Xerces has its own stream classes, pistache date classes, etc). And if you have two such libraries, they are unlikely to interact seemlessly, and their support classes (for stuff like date and streams above) certainly won't.
  * Stroika takes the approach of building its functionality in layers, leveraging other components (e.g. Streams and Containers) in lower layers (as well as standardized C++ libraries). This slightly increases the cost of adopting Stroika for something small (because pulling one thing in pulls many things in), but then pays dividends as you use it to accomplish a second or third task.
  
### Versions

* Stroika v2.0 is very stable, nearly complete, and requires C++14 or later. Stroika v2.0 is used to power a wide variety of open source and commercial applications.

* Stroika v2.1 is more actively evolving, and requires C++17 or later.

### Compilers / Platforms tested

* Stroika v2.0
    <br/>Regularly tested on x86, arm, gcc5 thru gcc8, clang 3 thru clang 6, XCode 8 thru 9.
* Stroika v2.1
    <br/>Regularly tested on x86, arm, gcc7 thru gcc9, clang 6 thru clang 8, XCode 11 (but should support XCode 10).

### CI System Integration

* CircleCI

  * Status: <https://circleci.com/gh/SophistSolutions/Stroika>
  * Build/test script: [.circleci/config.yml](.circleci/config.yml)

  Nice CI system (docker based). Autobuilds Linux configurations.

* Travis CI

  * Status: <https://travis-ci.com/SophistSolutions/Stroika>
  * Build/test script: [.travis.yml](.travis.yml)

  Autobuilds Linux configurations.
  MacOS and Windows may work in teh future.

### Random Features

* Vanilla make based builds
* Portable API (targets windows/mac/unix), and multiple processors

### Summary
  
  For more details on individual release changes, see:
  
  &nbsp;&nbsp;&nbsp;&nbsp;[Release-Notes.md](Release-Notes.md)

"Getting Started" build instructions can be found in:

  &nbsp;&nbsp;&nbsp;&nbsp; [Documentation/Building Stroika.md](Documentation/Building%20Stroika.md)

Looking through the samples is also a good way to start:

  &nbsp;&nbsp;&nbsp;&nbsp;[Samples/ReadMe.md](Samples/ReadMe.md)

Please report bugs at:

  &nbsp;&nbsp;&nbsp;&nbsp;[http://stroika-bugs.sophists.com](http://stroika-bugs.sophists.com)
