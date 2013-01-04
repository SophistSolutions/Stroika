This Folder contains the Foundation::Execution source code.

	TODO: DOCUMENT BETTER THE DESIGN OF THREAD INTERUOPTION!!! (and overall orgnaizaiton of this module).

	Consider adding class BlockingQueue<E>
		http://docs.oracle.com/javase/7/docs/api/java/util/concurrent/BlockingQueue.html

		This - I THINK - essentially replaces the need for WaitForMultipleObjects. Maybe add utility class that runs threads
		that wait on each individual object, and then POST an event to this Q when available. Then the caller can wait on events
		for that Q.

		...