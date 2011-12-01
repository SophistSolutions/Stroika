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
				inline	T&	SharedPtr<T>::GetRep () const
					{
						RequireNotNull (fPtr);
						AssertNotNull (fCountHolder);
						Assert (fCountHolder->fCount_DONT_ACCESS >= 1);
						return *fPtr;
					}
			template	<typename T>
				inline	T* SharedPtr<T>::operator-> () const
					{
						return &GetRep ();
					}
			template	<typename T>
				inline	T& SharedPtr<T>::operator* () const
					{
						return GetRep ();
					}
			template	<typename T>
				inline	SharedPtr<T>::operator T* () const
					{
						return fPtr;
					}
			template	<typename T>
				inline	T*	SharedPtr<T>::get () const
					{
						return (fPtr);
					}
			template	<typename T>
				inline	void	SharedPtr<T>::release ()
					{
						*this = SharedPtr<T> (nullptr);
					}
			template	<typename T>
				inline	void	SharedPtr<T>::clear ()
					{
						release ();
					}
			template	<typename T>
				inline	void	SharedPtr<T>::reset (T* p)
					{
						if (fPtr != p) {
							*this = SharedPtr<T> (p);
						}
					}
			template	<typename T>
				inline	size_t	SharedPtr<T>::CurrentRefCount () const
					{
						return fCountHolder==nullptr? 0: fCountHolder->fCount_DONT_ACCESS;
					}
			template	<typename T>
				inline	bool	SharedPtr<T>::IsUnique () const
					{
						return fCountHolder == nullptr? false: fCountHolder->fCount_DONT_ACCESS == 1;
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
				inline	void	ThrowIfNull (const Memory::SharedPtr<T>& p)
					{
						if (p.get () == nullptr) {
							Execution::DoThrow (bad_alloc (), "ThrowIfNull (SharedPtr<typename T> ()) - throwing bad_alloc ()");
						}
					}
		}


	}
}
#endif	/*_Stroika_Foundation_Memory_SharedPtr_inl_*/
