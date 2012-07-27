This Folder contains the Foundation::Memory source code.

	This module covers low level memory access/allocation sharing patterns. For example, shared pointers,
	copy on write (SharedByValue) code, a generic Variant-union (VariantValue), and optional values (Optional<T>).

	It also supports STL Allocators, memory debugging, and a BlockAllocation utility which is used internally
	in Stroika and CAN be used to greatly improve the efficiency of some memory access patterns.

	SmallStackBuffer<T> is a handy template that provides the semantics of allocating temporary memory (with malloc)
	on the stack, but typically avoids the overhead.
