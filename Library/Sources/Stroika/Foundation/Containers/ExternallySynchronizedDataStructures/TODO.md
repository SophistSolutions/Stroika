TODO:

	(o)		make sure ALL the interesting concrete containers (e.g. Sequence_Array<>) take move constructors with thier
			backend type, and that these are efficient (really move).

			I THINK this goes a long way towards addressing KDJs concerns about performance / single threading.

	(o)		MUST THEN - start really cleaning these classes up! EVEN MORE.

			ANd proabbly redo thier appraoch to iteration to match STLs

	(o)		I think we can mostly, or perhaps totoaly lose TRAITs from Array/LinkedList/DoubleLinkedList
			Just pass in templated compare function to functionst that need it.
