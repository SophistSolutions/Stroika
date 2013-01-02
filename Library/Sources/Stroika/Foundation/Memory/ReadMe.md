This Folder contains the Foundation::Memory source code.

	This module covers low level memory access/allocation sharing patterns. For example, shared pointers,
	copy on write (SharedByValue) code, a generic Variant-union (VariantValue), and optional values (Optional<T>).

	It also supports STL Allocators, memory debugging, and a BlockAllocation utility which is used internally
	in Stroika and CAN be used to greatly improve the efficiency of some memory access patterns.

	SmallStackBuffer<T> is a handy template that provides the semantics of allocating temporary memory (with malloc)
	on the stack, but typically avoids the overhead.

	Add Notes someplace on history of shared_ptr<> versus SharedPtr<>. And in those notes, consider re-instating
	my SharedPtr variant, as it maybe more performant. AND - I just tought of how it COULD be interoperable.
	When given a shared_ptr<T> - I could do a special rep that bumps refcount of shared_ptr, and has a refcnt.
	And similarly - that can produce a shared_ptr containing OURs (not thought through totally - careful about
	circularity - but MAYBE use weak_ptr). Also note if I were todo that - it would impose overhead and 
	not necessarily be desirable. And it could cause confusion. And use_count() wouldn't produce exactly the right
	answer.