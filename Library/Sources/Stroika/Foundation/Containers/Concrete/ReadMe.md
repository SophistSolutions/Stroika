This folder contains all the Stroika Library Foundation::Containers::Concrete source code.

	The Concrete modules are particular implementations of a high level container access
	pattern, such as using a B-Tree, or HashTable. Selectable here are also different thread
	locking patterns.

	Think of these a bit like factories. If you want to use a particular underlying data structure
	for a container, just select it here ,and assign. E.g.

		Sequence<int> s = Concrete::Sequence_DoublyLinkedList<int> ();

