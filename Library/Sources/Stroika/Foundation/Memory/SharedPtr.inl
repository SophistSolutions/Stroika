/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Memory_SharedPtr_inl_
#define	_Stroika_Foundation_Memory_SharedPtr_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"../Execution/AtomicOperations.h"

#include	"BlockAllocated.h"

namespace	Stroika {
	namespace	Foundation {
		namespace	Memory {


			namespace	SharedPtrNS {
				namespace	Private {
					struct	SimpleSharedPtrBase : SharedPtrBase {
						public:
							virtual	void	DO_DELETE_REF_CNT ();

						public:
							DECLARE_USE_BLOCK_ALLOCATION(SimpleSharedPtrBase);
					};
				}
			}



		//	class	SharedPtrBase
			inline	SharedPtrBase::SharedPtrBase ():
				fCount_DONT_ACCESS (0)
				{
				}
			inline	SharedPtrBase::~SharedPtrBase ()
				{
				}



		//	class	SharedPtr<T>
			template	<typename T>
				inline	SharedPtr<T>::SharedPtr ():
					fPtr (nullptr),
					fCountHolder (nullptr)
					{
					}
			template	<typename T>
				inline	SharedPtr<T>::SharedPtr (T* from):
					fPtr (from),
					fCountHolder (nullptr)
					{
						if (from != nullptr) {
							fCountHolder = DEBUG_NEW SharedPtrNS::Private::SimpleSharedPtrBase ();
							Assert (fCountHolder->fCount_DONT_ACCESS == 0);
							Execution::AtomicIncrement (&fCountHolder->fCount_DONT_ACCESS);
						}
					}
			template	<typename T>
				inline	SharedPtr<T>::SharedPtr (UsesSharedPtrBase, T* from):
					fPtr (from),
					fCountHolder (from)
					{
						if (fCountHolder != nullptr) {
							Execution::AtomicIncrement (&fCountHolder->fCount_DONT_ACCESS);
						}
					}
			template	<typename T>
				inline	SharedPtr<T>::SharedPtr (T* from, SharedPtrBase* useCounter):
					fPtr (from),
					fCountHolder (from == nullptr? nullptr: useCounter)
					{
						if (fCountHolder != nullptr) {
							Execution::AtomicIncrement (&fCountHolder->fCount_DONT_ACCESS);
						}
					}
			template	<typename T>
				inline	SharedPtr<T>::SharedPtr (const SharedPtr<T>& from):
					fPtr (from.fPtr),
					fCountHolder (from.fCountHolder)
					{
						if (fPtr != nullptr) {
							RequireNotNull (fCountHolder);
							Execution::AtomicIncrement (&fCountHolder->fCount_DONT_ACCESS);
						}
					}
			template	<typename T>
				inline	SharedPtr<T>& SharedPtr<T>::operator= (const SharedPtr<T>& rhs)
					{
						if (rhs.fPtr != fPtr) {
							if (fPtr != nullptr) {
								AssertNotNull (fCountHolder);
								Assert (fCountHolder->fCount_DONT_ACCESS > 0);
								if (Execution::AtomicDecrement (&fCountHolder->fCount_DONT_ACCESS) == 0) {
									fCountHolder->DO_DELETE_REF_CNT ();
									delete fPtr;
									fCountHolder = nullptr;
									fPtr = nullptr;
								}
							}
							fPtr = rhs.fPtr;
							fCountHolder = rhs.fCountHolder;
							if (fPtr != nullptr) {
								AssertNotNull (fCountHolder);
								Assert (fCountHolder->fCount_DONT_ACCESS > 0);
								Execution::AtomicIncrement (&fCountHolder->fCount_DONT_ACCESS);
							}
						}
						return *this;
					}
			template	<typename T>
				inline	SharedPtr<T>::~SharedPtr ()
					{
						if (fPtr != nullptr) {
							AssertNotNull (fCountHolder);
							Assert (fCountHolder->fCount_DONT_ACCESS > 0);
							if (
								Execution::AtomicDecrement (&fCountHolder->fCount_DONT_ACCESS) == 0
								) {
								fCountHolder->DO_DELETE_REF_CNT ();
								delete fPtr;
							}
						}
					}
			template	<typename T>
				inline	bool	SharedPtr<T>::IsNull () const
					{
						return fPtr == nullptr;
					}
			template	<typename T>
				/*
				@METHOD:		SharedPtr<T>::GetRep
				@DESCRIPTION:	<p>Asserts that the pointer is non-nullptr.</p>
				*/
				inline	T&	SharedPtr<T>::GetRep () const
					{
						AssertNotNull (fPtr);
						AssertNotNull (fCountHolder);
						Assert (fCountHolder->fCount_DONT_ACCESS >= 1);
						return *fPtr;
					}
			template	<typename T>
				/*
				@METHOD:		SharedPtr<T>::operator->
				@DESCRIPTION:	<p>Note - this CAN NOT return nullptr (because -> semantics are typically invalid for a logically null pointer)</p>
				*/
				inline	T* SharedPtr<T>::operator-> () const
					{
						return &GetRep ();
					}
			template	<typename T>
				/*
				@METHOD:		SharedPtr<T>::operator*
				@DESCRIPTION:	<p></p>
				*/
				inline	T& SharedPtr<T>::operator* () const
					{
						return GetRep ();
					}
			template	<typename T>
				/*
				@METHOD:		SharedPtr<T>::operator->
				@DESCRIPTION:	<p>Note - this CAN return nullptr</p>
				*/
				inline	SharedPtr<T>::operator T* () const
					{
						return fPtr;
					}
			template	<typename T>
				/*
				@METHOD:		SharedPtr<T>::get
				@DESCRIPTION:	<p>Mimic the 'get' API of the std::auto_ptr&lt;T&gt; class. Just return the pointed to object, with no
							asserts about it being non-null.</p>
				*/
				inline	T*	SharedPtr<T>::get () const
					{
						return (fPtr);
					}
			template	<typename T>
				/*
				@METHOD:		SharedPtr<T>::release
				@DESCRIPTION:	<p>Mimic the 'get' API of the std::auto_ptr&lt;T&gt; class. Make this pointer nullptr, but first return the
							pre-existing pointer value. Note - if there were more than one references to the underlying object, its not destroyed.
							<br>
							NO - Changed API to NOT return old pointer, since COULD have been destroyed, and leads to buggy coding.
							If you want the pointer before release, explicitly call get () first!!!
							</p>
				*/
				inline	void	SharedPtr<T>::release ()
					{
						*this = SharedPtr<T> (nullptr);
					}
			template	<typename T>
				/*
				@METHOD:		SharedPtr<T>::clear
				@DESCRIPTION:	<p>Synonymn for SharedPtr<T>::release ()
							</p>
				*/
				inline	void	SharedPtr<T>::clear ()
					{
						release ();
					}
			template	<typename T>
				/*
				@METHOD:		SharedPtr<T>::reset
				@DESCRIPTION:	<p>Mimic the 'get' API of the std::auto_ptr&lt;T&gt; class. Make this pointer 'p', but first return the
							pre-existing pointer value. Unreference any previous value. Note - if there were more than one references to the underlying object, its not destroyed.</p>
				*/
				inline	void	SharedPtr<T>::reset (T* p)
					{
						if (fPtr != p) {
							*this = SharedPtr<T> (p);
						}
					}
			template	<typename T>
				inline	void	SharedPtr<T>::Assure1Reference (T* (*copier) (const T&))
					{
						RequireNotNil (copier);
						if (not IsUnique ()) {
							BreakReferences_ (copier);
						}
					}
			template <class T>
				void	SharedPtr<T>::BreakReferences_ (T* (*copier) (const T&))
					{
						RequireNotNil (copier);
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
						RequireNotNull (fPtr);

						Require (CurrentRefCount () > 1);
						*this = ((*copier) (*fPtr));
						Ensure (CurrentRefCount () == 1);
					}
			template	<typename T>
				/*
				@METHOD:		SharedPtr<T>::CurrentRefCount
				@DESCRIPTION:	<p>I used to keep this available only for debugging, but I've found a few cases where its handy outside the debugging context
				so not its awlays avaialble (it has no cost to keep available).</p>
				*/
				inline	size_t	SharedPtr<T>::CurrentRefCount () const
					{
						return fCountHolder==nullptr? 0: fCountHolder->fCount_DONT_ACCESS;
					}
			template	<typename T>
				inline	bool	SharedPtr<T>::IsUnique () const
					{
						return fCountHolder == NULL? false: fCountHolder->fCount_DONT_ACCESS == 1;
					}
			template	<typename T>
				inline	bool	SharedPtr<T>::unique () const
					{
						// respect the stl-ish names
						return IsUnique ();
					}
			template	<typename T>
				bool	SharedPtr<T>::operator< (const SharedPtr<T>& rhs) const
					{
						// not technically legal to compare pointers this way, but its is legal to convert to int, and then compare, and
						// this does the same thing...
						//		-- LGP 2009-01-11
						return fPtr < rhs.fPtr;
					}
			template	<typename T>
				bool	SharedPtr<T>::operator<= (const SharedPtr<T>& rhs) const
					{
						// not technically legal to compare pointers this way, but its is legal to convert to int, and then compare, and
						// this does the same thing...
						//		-- LGP 2009-01-11
						return fPtr <= rhs.fPtr;
					}
			template	<typename T>
				bool	SharedPtr<T>::operator> (const SharedPtr<T>& rhs) const
					{
						// not technically legal to compare pointers this way, but its is legal to convert to int, and then compare, and
						// this does the same thing...
						//		-- LGP 2009-01-11
						return fPtr > rhs.fPtr;
					}
			template	<typename T>
				bool	SharedPtr<T>::operator>= (const SharedPtr<T>& rhs) const
					{
						// not technically legal to compare pointers this way, but its is legal to convert to int, and then compare, and
						// this does the same thing...
						//		-- LGP 2009-01-11
						return fPtr >= rhs.fPtr;
					}
			template	<typename T>
				bool	SharedPtr<T>::operator== (const SharedPtr<T>& rhs) const
					{
						return fPtr == rhs.fPtr;
					}
			template	<typename T>
				bool	SharedPtr<T>::operator!= (const SharedPtr<T>& rhs) const
					{
						return fPtr != rhs.fPtr;
					}
			template	<typename T>
				SharedPtrBase*		SharedPtr<T>::_PEEK_CNT_PTR_ () const
					{
						return fCountHolder;
					}

		}



		namespace	Execution {
			template	<typename	T>
				void	ThrowIfNull (const Memory::SharedPtr<T>& p);
			template	<typename	T>
				inline	void	ThrowIfNull (const Memory::SharedPtr<T>& p)
					{
						if (p.get () == nullptr) {
							DbgTrace ("ThrowIfNull (SharedPtr<typename T> ()) - throwing bad_alloc ()");
							Execution::DoThrow (bad_alloc ());
						}
					}
		}


	}
}
#endif	/*_Stroika_Foundation_Memory_SharedPtr_inl_*/
