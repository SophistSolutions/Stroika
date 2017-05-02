\page thread_safety Thread Safety

	<div>
	Stroika classes respect two different levels of thread safety
	</div>
	
	<ul>
		<li>POD-Level-Thread-Safety</li>
		<li>Automatically-Synchronized-Thread-Safety</li>
	</ul>
	

	<h3><a name='POD-Level-Thread-Safety'>POD-Level-Thread-Safety</a></h3>
	<div>
		Like POD (plain old data) types. External synchonization is required
		
		Different object instances can be freely used in different threads,
		but a given instance can only be safely used 
		(read + write, or write + write) from a single thread at a time

		<span>OLD - GET RID OF THIS DESIGNATION</span>
	</div>

	<h3><a name='Automatically-Synchronized-Thread-Safety'>Automatically-Synchronized-Thread-Safety</a></h3>
	<div>
		DEPRECATED - USE Internally-Synchronized-Thread-Safety
	</div>


	<h3><a name='Internally-Synchronized-Thread-Safety'>Internally-Synchronized-Thread-Safety</a></h3>
	<div>
		Callers can freely mix readers and writers. The class automatically
		manages assuring maintainance of a consistent internal state.

		Note also that this automatic synchonization MAY be maintained using mutexes,
		or possible via transactional memories, or other techniques.
	
		<span>RECONSIDER THIS DESIGNATION - not sure we use anymore</span>
	</div>


		

	<h3><a name='C++-Standard-Thread-Safety'>C++-Standard-Thread-Safety</a></h3>
	<div>
		[17.6.5.9/3] A C++ standard library function shall not directly or indirectly modify objects (1.10) accessible by threads other than the current thread unless the objects are accessed directly or indirectly via the function’s non-const arguments, including this.
		<br/>
		So basically this means you can call multiple read operations on the object without concern (threadsafe), but mixing read and write
		must be somehow externally synchonized (suggestion - Execution::Synchronized)
	</div>



	<h3><a name='Must-Externally-Synchronize-Letter-Thread-Safety'>Must-Externally-Synchronize-Letter-Thread-Safety</a></h3>
	<div>
		This is a very weak form of thread safety promise. It means that two separate instances can be safely used from two differnt threads.

		Execpt if you copy one to another. This is for envelope/letter classes that act as smart pointers, and where the underlying internal
		object is not synchonized.

		For example, a Stream object, or a DirectoryIterator, for example.

		What you create and copy around within one thread and destroy is perfectly safe, but allowing one of these objects to be copied can lead to bugs.

		These will generally be protoected internally with Debug::AssertExternallySynchonized on their internal 'shared reps' so that any mis-use will generally be detected in debug builds.
	</div>





	<h3>DOCS TO ADD - ILLORGANIZED</h3>
	<div>
		<ul>
			<li>DOCUMENT /? DEAL WITH THE ISSUE OF deadlocks: Apply/FindFirstThat 
			and lambda - use Iterator<> directly instead of apply to avoid deadlocks.
			Advantage of using Apply is its faster - since it acuires locks once
			and logically atomic
			</li>
			
			<li>
			</li>
			
		</ul>
	</div>

		
		
