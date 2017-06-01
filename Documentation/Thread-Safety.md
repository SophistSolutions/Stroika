***Thread Safety***

   Stroika classes respect two different levels of thread safety:
   * <a href='#C++-Standard-Thread-Safety'>C++-Standard-Thread-Safety</a>
   * <a href='#Internally-Synchronized-Thread-Safety'>Internally-Synchronized-Thread-Safety</a>
   * <a href='#Must-Externally-Synchronize-Letter-Thread-Safety'>Must-Externally-Synchronize-Letter-Thread-Safety</a>

---

<h3><a name='C++-Standard-Thread-Safety'>C++-Standard-Thread-Safety</a></h3>

   From [ http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2013/n3690.pdf section 17.6.5.9 /3 ]
   > A C++ standard library function shall not directly or indirectly modify objects (1.10) accessible by threads other
   > than the current thread unless the objects are accessed directly or indirectly via the function's non-const arguments, including this.

   So basically this means you can call multiple read operations on the object without concern (threadsafe), but mixing read and write
   must be somehow externally synchonized (suggestion - see Execution::Synchronized&lt;&gt;)

   *Most* Stroika code follows this principle.

   *Most* such Stroika classes also protect their internal representation integrity with Debug::AssertExternallySynchonized in Debug builds

---

<h3><a name='Internally-Synchronized-Thread-Safety'>Internally-Synchronized-Thread-Safety</a></h3>
  Callers can freely mix readers and writers. The class automatically
   manages assuring maintainance of a consistent internal state.

  Note also that this automatic synchonization MAY be maintained using mutexes,
  or possible via transactional memories, or other techniques.

  *Example* Execution::BlockingQueue

---

<h3><a name='C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter'>C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter</a></h3>

	<p>
	This has all the constraints of <a href='C++-Standard-Thread-Safety'>C++-Standard-Thread-Safety</a>.
	</p>
	<p>
	Plus, the caller must somehow arrange to synchonize access to the underlying object pointed to by the stream. So if the stream is shared (more than one thread accessing a copy of its rep)
	the caller must somehow synchonize those accesses.
	</p>

---

<h3><a name='C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter'>C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter</a></h3>
	<p>
	This has all the constraints of <a href='C++-Standard-Thread-Safety'>C++-Standard-Thread-Safety</a>.
	</p>
	<p>
	To manage access to the letter (rep), you may be in the case of <a href='C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter'>C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter</a>
	or <a href='C++-Standard-Thread-Safety-Letter-Internally-Synchonized'>C++-Standard-Thread-Safety-Letter-Internally-Synchonized</a>, depending on the ultimate origin of the underlying stream rep.
	</p>

---

<h3><a name='C++-Standard-Thread-Safety-Letter-Internally-Synchonized'>C++-Standard-Thread-Safety-Letter-Internally-Synchonized</a></h3>
	<p>
	This has all the constraints of <a href='C++-Standard-Thread-Safety'>C++-Standard-Thread-Safety</a>.
	</p>
	<p>
	The underlying rep stream is internally synchronized, so it can be used safely from multiple threads, without any additional checking/worries.
	</p>

---



OLD/OBSOLETE
<h3><a name='Must-Externally-Synchronize-Letter-Thread-Safety'>Must-Externally-Synchronize-Letter-Thread-Safety</a></h3>
  ***todo - CLEANUP THESE DOCS *** MAKE SURE WE HANDLE THE CASE OF STREAMS - ENVOLEOPE MUST BE PROTECTED, BUT INSIDE STREAM
     CLASS DEPERNEDN - DOC/CLARIFY***

  This is a very weak form of thread safety promise. It means that two separate instances can be safely used from two differnt threads.

 Execpt if you copy one to another. This is for envelope/letter classes that act as smart pointers, and where the underlying internal
 object is not synchonized.

 For example, a Stream object, or a DirectoryIterator, for example.

 What you create and copy around within one thread and destroy is perfectly safe, but allowing one of these objects to be copied can lead to bugs.

 These will generally be protoected internally with Debug::AssertExternallySynchonized on their internal 'shared reps' so that any mis-use will generally be detected in debug builds.

   *Example* Streams::BufferedInputStream
