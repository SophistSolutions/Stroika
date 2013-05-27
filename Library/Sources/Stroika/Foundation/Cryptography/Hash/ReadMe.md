This Folder contains the Foundation::Cryptography::Hash source code.

	Each file repesents a different algorithm. Each follows a pattern layed out
	in the Hasher<> template pattern.
	
	Conceptually its best for each to just operate on a stream, but for performance
	reasons, we must also allow Hash of individual ints etc.
	
	Then - other parts of stroika can uniformly select and utilize hash algoriuthms
	like the jenkins algirithm...
	
	
	(VERY PROTOTYPE DEFINITION AT THIS STAGE)
	


o	Adapters take other types – like string, or stream, etc, and maybe template things todo other types, and let use plug in an int-based algorithm, and ??
