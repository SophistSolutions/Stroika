# Recognizing Stroika's Recurring Patterns {#Patterns}

[Design-Overview.md](Design-Overview.md) explains the *why* behind Stroika's conventions. This doc is a faster, more mechanical companion: a handful of patterns recur across almost every module in [Foundation](../Library/Sources/Stroika/Foundation/) and [Frameworks](../Library/Sources/Stroika/Frameworks/), and once you can recognize them on sight, you can classify an unfamiliar file in seconds instead of re-deriving its architecture from scratch — whether you're a new contributor skimming the tree, or an AI assistant doing the same.

Each pattern below lists what to grep for, a couple of real examples, and how it differs from its nearest lookalike (that's usually the part that isn't obvious from the name alone).

## Quick lookup

| Pattern | Recognize by | Example |
| --- | --- | --- |
| [Envelope + IRep](#Envelope-IRep) | nested `class IRep` / `_IRep`, envelope holds `shared_ptr<IRep>` | `Collection<T>`, `Stream<T>`, `Iterator<T>` |
| [COW via SharedByValue](#COW) | `SharedByValue<` in the rep's storage member | `Iterable<T>` (and hence all containers) |
| [Immutable-shared value](#Immutable-Shared) | rep has no mutating virtuals; plain `shared_ptr` held | `String`, `BLOB`, `VariantValue` |
| [Provider behind a facade](#Provider-Facade) | facade namespace + `Providers::X::` impl gated by `#if qStroika_HasComponent_X` | `SSL::`, `PKI::`, `SQL::` |
| [Archetype vs Concrete](#Archetype-Concrete) | abstract container archetype + swappable `Concrete::X_Y` backends | `Sequence<T>` vs `Concrete::Sequence_Array<T>` |
| [Debug-only race detection](#Debug-Race-Detection) | `Debug::AssertExternallySynchronizedMutex::ReadContext`/`WriteContext` locals | almost every rep method |
| [Real thread-safety wrapper](#Synchronized) | a member of type `Synchronized<T>`, accessed via `.cget()`/`.rwget()` | `ConnectionManager`, `Thread::Ptr`'s rep |
| [Compile-time contracts](#Compile-Time-Contracts) | `static_assert (some_concept<T>)` right after a class/template | Iterator category conformance |

---

## Envelope + IRep (letter-envelope / pimpl) {#Envelope-IRep}

The single most common pattern in the codebase. A thin, usually `nonvirtual`-only **envelope** class holds a smart pointer to an abstract nested **`IRep`** (sometimes `_IRep`) interface; concrete backends subclass the rep. The envelope's public methods just forward:

```c++
// Collection<T>::_IRep (Containers/Collection.h)
virtual void Add (ArgByValueType<value_type> item, Iterator<value_type>* oAddedI) = 0;

// Collection<T>::Add (Containers/Collection.inl)
inline void Collection<T>::Add (ArgByValueType<value_type> item) {
    _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (item, nullptr);
}
```

Two different **ownership strategies** show up under this same pattern, and it's worth telling them apart:

- **Shared, COW-eligible** — the envelope holds a `shared_ptr<IRep>` (via `SharedByValue`, see below). This is what `Iterable<T>`/`Collection<T>`/`Sequence<T>` do.
- **Unique, clone-on-copy** — the envelope holds a `unique_ptr<IRep>`, and copying the envelope deep-clones via a virtual `Clone()`. `Iterator<T>` does this deliberately — a design note in `Iterator.h` explains it was switched *away* from `SharedByValue` because iterator clones turn out to be rare, so the COW machinery wasn't paying for itself.

If you're not sure which one a given class uses, check the envelope's storage member — `SharedByValue<...>` vs `unique_ptr<...>`/`shared_ptr<...>` directly.

## Copy-on-write via `Memory::SharedByValue<T, TRAITS>` {#COW}

The generic COW engine underneath the shared-ownership case above (and underneath `String`'s base class, `Iterable<Character>`). Recognize it by a member like:

```c++
using _SharedByValueRepType = Memory::SharedByValue<_IRep, Memory::SharedByValueSupport::DefaultTraits<_IRep, &Clone_>>;
_SharedByValueRepType _fRep;
```

The mechanics, concretely: `rwget()` calls `AssureNOrFewerReferences`, which only clones when `use_count() > 1`. So `Collection<int> b = a;` is one atomic refcount bump (no data copied); `b.Add(5)` triggers the clone *only if* `a` is still aliasing the same rep — if `b` already has sole ownership, it mutates in place with no copy at all. `Memory::SharedByValue<int>` is guaranteed the same size as a `shared_ptr<int>` (`static_assert (sizeof (SharedByValue<int>) == sizeof (shared_ptr<int>))`), so this costs nothing extra to hold.

## Immutable-shared value types (no COW needed) {#Immutable-Shared}

Easy to mistake for the COW case because it also stores a `shared_ptr` to a rep — but the rep has **no mutating virtuals at all**, so no clone-on-write path ever fires. Copies are just shared references to read-only data, forever.

- `String`'s reps are immutable (`String.h`: *"String reps are IMMUTABLE"*) — real mutation is punted entirely to `StringBuilder`. This is despite `String` deriving from `Iterable<Character>`, whose `SharedByValue` COW machinery is present but simply never triggered for `String`.
- `BLOB` uses the same idea even more directly — no `SharedByValue` at all, just `shared_ptr<_IRep> fRep_` and a one-line comment: *"Copying a BLOB is just copying a shared_ptr."*
- `VariantValue` is type-erased rather than a real tagged union — `shared_ptr<IRep_>` where each concrete type lives in a template subclass `TIRep_<T>` — but is likewise immutable-shared under the hood.

If you're reading a doc comment that says something "uses COW" or "is similar to std::variant," it's worth checking which of these two buckets it's actually in — the *envelope* code can look identical either way.

## Provider behind a facade {#Provider-Facade}

Used wherever Stroika wraps an optional third-party dependency. A small facade namespace/class defines `IRep` + `Ptr` + an `Options` struct with no third-party symbols in it; the only real implementation lives under a `Providers::` subnamespace, compiled in only when the dependency is present:

```c++
// SSL::SocketStream::New (Cryptography/SSL/SocketStream.cpp)
#if qStroika_HasComponent_OpenSSL
    return Providers::OpenSSL::SocketStream::New (sd, Providers::OpenSSL::ClientContext::Options{o});
#else
    Throw (RequiredComponentMissingException{"SSL providing service"sv});
#endif
```

Same pattern in `Database/SQL` (`SQL::Connection` → `SQLite`/`ODBC` backends) and `PKI::Certificate` → `Providers::OpenSSL::Certificate`. Cross-casts between a facade `Ptr` and its provider-specific rep go through `dynamic_pointer_cast` and throw `bad_cast` on mismatch, so you can't accidentally hand a SQLite rep to code expecting a different backend.

## Archetype vs Concrete (swappable container backends) {#Archetype-Concrete}

Specific to containers, and easy to conflate with the general Envelope+IRep pattern above, but the distinguishing feature is that **multiple named concrete backends exist for the same archetype**, and callers can swap backends explicitly:

```c++
Set<int> s{1, 2, 3};               // default backend
s = Concrete::SortedSet_SkipList<int>{s};  // same archetype, different backend, same call sites work
```

Look for a `Concrete::` subnamespace with several `X_Array`, `X_LinkedList`, `X_stdvector`-style classes all implementing the same archetype's `_IRep` (e.g. `Concrete::Collection_Array<T>` implementing `Collection<T>::_IRep`). The default backend is chosen by a `Factory` at construction time, not hardcoded into the archetype.

## Debug-only race detection: `Debug::AssertExternallySynchronizedMutex` {#Debug-Race-Detection}

Not a real mutex — a zero-cost-in-release, assertion-based misuse detector, used pervasively inside rep methods:

```c++
virtual void Add (...) override {
    Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
    ...
}
```

If you see this, the object is documenting "the caller is responsible for synchronizing access to me" — it will *catch* concurrent misuse in debug builds (via assertion) but does nothing in release. See [Thread-Safety.md](Thread-Safety.md) for the full taxonomy. Don't confuse this with the next pattern.

## Real thread-safety wrapper: `Synchronized<T>` {#Synchronized}

The actual mutex-holding wrapper, for state that must be genuinely thread-safe at runtime (not just assertion-checked). Access is through `.cget()`/`.rwget()` (or `->` for the const/read case), which return RAII lock-holding reference objects:

```c++
auto lockedConfigData = fConfig_.rwget ();   // lock held for this object's lifetime
lockedConfigData->SomeValueChanged = 1;
```

Used for things like `ConnectionManager`'s active-connections list, or a `Thread::Ptr` rep's saved exception. If a class needs to be safe to call from multiple threads without the caller doing external locking, look for a `Synchronized<T>` member — that's the real synchronization; `AssertExternallySynchronizedMutex` above is documentation-and-detection, not enforcement.

## Compile-time contracts via concepts + `static_assert` {#Compile-Time-Contracts}

Rather than SFINAE/`enable_if`, Stroika states template requirements as named concepts (prefixed `I`, per the naming convention in Design-Overview.md) and then proves conformance immediately after the class definition:

```c++
static_assert (forward_iterator<Iterator<int>>);
static_assert (bidirectional_iterator<BidirectionalIterator<int>>);
static_assert (random_access_iterator<RandomAccessIterator<int>>);
```

This is worth checking when working on templated code — a missing or failing `static_assert` here is often the fastest way to catch a broken template method before it's ever instantiated by real calling code (some currently-unexercised methods in the codebase would fail to compile if called, precisely because no such assertion/instantiation currently forces the compiler to check them).

---

Once these seven patterns are familiar, most new Foundation or Frameworks code reduces to "which of these is this, and what's different about this particular instance" rather than a fresh read from first principles.
