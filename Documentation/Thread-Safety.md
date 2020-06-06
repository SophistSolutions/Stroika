# **_Thread Safety_**

Stroika classes respect two different levels of thread safety:

- <a href='#C++-Standard-Thread-Safety'>C++-Standard-Thread-Safety</a>
- <a href='#Internally-Synchronized-Thread-Safety'>Internally-Synchronized-Thread-Safety</a>
- <a href='#Must-Externally-Synchronize-Letter-Thread-Safety'>Must-Externally-Synchronize-Letter-Thread-Safety</a>

---

## <a name='C++-Standard-Thread-Safety'>C++-Standard-Thread-Safety</a>

From [ http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2013/n3690.pdf section 17.6.5.9 /3 ]

> A C++ standard library function shall not directly or indirectly modify objects (1.10) accessible by threads other
> than the current thread unless the objects are accessed directly or indirectly via the function's non-const arguments, including this.

So basically this means you can call multiple read operations on the object without concern (threadsafe), but mixing read and write
must be somehow externally synchronized (suggestion - see Execution::Synchronized<>)

_Most_ Stroika code follows this principle.

_Most_ such Stroika classes also protect their internal representation integrity with Debug::AssertExternallySynchronized in Debug builds

---

## <a name='Internally-Synchronized-Thread-Safety'>Internally-Synchronized-Thread-Safety</a>

Callers can freely mix readers and writers. The class automatically
manages assuring maintainance of a consistent internal state.

Note also that this automatic synchonization MAY be maintained using mutexes,
or possible via transactional memories, or other techniques.

_Example_ Execution::BlockingQueue

---

## <a name='C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter'>C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter</a>

This has all the constraints of <a href='C++-Standard-Thread-Safety'>C++-Standard-Thread-Safety</a>.

Plus, the caller must somehow arrange to synchonize access to the underlying object pointed to by the stream. So if the stream is shared (more than one thread accessing a copy of its rep)
the caller must somehow synchonize those accesses.

---

## <a name='C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter'>C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter/a>

This has all the constraints of <a href='C++-Standard-Thread-Safety'>C++-Standard-Thread-Safety</a> for the top level Ptr / shared_ptr / envelope object.

If or the type of syncrhonization required on the underlying 'letter' or 'rep' depends on the source of that object. See the OBJECT::New () static method used to construct that
shared rep object.

---

## <a name='Thread-Safety-Rules-Depends-On-Subtype'>Thread-Safety-Rules-Depends-On-Subtype</a>

For this abstract class, the thread safety rules are left ambiguous. The type of syncrhonization required for this object depends on the (concrete) subtype.

See the OBJECT::New () static method used to construct that
shared rep object.

---

## <a name='C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter'>C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter</a>

This has all the constraints of <a href='C++-Standard-Thread-Safety'>C++-Standard-Thread-Safety</a>.

To manage access to the letter (rep), you may be in the case of <a href='C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter'>C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter</a>
or <a href='C++-Standard-Thread-Safety-Letter-Internally-Synchronized'>C++-Standard-Thread-Safety-Letter-Internally-Synchronized</a>, depending on the ultimate origin of the underlying stream rep.

---

## <a name='Rep-Inside-Ptr-Is-Internally-Synchronized'>Rep-Inside-Ptr-Is-Internally-Synchronized</a>

    <p>
    For methods that return a smart pointer, the thread safety of that type are defined by the type itself, but this designates that the returned value
    is a rep which is internally synchronized.
    </p>

---

## <a name='Rep-Inside-Ptr-Must-Be-Externally-Synchronized'>Rep-Inside-Ptr-Must-Be-Externally-Synchronized</a>

For methods that return a smart pointer, the thread safety of that type are defined by the type itself, but this designates that the returned value
is a rep which is must be externally synchronized (and probably is checked via Debug::AssertExternallySynchronized).

---

## <a name='C++-Standard-Thread-Safety-Letter-Internally-Synchronized'>C++-Standard-Thread-Safety-Letter-Internally-Synchronized</a>

This has all the constraints of <a href='C++-Standard-Thread-Safety'>C++-Standard-Thread-Safety</a> for the Ptr object.

The underlying rep stream is internally synchronized, so it can be used safely from multiple threads, without any additional checking/worries.

---

OLD/OBSOLETE

<h3><a name='Must-Externally-Synchronize-Letter-Thread-Safety'>Must-Externally-Synchronize-Letter-Thread-Safety</a></h3>
  ***todo - CLEANUP THESE DOCS *** MAKE SURE WE HANDLE THE CASE OF STREAMS - ENVOLEOPE MUST BE PROTECTED, BUT INSIDE STREAM
     CLASS DEPERNEDN - DOC/CLARIFY***

This is a very weak form of thread safety promise. It means that two separate instances can be safely used from two different threads.

Execpt if you copy one to another. This is for envelope/letter classes that act as smart pointers, and where the underlying internal
object is not synchronized.

For example, a Stream object, or a DirectoryIterator, for example.

What you create and copy around within one thread and destroy is perfectly safe, but allowing one of these objects to be copied can lead to bugs.

These will generally be protoected internally with Debug::AssertExternallySynchronized on their internal 'shared reps' so that any mis-use will generally be detected in debug builds.

_Example_ Streams::BufferedInputStream
