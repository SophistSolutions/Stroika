/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Memory_Shared_inl_
#define	_Stroika_Foundation_Memory_Shared_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"BlockAllocated.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Memory {

			typedef	uint32_t		Counter_Shared;			// MUST BE SAME TYPE AS Shared<T>::Counter
			extern	Counter_Shared*		sCounterList_Shared;
			extern	void				GetMem_Shared ();
	
			inline	Counter_Shared*	NewCounter_Shared ()
				{
					/*
					 * We assume this in our free/list scheme.
					 * Otherwise alloc union of two types (ptr and Counter_Shared)
					 */
					Assert (sizeof (Counter_Shared) >= sizeof (Counter_Shared*));
	
					if (sCounterList_Shared == nullptr) {
						GetMem_Shared ();
					}
					AssertNotNil (sCounterList_Shared);
					Counter_Shared*	result = sCounterList_Shared;
					sCounterList_Shared = *(Counter_Shared**)sCounterList_Shared;
					return (result);
				}
	
			inline	void	DeleteCounter_Shared (Counter_Shared* counter)
				{
					/*
					 * We assume this in our free/list scheme.
					 * Otherwise alloc union of two types (ptr and Counter_Shared)
					 */
					Assert (sizeof (Counter_Shared) >= sizeof (Counter_Shared*));
	
					RequireNotNil (counter);
					(*(Counter_Shared**)counter) = sCounterList_Shared;
					sCounterList_Shared = counter;
				}


			template <class T> 
				inline	Shared<T>::Shared (T* (*cloneFunction) (const T&), T* ptr) : 
					fPtr (ptr),
					fCount (nullptr),
					fCloner (cloneFunction)
					{
						Assert (sizeof (Counter_Shared) == sizeof (Counter));
						fCount = NewCounter_Shared ();
						AssertNotNil (fCount);
						(*fCount) = 1;
					}
			template <class T>
				inline	Shared<T>::Shared (const Shared<T>& src) :
					fPtr (src.fPtr),
					fCount (src.fCount),
					fCloner (src.fCloner)
					{
						Assert (sizeof (Counter_Shared) == sizeof (Counter));
						RequireNotNil (fCount);
						(*fCount)++;
					}
			template <class T>
				inline	const T*	Shared<T>::operator-> () const
					{
						EnsureNotNil (fPtr);
						return (fPtr);
					}
			template <class T> 
				inline	const T&	Shared<T>::operator* () const
					{
						EnsureNotNil (fPtr);
						return (*fPtr);
					}
			template	<class T>
				inline	const T*	Shared<T>::GetPointer () const
					{
						return (fPtr);
					}
			template	<class T>	inline	void	Shared<T>::Assure1Reference ()
				{
					if (CountReferences () != 1) {
						BreakReferences ();
					}
				}


			template	<class	T>	
				inline	bool	operator== (const Shared<T>& lhs, const Shared<T>& rhs)
					{
						return (lhs.GetPointer () == rhs.GetPointer ());
					}
			template	<class	T>
				inline	bool	operator!= (const Shared<T>& lhs, const Shared<T>& rhs)
					{
						return (lhs.GetPointer () != rhs.GetPointer ());
					}



			template <class T>
				Shared<T>::~Shared ()
					{
						if (fCount != nullptr) {	// could be nullptr in presence of exceptions
							Require (*fCount >= 1);
							(*fCount)--;
							/*
							 * When reference count drops to zero, delete both the counter,
							 * and the item we are reference counting. NB: fPtr could be
							 * nullptr at this point.
							 */
							if (*fCount == 0) {
								DeleteCounter_Shared (fCount);
								delete fPtr;
							}
						}
					}
			template <class T>
				Shared<T>& Shared<T>::operator= (const Shared<T>& src)
					{
						/*
							* Careful to avoid *this = *this - do nothing.
							*
							* Unreference old item (deleting if ours is the last reference) and
							* add a new reference to the rhs.
							*
							* NB: fPtr can still be nullptr here.
							*/
						if (fPtr != src.fPtr) {
							RequireNotNil (fCount);
							Require (*fCount >= 1);
							(*fCount)--;
							/*
								* When reference count drops to zero, delete both the counter,
								* and the item we are reference counting. NB: fPtr could be
								* nullptr at this point.
								*/
							if (*fCount == 0) {
								DeleteCounter_Shared (fCount);
								delete fPtr;
							}
							fPtr = src.fPtr;
							fCount = src.fCount;
							fCloner = src.fCloner;
							(*fCount)++;
						}
						return (*this);
					}
			template <class T>
				Shared<T>& Shared<T>::operator= (T* pointer)
					{
						/*
						 * Again: careful to avoid *this = *this - do nothing.
						 *
						 * Unreference old item (deleting if ours is the last reference) and
						 * add a new reference to the rhs.
						 *
						 * NB: fPtr and pointer can still be nullptr here.
						 */
						if (fPtr != pointer) {
							RequireNotNil (fCount);
							Require (*fCount >= 1);
							(*fCount)--;
							/*
							 * One subtle point here is that we only allocate a new
							 * reference count if *fCount != 0 - this is because we
							 * would be deleting that counter here, and then allocating
							 * a new one, so why bother.
							 */
							if (*fCount == 0) {
								delete fPtr;
							}
							else {
								fCount = NewCounter_Shared ();
							}
							fPtr = pointer;
							*fCount = 1;
						}
						return (*this);
					}
			template <class T>
				T* Shared<T>::operator-> ()
					{
						/*
						 * For non-const pointing, we must clone ourselves (if there are
						 * extra referneces).
						 */
						Assure1Reference ();
						EnsureNotNil (fPtr);
						return (fPtr); 
					}
			template <class T>
				T& Shared<T>::operator* ()
					{
						/*
						 * For non-const dereferencing, we must clone ourselves (if there are
						 * extra referneces).
						 */
						Assure1Reference ();
						EnsureNotNil (fPtr);
						return (*fPtr); 
					}
			template <class T>
				T* Shared<T>::GetPointer ()
					{
						/*
						 * For non-const pointing, we must clone ourselves (if there are
						 * extra referneces). If we are a nullptr pointer, nobody could actually
						 * rereference it anyhow, so don't bother with the Assure1Reference()
						 * in that case.
						 */
						if (fPtr == nullptr) {
							return (nullptr);
						}
						Assure1Reference ();
						EnsureNotNil (fPtr);
						return (fPtr); 
					}
			template <class T>
				void	Shared<T>::BreakReferences ()
					{
						/*
						 *		For a valid pointer that is reference counted and multiply shared,
						 *	make a copy of that pointer via our fCloner function, and assign
						 *	that cloned reference to this.
						 *
						 *		Note that by doing so, we remove any references to the current
						 *	item, and end up with our having the sole reference to the new copy of fPtr.
						 *
						 *		Since we will be cloning the given pointer, we assume(assert) that
						 *	it is non-nullptr.
						 */
						RequireNotNil (fPtr);
						RequireNotNil (fCloner);
						AssertNotNil (fCount);

						Require (CountReferences () > 1);
						*this = ((*fCloner) (*fPtr));
						Ensure (CountReferences () == 1);
					}


		}
	}
}
#endif	/*_Stroika_Foundation_Memory_Shared_inl_*/
