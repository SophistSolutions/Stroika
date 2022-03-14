# Stroika v3 Roadmap

Rough outline of (informal/weak) plans for Stroika v3

- Start April 2022
- Improve HTTP server
  - inline zip
  - https
  - http 2??
  - http 3?
- Concepts use in containers
- Immutables
  - Already did alot of this in Stroika v2.1 (eg. String::IRep)
  - Because of new rules on lifetime of iterators, I think we can lose most/all use of shared_from_this - used just for
    making iterators - instead do something (maybe already done) to check bugs, but otherwise container cannot change while
    iterating so no need to copy shared_ptr
  - but consider doing much more or at least documenting it better.
  - CONSIDER the pattern of USING const objects more, or facilitating that as alternative
  - IT IS important for reasning about programs to know what is immutable. If I can do more in foundation layer (not contrainers probbaly). BUt at least String would be easy todo (cuz I already have StringBuilder).
- General c++ 20-based use cleanup
- std::format use
- Consider switch to cmake, or otherwise address make depend issue
  and provide cmake project file / package so easy for people using cmake to just use Stroika
  https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html
- Modules support
- OpenAPI integration (maybe automatic methods, or at least validation).
- Some sort of GUI integration (perhaps ) - CEF (https://code.google.com/p/chromiumembedded/) or Gekko/Mozilla, or WebKit/V8
- Finish cleanup of IO::FileSystem code (really getting rid of most or all of it- unless I decide worth virtual fs for zipfiles and remote S3 etc)?
