# **_Thread Safety_**

Stroika classes respect several different levels of thread safety:

## Principle

- <a href='#C++-Standard-Thread-Safety'>C++-Standard-Thread-Safety</a>
  
  This is by FAR the most common, and represents how the C++ standard library approaches thread safety.

- <a href='#Internally-Synchronized-Thread-Safety'>Internally-Synchronized-Thread-Safety</a>

  Certain utiltiies that are meant for sharing data between threads, manage their state/syncrhonization
  fully internally.

- <a href='#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter'>C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter</a>

  Standard C++ thread safety for the envelope, but whether underlying shared rep is synchronized depends on how that rep was allocated.


## Less Common


- <a href='#C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized'>C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized</a>

- <a href='#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter'>C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>

- <a href='#Thread-Safety-Rules-Depends-On-Subtype'>Thread-Safety-Rules-Depends-On-Subtype</a>





---

## <a name='C++-Standard-Thread-Safety'>C++-Standard-Thread-Safety</a>

From [ http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2013/n3690.pdf section 17.6.5.9 /3 ]

> A C++ standard library function shall not directly or indirectly modify objects (1.10) accessible by threads other
> than the current thread unless the objects are accessed directly or indirectly via the function's non-const arguments, including this.

So basically this means you can call multiple read operations on the object without concern (threadsafe), but mixing read and write
must be somehow externally synchronized (suggestion - see Execution::Synchronized<>, or std::mutex<>)

_Most_ Stroika code follows this principle.

_Most_ such Stroika classes also protect their internal representation integrity with Debug::AssertExternallySynchronized in Debug builds


#### Examples:
  - Characters::String


---

## <a name='Internally-Synchronized-Thread-Safety'>Internally-Synchronized-Thread-Safety</a>

Callers can freely mix readers and writers. The class automatically
manages assuring maintainance of a consistent internal state.

Note also that this automatic synchonization MAY be maintained using mutexes,
or possibly via transactional memories, or other techniques.

#### Examples:
  - Execution::BlockingQueue
  - template <> class Cache::SynchronizedLRUCache
  - Execution::Logger

---

## <a name='C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized'>C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized</a>

This (the envelope 'Ptr' class) has all the constraints of <a href='C++-Standard-Thread-Safety'>C++-Standard-Thread-Safety</a>.

The underlying rep stream is internally synchronized, so it can be used safely from multiple threads, without any additional checking/worries.

#### Examples:
  - Execution::Thread

---

## <a name='C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter'>C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>

This (the envelope 'Ptr' class) has all the constraints of <a href='C++-Standard-Thread-Safety'>C++-Standard-Thread-Safety</a>.

Plus, the caller must somehow arrange to synchonize access to the underlying object pointed to by the Ptr. So if the underlying object (e.g. Stream) is shared (more than one thread accessing a copy of its rep)
the caller must somehow synchonize those accesses.

Typically the rep will check via Debug::AssertExternallySynchronized.

---

## <a name='C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter'>C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter</a>

This (the envelope 'Ptr' class) has all the constraints of <a href='C++-Standard-Thread-Safety'>C++-Standard-Thread-Safety</a>.

This threadsafety means the object is:
  - <a href='#C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized'>C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized</a> OR
  - <a href='#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter'>C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>

You just cannot tell which based on the type itself (you need to know the type of the underlying contained letter object).

If or the type of syncrhonization required on the underlying 'letter' or 'rep' depends on the source of that object. See the OBJECT::New () static method used to construct that
shared rep object.

#### Examples:
  - template <typename ELEMENT_TYPE> class InputStream<ELEMENT_TYPE>::Ptr

    The underlying shared part of the input stream could be internally synhconized or not, depending on how it was created.

---

## <a name='Thread-Safety-Rules-Depends-On-Subtype'>Thread-Safety-Rules-Depends-On-Subtype</a>

For this abstract class, the thread safety rules are left ambiguous. The type of syncrhonization required for this object depends on the (concrete) subtype.

Depending on the subclass, this might be:
  - <a href='#C++-Standard-Thread-Safety'>C++-Standard-Thread-Safety</a> OR
  - <a href='#Internally-Synchronized-Thread-Safety'>Internally-Synchronized-Thread-Safety</a>

#### Examples:
  - template <typename ELEMENT_TYPE> class OutputStream<ELEMENT_TYPE>::_IRep;

    Some sub-types will be internally syncrhonized, and others C++-Standard-Thread-Safety

