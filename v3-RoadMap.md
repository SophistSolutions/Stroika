# Stroika v3 Roadmap

Rough outline of plans for Stroika v3

## Schedule
- Start April 2022
- Target completion 
  - roughly end of 2023
  - depends on stable C++20 compilers available for all platforms (e.g. cross-compile to raspberry pi still
    not easy as of 2022-12-29)

## Features
- C++20 features adoption/integration
  - Lose backward compatability support for C++17, and C++-17 era compilers
  - Assume things that were only conditionally used/supported in Stroika 2.1 (e.g. operator<=>, char8_t, etc)
  - Concepts use **started**
    - in containers
    - elsewhere
    - replace use of enable_if_t, for example
  - use span<> **1/2 done**
  - designated initiializers
  - use range<>
  - use/leverage/support std::format
  - async
    Just planned to use in generators - not more widely
  - new chrono time support integrated with Time classes **3/4 done** [STK-925](https://stroika.atlassian.net/browse/STK-925)
- HTTP server improvements
  - inline zip
  - https
  - http 2??
  - http 3?
- Docs/Debug
  - Clear up confusion/use surrounding AssertExternallySynchronizedMutex **3/4 done**
- Fully fix String class **1/2 done**
  - handle surrogates etc properly
  - Support compact ascii/char8_t representation
  - Performance improvements (largely the above, but profile and do more - UTF convert e.g)
- Build system
  - Consider switch to cmake, or otherwise address make depend issue
  - provide cmake project file / package so easy for people using cmake to just use Stroika
    https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html
- OpenAPI integration (maybe automatic methods, or at least validation).
- Some sort of GUI integration (perhaps ) - CEF (https://code.google.com/p/chromiumembedded/) or Gekko/Mozilla, or WebKit/V8
- Finish cleanup of IO::FileSystem code (really getting rid of most or all of it- unless I decide worth virtual fs for zipfiles and remote S3 etc)?
- Performance
  - Better performance and performance tests comparing with other libararies - **1/10 done**
- DataExchange
  - Faster json parsing **1/2 done**
  - Binary JSON formats
  - Cleanup ObjectVariantMapper (using modern C++ instead of current template enable_if_t mess)
  - JSON Pointer/JSONPatch [STK-110](https://stroika.atlassian.net/browse/STK-110)
- Threads
  - use jthread, and stop tokens to significantly cleanup Stroika thread classes [STK-707](https://stroika.atlassian.net/browse/STK-707), [STK-930](https://stroika.atlassian.net/browse/STK-930)
- Immutables
  - Already did alot of this in Stroika v2.1 (eg. String::IRep)
  - Because of new rules on lifetime of iterators, I think we can lose most/all use of shared_from_this - used just for
    making iterators - instead do something (maybe already done) to check bugs, but otherwise container cannot change while
    iterating so no need to copy shared_ptr
  - but consider doing much more or at least documenting it better.
  - CONSIDER the pattern of USING const objects more, or facilitating that as alternative
  - IT IS important for reasning about programs to know what is immutable. If I can do more in foundation layer (not contrainers probbaly). BUt at least String would be easy todo (cuz I already have StringBuilder).


## Expected DEFERRED til Stroika v3.1
  - Modules support
    compiler support not ready for prime time in C++20
