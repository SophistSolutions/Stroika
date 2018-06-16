Stroika v2.0
----------

Stroika is a C++ application framework designed to make high performance, 
modern, reliable and multi-threaded applications easier to write.

  *  Nearly all public interfaces and methods are well documented; implementations are clear and well commented
  *  An elaborate system of builtin assertions (including threadsafty assertions) helps document and enforce usage patterns (without performance cost in production builds).
  *  Each release is tested with valgrind, google sanitizers, and a variety of platforms and compilers. as well as a large body of regression tests (including most documentation examples) to help assure library correctness.
  *  Sample applications help show common use cases (like building a web server into your app to deliver web-service content and APIs). 
  
Stroika provides
  *  Container classes (Collection, Queue, Mapping, Sequence, Set, and much more with rich variety of backend implementations)
  *  DataExchange (VariantValue, rich collection of data format serializer/deserializers - like JSON, object to/from serialziation mappings, and much more)
  *  Execution pattern classes (external process runner, synchonization patterns, blockingqueue, threadpools, and much more)
  *  Networking support (high level socket wrappers, Web client fetching)
  *  Framework libraries for web server, web services, UPnP, SSDP, text editing/word proocessor, system performance monitoring, and much more
  *  and much more...


Versions
  *  Stroika v2.0 is very stable, and nearly complete, and requires C++14 or later. Stroika v2.0 is used to power a wide variety of open source and commercial applications.
  *  Later versions depend on newer compilers, and are actively being developed.


For more details on individual release changes see:
	[Release-Notes.md](Release-Notes.md)

Quick start building instructions can be found in:
	[Documentation/Building Stroika.pdf](Documentation/Building%20Stroika.pdf)

Looking through the samples is also a good way to start:
	[Samples/ReadMe.md](Samples/ReadMe.md)

Please report bugs at:
	[http://stroika-bugs.sophists.com](http://stroika-bugs.sophists.com)