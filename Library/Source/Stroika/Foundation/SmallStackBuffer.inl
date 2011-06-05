/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_SmallStackBuffer_inl
#define	_SmallStackBuffer_inl	1



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Assertions.h"
#include	"Memory.h"

namespace	Stroika {	
	namespace	Foundation {

		inline	void	xxx (size_t nElements)
		{
		}

//	class	SmallStackBuffer<T,BUF_SIZE>
	template	<typename	T, size_t BUF_SIZE>
		inline	void	SmallStackBuffer<T,BUF_SIZE>::GrowToSize (size_t nElements)
			{
				fSize = nElements;
				// very rare we'll need to grow past this limit. And we want to keep this routine small so it can be
				// inlined. And put the rare, complex logic in other outofline function
				if (nElements > (NEltsOf (fBuffer))) {
					GrowToSize_ (nElements);
				}
			}
	template	<typename	T, size_t BUF_SIZE>
		void	SmallStackBuffer<T,BUF_SIZE>::GrowToSize_ (size_t nElements)
			{
				#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
					#pragma push
					#pragma warn -8008
					#pragma warn -8066
				#endif
				Require (nElements > (NEltsOf (fBuffer)));
				// if we were using buffer, then assume whole thing, and if we malloced, save
				// size in unused buffer
				Assert (sizeof (fBuffer) >= sizeof (size_t));	// one customer changes the size of the buffer to 1, and wondered why it crashed...
				size_t	oldEltCount	=	(fPointer == fBuffer)?
											NEltsOf (fBuffer):
											*(size_t*)&fBuffer;
				#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
        				#pragma pop
				#endif
				if (nElements > oldEltCount) {
					/*
					*	If we REALLY must grow, the double in size so unlikely we'll have to grow/malloc/copy again.
					*/
					nElements = max (nElements, oldEltCount*2);

					#if		qVERYLargeOpNewFailsToFail
					if (nElements > 512*1024*1024) {
						ThrowOutOfMemoryException ();
					}
					#endif

					T*	newPtr = DEBUG_NEW T [nElements];			// NB: We are careful not to update our size field til this has succeeded (exception safety)

					// Not totally safe for T with CTOR/DTOR/Op= ... Don't use this class in that case!!!
					// No idea how many to copy!!! - do worst case(maybe should keep old size if this ever
					// bus errors???)
					::memcpy (newPtr, fPointer, oldEltCount*sizeof (T));
					if (fPointer != fBuffer) {
						// we must have used the heap...
						delete[] fPointer;
					}
					fPointer = newPtr;

					// since we are using the heap, we can store the size in our fBuffer
					*(size_t*)&fBuffer = nElements;
				}
			}
	template	<typename	T, size_t BUF_SIZE>
		inline	SmallStackBuffer<T,BUF_SIZE>::SmallStackBuffer (size_t nElements):
			fSize (0),
			//fBuffer (),
			fPointer (fBuffer)
			{
				GrowToSize (nElements);
			}
	template	<typename	T, size_t BUF_SIZE>
		SmallStackBuffer<T,BUF_SIZE>::SmallStackBuffer (const SmallStackBuffer<T,BUF_SIZE>& from):
			fSize (0),
			//fBuffer (),
			fPointer (fBuffer)
			{
				GrowToSize (from.fSize);
				std::copy (from.fPointer, from.fPointer + from.fSize, fPointer);
			}
	template	<typename	T, size_t BUF_SIZE>
		inline	SmallStackBuffer<T,BUF_SIZE>::~SmallStackBuffer ()
			{
				if (fPointer != fBuffer) {
					// we must have used the heap...
					delete[] fPointer;
				}
			}
	template	<typename	T, size_t BUF_SIZE>
		SmallStackBuffer<T,BUF_SIZE>&	SmallStackBuffer<T,BUF_SIZE>::operator= (const SmallStackBuffer<T,BUF_SIZE>& rhs)
			{
				GrowToSize (rhs.fSize);
				std::copy (rhs.fPointer, rhs.fPointer + rhs.fSize, fPointer);
				return *this;
			}
	template	<typename	T, size_t BUF_SIZE>
		inline	typename SmallStackBuffer<T,BUF_SIZE>::iterator		SmallStackBuffer<T,BUF_SIZE>::begin ()
			{
				return fPointer;
			}
	template	<typename	T, size_t BUF_SIZE>
		inline	typename SmallStackBuffer<T,BUF_SIZE>::iterator		SmallStackBuffer<T,BUF_SIZE>::end ()
			{
				return fPointer + fSize;
			}
	template	<typename	T, size_t BUF_SIZE>
		inline	typename SmallStackBuffer<T,BUF_SIZE>::const_iterator	SmallStackBuffer<T,BUF_SIZE>::begin () const
			{
				return fPointer;
			}
	template	<typename	T, size_t BUF_SIZE>
		inline	typename SmallStackBuffer<T,BUF_SIZE>::const_iterator	SmallStackBuffer<T,BUF_SIZE>::end () const
			{
				return fPointer + fSize;
			}
	template	<typename	T, size_t BUF_SIZE>
		inline	size_t	SmallStackBuffer<T,BUF_SIZE>::GetSize () const
			{
				return fSize;
			}
	template	<typename	T, size_t BUF_SIZE>
		inline	void	SmallStackBuffer<T,BUF_SIZE>::push_back (const T& e)
			{
				size_t	s	=	GetSize ();
				GrowToSize (s + 1);
				fPointer[s] = e;
			}
	#if		qCompilerBuggyOverloadingConstOperators
	template	<typename	T, size_t BUF_SIZE>
		inline	SmallStackBuffer<T,BUF_SIZE>::operator T* () const
			{
				AssertNotNil (fPointer);
				return (const_cast<T*> (fPointer));
			}
	#else
	template	<typename	T, size_t BUF_SIZE>
		inline	SmallStackBuffer<T,BUF_SIZE>::operator T* ()
			{
				AssertNotNil (fPointer);
				return (fPointer);
			}
	template	<typename	T, size_t BUF_SIZE>
		inline	SmallStackBuffer<T,BUF_SIZE>::operator const T* () const
			{
				AssertNotNil (fPointer);
				return (fPointer);
			}
	#endif



	}
}

#endif	/*_SmallStackBuffer_inl*/
