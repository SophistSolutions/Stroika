/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Memory_MemoryAllocator_inl_
#define	_Stroika_Foundation_Memory_MemoryAllocator_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace	Stroika {	
	namespace	Foundation {
		namespace	Memory {
			// class	Memory::AbstractGeneralPurposeAllocator
			inline	AbstractGeneralPurposeAllocator::~AbstractGeneralPurposeAllocator ()
				{
				}

		// class	Memory::STLAllocator<T,BASE_ALLOCATOR>
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
						else if (((size_t)(-1) / _Count) < sizeof (T))
						     throw std::bad_alloc ();
						// allocate storage for _Count elements of type T
						return ((T*)fBaseAllocator.Allocate (_Count * sizeof (T)));
					}
			template <typename T, typename BASE_ALLOCATOR>
				inline	typename STLAllocator<T,BASE_ALLOCATOR>::pointer STLAllocator<T,BASE_ALLOCATOR>::allocate (size_type _Count, const void*)
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
				inline	size_t	STLAllocator<T,BASE_ALLOCATOR>::max_size () const throw ()
					{
						return std::numeric_limits<size_type>::max() / sizeof(T);
					}
		}
	}
}
#endif	/*_Stroika_Foundation_Memory_MemoryAllocator_inl_*/
