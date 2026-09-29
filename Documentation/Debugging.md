# Debugging with Stroika {#Debugging-with-Stroika}

## @todo DOC ABOUT setting up vscode and using .natvis files

## GDB and ASAN (Address Sanitizer) and UBSAN (Undefined Behavior Sanitizer)

* br __ubsan::ScopedReport::~ScopedReport or __ubsan::Diag::~Diag
* br __asan::ReportGenericError
- see https://stackoverflow.com/questions/30809022/how-can-i-break-on-ubsan-reports-in-gdb-and-continue

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


## Valgrind memcheck

### MemCheck

Each release runs the regression tests under valgrind memcheck, on Ubuntu 24.04 and 26.04 (the
`valgrind-release-SSLPurify-NoBlockAlloc` configuration). It earns its place by finding what the sanitizers cannot: reads
of uninitialized memory, including inside third-party code. Its first run on 26.04, for example, caught libstdc++'s
`from_chars` reading past the end of its input for `long double` inf/nan
([GCC PR 127666](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=127666)) - which ASan missed, because the stray `strlen` usually
found a zero byte before leaving the buffer.

- **Release builds only.** Memcheck finds the same classes of bug either way, and a Debug build under valgrind takes hours.
- **Block allocation off**, since memcheck cannot see inside `BlockAllocator`'s pools
  ([#1181](https://github.com/SophistSolutions/Stroika/issues/1181)).
- **Fewer iterations, not skips.** Valgrind runs one thread at a time and is far slower than the sanitizers, so a test that
  is slow under it cuts its loop counts when `Debug::IsRunningUnderValgrind ()` rather than skipping. Memcheck gets its value
  from the first few iterations; busy-spin handoffs between threads are especially slow.
- **Suppressions** live in `Tests/Valgrind-MemCheck-Common.supp`, empty as of 2026-09 (every earlier entry had stopped
  matching). Add one only for a confirmed false positive or third-party problem, noting where it was seen and why.

### Helgrind and DRD: not supported

Valgrind's race detectors do not understand std::atomic - they mis-diagnose it unless every handoff is annotated - and
Stroika is built on it. Years of false positives, suppressions and annotations bought nothing TSAN does not already find
(and TSAN understands atomics natively), so as of Stroika v3 they are not supported.