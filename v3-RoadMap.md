# Stroika v3 Roadmap

Rough outline of (informal/weak) plans for Stroika v3

- Start January 2022
- Improve HTTP server
  - inline zip
  - https
  - http 2??
  - http 3?
- Concepts use in containers
- General c++ 20-based use cleanup
- std::format use
- Consider switch to cmake, or otherwise address make depend issue
- Modules support
- OpenAPI integration (maybe automatic methods, or at least validation).
- Some sort of GUI integration (perhaps ) - CEF (https://code.google.com/p/chromiumembedded/) or Gekko/Mozilla, or WebKit/V8
- Finish cleanup of IO::FileSystem code (really getting rid of most or all of it- unless I decide worth virtual fs for zipfiles and remote S3 etc)?