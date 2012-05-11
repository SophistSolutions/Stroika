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




		//	class	SharedPtr_Default_Traits<T>
		template	<typename	T>
			SharedPtr_Default_Traits<T>::Envelope::Envelope (TTYPE* ptr, ReferenceCountObjectType* countHolder)
				: fPtr_ (ptr)
				, fCountHolder_ (countHolder)
				{
					if (fPtr_ != nullptr and countHolder == nullptr) {
						fCountHolder_ = new ReferenceCountObjectType ();
					}
				}





		//	class	SharedPtr<T,T_TRAITS>
			template	<typename T, typename T_TRAITS>
				inline	SharedPtr<T,T_TRAITS>::SharedPtr ()
					: fEnvelope_ (nullptr, nullptr)
					{
					}
			template	<typename T, typename T_TRAITS>
				inline	SharedPtr<T,T_TRAITS>::SharedPtr (T* from)
					: fEnvelope_ (from, nullptr)
					{
						if (from != nullptr) {
							Assert (fEnvelope_.CurrentRefCount () == 0);
							fEnvelope_.Increment ();
						}
					}
			template	<typename T, typename T_TRAITS>
				inline	SharedPtr<T,T_TRAITS>::SharedPtr (T* from, typename T_TRAITS::ReferenceCountObjectType* useCounter)
					: fEnvelope_ (from, from == nullptr? nullptr: useCounter)
					{
						if (from != nullptr) {
							fEnvelope_.Increment ();
						}
					}
			template	<typename T, typename T_TRAITS>
				inline	SharedPtr<T,T_TRAITS>::SharedPtr (const SharedPtr<T,T_TRAITS>& from)
					: fEnvelope_ (from.fEnvelope_)
					{
						if (fEnvelope_.GetPtr () != nullptr) {
							fEnvelope_.Increment ();
						}
					}
			template	<typename T, typename T_TRAITS>
					template <typename T2, typename T2_TRAITS>
						SharedPtr<T,T_TRAITS>::SharedPtr (const SharedPtr<T2, T2_TRAITS>& from)
							: fEnvelope_ (from.fEnvelope_)
							{
								if (fEnvelope_.GetPtr () != nullptr) {
									fEnvelope_.Increment ();
								}
							}
			template	<typename T, typename T_TRAITS>
				inline	SharedPtr<T,T_TRAITS>& SharedPtr<T,T_TRAITS>::operator= (const SharedPtr<T,T_TRAITS>& rhs)
					{
						if (rhs.fEnvelope_.GetPtr () != fEnvelope_.GetPtr ()) {
							if (fEnvelope_.GetPtr () != nullptr) {
								if (fEnvelope_.Decrement ()) {
									delete fEnvelope_.GetPtr ();
									fEnvelope_.SetPtr (nullptr);
								}
							}
							fEnvelope_ = rhs.fEnvelope_;
							if (fEnvelope_.GetPtr () != nullptr) {
								Assert (fEnvelope_.CurrentRefCount () > 0);
								fEnvelope_.Increment ();
							}
						}
						return *this;
					}
			template	<typename T, typename T_TRAITS>
				inline	SharedPtr<T,T_TRAITS>::~SharedPtr ()
					{
						if (fEnvelope_.GetPtr () != nullptr) {
							if (fEnvelope_.Decrement ()) {
								delete fEnvelope_.GetPtr ();
							}
						}
					}
			template	<typename T, typename T_TRAITS>
				inline	bool	SharedPtr<T,T_TRAITS>::IsNull () const
					{
						return fEnvelope_.GetPtr () == nullptr;
					}
			template	<typename T, typename T_TRAITS>
				inline	T&	SharedPtr<T,T_TRAITS>::GetRep () const
					{
						RequireNotNull (fEnvelope_.GetPtr ());
						Assert (fEnvelope_.CurrentRefCount () > 0);
						return *fEnvelope_.GetPtr ();
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
						return fEnvelope_.GetPtr ();
					}
			template	<typename T, typename T_TRAITS>
				inline	T*	SharedPtr<T,T_TRAITS>::get () const
					{
						return (fEnvelope_.GetPtr ());
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
						if (fEnvelope_.GetPtr () != p) {
							*this = SharedPtr<T,T_TRAITS> (p);
						}
					}
			template	<typename T, typename T_TRAITS>
					template <typename T2>
						SharedPtr<T2> SharedPtr<T,T_TRAITS>::Dynamic_Cast ()
							{
								return SharedPtr<T2> (dynamic_cast<T2*> (get ()), fEnvelope_.GetCounterPointer ());
							}
			template	<typename T, typename T_TRAITS>
				inline	size_t	SharedPtr<T,T_TRAITS>::CurrentRefCount () const
					{
						return fEnvelope_.CurrentRefCount ();
					}
			template	<typename T, typename T_TRAITS>
				inline	bool	SharedPtr<T,T_TRAITS>::IsUnique () const
					{
						return fEnvelope_.CurrentRefCount () == 1;
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
						return fEnvelope_.GetPtr () < rhs.fEnvelope_.GetPtr ();
					}
			template	<typename T, typename T_TRAITS>
				bool	SharedPtr<T,T_TRAITS>::operator<= (const SharedPtr<T,T_TRAITS>& rhs) const
					{
						// not technically legal to compare pointers this way, but its is legal to convert to int, and then compare, and
						// this does the same thing...
						//		-- LGP 2009-01-11
						return fEnvelope_.GetPtr () <= rhs.fEnvelope_.GetPtr ();
					}
			template	<typename T, typename T_TRAITS>
				bool	SharedPtr<T,T_TRAITS>::operator> (const SharedPtr<T,T_TRAITS>& rhs) const
					{
						// not technically legal to compare pointers this way, but its is legal to convert to int, and then compare, and
						// this does the same thing...
						//		-- LGP 2009-01-11
						return fEnvelope_.GetPtr () > rhs.fEnvelope_.GetPtr ();
					}
			template	<typename T, typename T_TRAITS>
				bool	SharedPtr<T,T_TRAITS>::operator>= (const SharedPtr<T,T_TRAITS>& rhs) const
					{
						// not technically legal to compare pointers this way, but its is legal to convert to int, and then compare, and
						// this does the same thing...
						//		-- LGP 2009-01-11
						return fEnvelope_.GetPtr () >= rhs.fEnvelope_.GetPtr ();
					}
			template	<typename T, typename T_TRAITS>
				bool	SharedPtr<T,T_TRAITS>::operator== (const SharedPtr<T,T_TRAITS>& rhs) const
					{
						return fEnvelope_.GetPtr () == rhs.fEnvelope_.GetPtr ();
					}
			template	<typename T, typename T_TRAITS>
				bool	SharedPtr<T,T_TRAITS>::operator!= (const SharedPtr<T,T_TRAITS>& rhs) const
					{
						return fEnvelope_.GetPtr () != rhs.fEnvelope_.GetPtr ();
					}

		}



		namespace	Execution {
			template	<typename T>
				inline	void	ThrowIfNull (const Memory::SharedPtr<T>& p)
					{
						if (p.get () == nullptr) {
							Execution::DoThrow (bad_alloc (), "ThrowIfNull (SharedPtr<T> ()) - throwing bad_alloc ()");
						}
					}
			template	<typename T, typename T_TRAITS>
				inline	void	ThrowIfNull (const Memory::SharedPtr<T,T_TRAITS>& p)
					{
						if (p.get () == nullptr) {
							Execution::DoThrow (bad_alloc (), "ThrowIfNull (SharedPtr<T,T_TRAITS> ()) - throwing bad_alloc ()");
						}
					}
		}


	}
}
#endif	/*_Stroika_Foundation_Memory_SharedPtr_inl_*/
