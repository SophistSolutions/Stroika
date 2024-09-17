# Debugging with Stroika

## GDB and ASAN (Address Sanitizer) and UBSAN (Undefined Behavior Sanitizer)

* br __ubsan::ScopedReport::~ScopedReport or __ubsan::Diag::~Diag
* br __asan::ReportGenericError

## TSAN (thread sanitizer) on Ubuntu 24.04 (Host - not container) and later

- see https://stackoverflow.com/questions/77850769/fatal-threadsanitizer-unexpected-memory-mapping-when-running-on-linux-kernels

~~~
	sudo cat /proc/sys/vm/mmap_rnd_bits < /dev/null
    if result > 28 - seems to default to 32 now must do
        sudo sysctl vm.mmap_rnd_bits=28
    or TSAN will fail

    NOTE - even though failure in container, and test can be done in either place, the SET must be done in the host (or special docker settings)
~~~

- NOTE - though I've found no clear docs on this - setting rnd_bits=28
  also appears to fix an issue on Ubuntu 23.10 (setting it in Ubuntu 24.04 host) - regtests for ASAN - where sporadically we get crash on startup of ASAN test code --LGP 2024-06-12


## Valgrind (memcheck, and helgrind)

### MemCheck

Stroika is tested with valgrind memcheck for each release. REVISIT to see if/how useful this is, since I cannot recall ever finding
anything useful with it - just a bunch of false positives and effort to workaround stuff that was tearfully slow.

### Helgrind

I spent TONS of effort over the years to study things it reported, and craft workarounds for obvious false positive warnings.
I eventually gave up (as what use it migth have is presumably provided better by TSAN).

Sadly - they just don't really appear to support std c++ threading primitives (except sometimes where they fallback neatly on POSIX primitives).
But things (and I have alot of them) - that use std::atomic, just flat out get mis-diagnosed, and I grew weary of the silencing and efforts to
help it to understand with follows etc annotations.

So - as of Stroika v3 - no more helgrind support.