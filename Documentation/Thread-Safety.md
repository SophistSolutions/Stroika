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
	</div>

	<h3><a name='Automatically-Synchronized-Thread-Safety'>Automatically-Synchronized-Thread-Safety</a></h3>
	<div>
		Callers can freely mix readers and writers. The class automatically
		manages assuring maintainance of a consistent internal state.

		Most Stroika classes use this.
		
		Note also that this automatic synchonization MAY be maintained using mutexes,
		or possible via transactional memories, or other techniques.
	</div>

		
		
