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



		//	class	SharedPtr<T,T_TRAITS>
			template	<typename T, typename T_TRAITS>
				inline	SharedPtr<T,T_TRAITS>::SharedPtr ()
					: fPtr_ (nullptr)
					, fCountHolder_ (nullptr)
					{
					}
			template	<typename T, typename T_TRAITS>
				inline	SharedPtr<T,T_TRAITS>::SharedPtr (T* from)
					: fPtr_ (from)
					, fCountHolder_ (nullptr)
					{
						if (from != nullptr) {
							fCountHolder_ = DEBUG_NEW SharedPtrNS::Private::SimpleSharedPtrBase ();
							Assert (fCountHolder_->fCount_DONT_ACCESS == 0);
							Execution::AtomicIncrement (&fCountHolder_->fCount_DONT_ACCESS);
						}
					}
			template	<typename T, typename T_TRAITS>
				inline	SharedPtr<T,T_TRAITS>::SharedPtr (UsesSharedPtrBase, T* from)
					: fPtr_ (from)
					, fCountHolder_ (from)
					{
						if (fCountHolder_ != nullptr) {
							Execution::AtomicIncrement (&fCountHolder_->fCount_DONT_ACCESS);
						}
					}
			template	<typename T, typename T_TRAITS>
				inline	SharedPtr<T,T_TRAITS>::SharedPtr (T* from, SharedPtrBase* useCounter)
					: fPtr_ (from)
					, fCountHolder_ (from == nullptr? nullptr: useCounter)
					{
						if (fCountHolder_ != nullptr) {
							Execution::AtomicIncrement (&fCountHolder_->fCount_DONT_ACCESS);
						}
					}
			template	<typename T, typename T_TRAITS>
				inline	SharedPtr<T,T_TRAITS>::SharedPtr (const SharedPtr<T,T_TRAITS>& from)
					: fPtr_ (from.fPtr_)
					, fCountHolder_ (from.fCountHolder_)
					{
						if (fPtr_ != nullptr) {
							RequireNotNull (fCountHolder_);
							Execution::AtomicIncrement (&fCountHolder_->fCount_DONT_ACCESS);
						}
					}
			template	<typename T, typename T_TRAITS>
				inline	SharedPtr<T,T_TRAITS>& SharedPtr<T,T_TRAITS>::operator= (const SharedPtr<T,T_TRAITS>& rhs)
					{
						if (rhs.fPtr_ != fPtr_) {
							if (fPtr_ != nullptr) {
								AssertNotNull (fCountHolder_);
								Assert (fCountHolder_->fCount_DONT_ACCESS > 0);
								if (Execution::AtomicDecrement (&fCountHolder_->fCount_DONT_ACCESS) == 0) {
									fCountHolder_->DO_DELETE_REF_CNT ();
									delete fPtr_;
									fCountHolder_ = nullptr;
									fPtr_ = nullptr;
								}
							}
							fPtr_ = rhs.fPtr_;
							fCountHolder_ = rhs.fCountHolder_;
							if (fPtr_ != nullptr) {
								AssertNotNull (fCountHolder_);
								Assert (fCountHolder_->fCount_DONT_ACCESS > 0);
								Execution::AtomicIncrement (&fCountHolder_->fCount_DONT_ACCESS);
							}
						}
						return *this;
					}
			template	<typename T, typename T_TRAITS>
				inline	SharedPtr<T,T_TRAITS>::~SharedPtr ()
					{
						if (fPtr_ != nullptr) {
							AssertNotNull (fCountHolder_);
							Assert (fCountHolder_->fCount_DONT_ACCESS > 0);
							if (
								Execution::AtomicDecrement (&fCountHolder_->fCount_DONT_ACCESS) == 0
								) {
								fCountHolder_->DO_DELETE_REF_CNT ();
								delete fPtr_;
							}
						}
					}
			template	<typename T, typename T_TRAITS>
				inline	bool	SharedPtr<T,T_TRAITS>::IsNull () const
					{
						return fPtr_ == nullptr;
					}
			template	<typename T, typename T_TRAITS>
				inline	T&	SharedPtr<T,T_TRAITS>::GetRep () const
					{
						RequireNotNull (fPtr_);
						AssertNotNull (fCountHolder_);
						Assert (fCountHolder_->fCount_DONT_ACCESS >= 1);
						return *fPtr_;
					}
			template	<typename T, typename T_TRAITS>
				inline	T* SharedPtr<T,T_TRAITS>::operator-> () const
					{
						return &GetRep ();
					}
			template	<typename T, typename T_TRAITS>
				inline	T& SharedPtr<T,T_TRAITS>::operator* () const
					{
						return GetRep ();
					}
			template	<typename T, typename T_TRAITS>
				inline	SharedPtr<T,T_TRAITS>::operator T* () const
					{
						return fPtr_;
					}
			template	<typename T, typename T_TRAITS>
				inline	T*	SharedPtr<T,T_TRAITS>::get () const
					{
						return (fPtr_);
					}
			template	<typename T, typename T_TRAITS>
				inline	void	SharedPtr<T,T_TRAITS>::release ()
					{
						*this = SharedPtr<T,T_TRAITS> (nullptr);
					}
			template	<typename T, typename T_TRAITS>
				inline	void	SharedPtr<T,T_TRAITS>::clear ()
					{
						release ();
					}
			template	<typename T, typename T_TRAITS>
				inline	void	SharedPtr<T,T_TRAITS>::reset (T* p)
					{
						if (fPtr_ != p) {
							*this = SharedPtr<T,T_TRAITS> (p);
						}
					}
			template	<typename T, typename T_TRAITS>
				inline	size_t	SharedPtr<T,T_TRAITS>::CurrentRefCount () const
					{
						return fCountHolder_==nullptr? 0: fCountHolder_->fCount_DONT_ACCESS;
					}
			template	<typename T, typename T_TRAITS>
				inline	bool	SharedPtr<T,T_TRAITS>::IsUnique () const
					{
						return fCountHolder_ == nullptr? false: fCountHolder_->fCount_DONT_ACCESS == 1;
					}
			template	<typename T, typename T_TRAITS>
				inline	bool	SharedPtr<T,T_TRAITS>::unique () const
					{
						// respect the stl-ish names
						return IsUnique ();
					}
			template	<typename T, typename T_TRAITS>
				bool	SharedPtr<T,T_TRAITS>::operator< (const SharedPtr<T,T_TRAITS>& rhs) const
					{
						// not technically legal to compare pointers this way, but its is legal to convert to int, and then compare, and
						// this does the same thing...
						//		-- LGP 2009-01-11
						return fPtr_ < rhs.fPtr_;
					}
			template	<typename T, typename T_TRAITS>
				bool	SharedPtr<T,T_TRAITS>::operator<= (const SharedPtr<T,T_TRAITS>& rhs) const
					{
						// not technically legal to compare pointers this way, but its is legal to convert to int, and then compare, and
						// this does the same thing...
						//		-- LGP 2009-01-11
						return fPtr_ <= rhs.fPtr_;
					}
			template	<typename T, typename T_TRAITS>
				bool	SharedPtr<T,T_TRAITS>::operator> (const SharedPtr<T,T_TRAITS>& rhs) const
					{
						// not technically legal to compare pointers this way, but its is legal to convert to int, and then compare, and
						// this does the same thing...
						//		-- LGP 2009-01-11
						return fPtr_ > rhs.fPtr_;
					}
			template	<typename T, typename T_TRAITS>
				bool	SharedPtr<T,T_TRAITS>::operator>= (const SharedPtr<T,T_TRAITS>& rhs) const
					{
						// not technically legal to compare pointers this way, but its is legal to convert to int, and then compare, and
						// this does the same thing...
						//		-- LGP 2009-01-11
						return fPtr_ >= rhs.fPtr_;
					}
			template	<typename T, typename T_TRAITS>
				bool	SharedPtr<T,T_TRAITS>::operator== (const SharedPtr<T,T_TRAITS>& rhs) const
					{
						return fPtr_ == rhs.fPtr_;
					}
			template	<typename T, typename T_TRAITS>
				bool	SharedPtr<T,T_TRAITS>::operator!= (const SharedPtr<T,T_TRAITS>& rhs) const
					{
						return fPtr_ != rhs.fPtr_;
					}
			template	<typename T, typename T_TRAITS>
				SharedPtrBase*		SharedPtr<T,T_TRAITS>::_PEEK_CNT_PTR_ () const
					{
						return fCountHolder_;
					}

		}



		namespace	Execution {
			template	<typename T, typename T_TRAITS>
				inline	void	ThrowIfNull (const Memory::SharedPtr<T,T_TRAITS>& p)
					{
						if (p.get () == nullptr) {
							Execution::DoThrow (bad_alloc (), "ThrowIfNull (SharedPtr<typename T> ()) - throwing bad_alloc ()");
						}
					}
		}


	}
}
#endif	/*_Stroika_Foundation_Memory_SharedPtr_inl_*/
