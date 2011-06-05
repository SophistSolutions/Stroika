/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_MemoryAllocator_inl
#define	_MemoryAllocator_inl	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace	Stroika {	
	namespace	Foundation {

namespace	MemoryAllocator {
	// class	MemoryAllocator::AbstractGeneralPurposeAllocator
	inline	AbstractGeneralPurposeAllocator::~AbstractGeneralPurposeAllocator ()
		{
		}
}



namespace	MemoryAllocator {
	// class	MemoryAllocator::STLAllocator<T,BASE_ALLOCATOR>
	template <typename T, typename BASE_ALLOCATOR>
		inline	typename STLAllocator<T,BASE_ALLOCATOR>::pointer STLAllocator<T,BASE_ALLOCATOR>::address (typename STLAllocator<T,BASE_ALLOCATOR>::reference _Val) const
			{
				return (&_Val);
			}

	template <typename T, typename BASE_ALLOCATOR>
		inline	typename	STLAllocator<T,BASE_ALLOCATOR>::const_pointer STLAllocator<T,BASE_ALLOCATOR>::address (typename STLAllocator<T,BASE_ALLOCATOR>::const_reference _Val) const
			{
				return (&_Val);
			}
	template <typename T, typename BASE_ALLOCATOR>
		inline	STLAllocator<T,BASE_ALLOCATOR>::STLAllocator ():
			fBaseAllocator ()
			{
			}
	template <typename T, typename BASE_ALLOCATOR>
		inline	STLAllocator<T,BASE_ALLOCATOR>::STLAllocator (const STLAllocator<T,BASE_ALLOCATOR>& from):
			fBaseAllocator (from.fBaseAllocator)
			{
			}
	template <typename T, typename BASE_ALLOCATOR>
		inline	void STLAllocator<T,BASE_ALLOCATOR>::deallocate (pointer _Ptr, size_type)
			{
				if (_Ptr != NULL) {
					fBaseAllocator.Deallocate (_Ptr);
				}
			}
	template <typename T, typename BASE_ALLOCATOR>
		inline	typename STLAllocator<T,BASE_ALLOCATOR>::pointer STLAllocator<T,BASE_ALLOCATOR>::allocate (size_type _Count)
			{
				// check for integer overflow
				if (_Count <= 0)
					_Count = 0;
				else if (((_SIZT)(-1) / _Count) < sizeof (T))
					_THROW_NCEE(std::bad_alloc, NULL);
				// allocate storage for _Count elements of type T
				return ((T _FARQ *)fBaseAllocator.Allocate (_Count * sizeof (T)));
			}
	template <typename T, typename BASE_ALLOCATOR>
		inline	typename STLAllocator<T,BASE_ALLOCATOR>::pointer STLAllocator<T,BASE_ALLOCATOR>::allocate (size_type _Count, const void _FARQ *)
			{
				return (allocate (_Count));
			}
	template <typename T, typename BASE_ALLOCATOR>
		inline	void	STLAllocator<T,BASE_ALLOCATOR>::construct (pointer _Ptr, const T& _Val)
			{
				_Construct (_Ptr, _Val);
			}
	template <typename T, typename BASE_ALLOCATOR>
		inline	void	STLAllocator<T,BASE_ALLOCATOR>::destroy (pointer _Ptr)
			{
				_Destroy (_Ptr);
			}
	template <typename T, typename BASE_ALLOCATOR>
		inline	_SIZT	STLAllocator<T,BASE_ALLOCATOR>::max_size () const throw ()
			{
				_SIZT _Count = (_SIZT)(-1) / sizeof (T);
				return (0 < _Count ? _Count : 1);
			}

}

	}
}


#endif	/*_MemoryAllocator_inl*/
