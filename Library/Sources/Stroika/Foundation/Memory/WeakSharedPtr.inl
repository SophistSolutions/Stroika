/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Memory_WeakSharedPtr_inl_
#define	_Stroika_Foundation_Memory_WeakSharedPtr_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace	Stroika {
	namespace	Foundation {
		namespace	Memory {


			// class	WeakSharedPtr<T,BASE_SharedPtr_TRAITS>
			template	<typename T, typename BASE_SharedPtr_TRAITS>
				inline	WeakSharedPtr<T,BASE_SharedPtr_TRAITS>::WeakSharedPtr ()
					: fRep_ ()
					{
					}
			template	<typename T, typename BASE_SharedPtr_TRAITS>
				WeakSharedPtr<T,BASE_SharedPtr_TRAITS>::WeakSharedPtr (const WeakSharedPtr<T,BASE_SharedPtr_TRAITS>& from)
					: fRep_ ()
					{
					}
			template	<typename T, typename BASE_SharedPtr_TRAITS>
					WeakSharedPtr<T,BASE_SharedPtr_TRAITS>::WeakSharedPtr (const SharedPtrType& from)
						: fRep_ ()
					{
					}
			template	<typename T, typename BASE_SharedPtr_TRAITS>
				const WeakSharedPtr<T,BASE_SharedPtr_TRAITS>& WeakSharedPtr<T,BASE_SharedPtr_TRAITS>::operator= (const WeakSharedPtr<T,BASE_SharedPtr_TRAITS>& rhs)
					{
						fRep_ = rhs.fRep_;
						return *this;
					}
			template	<typename T, typename BASE_SharedPtr_TRAITS>
				typename WeakSharedPtr<T,BASE_SharedPtr_TRAITS>::SharedPtrType	WeakSharedPtr<T,BASE_SharedPtr_TRAITS>::Lock () const
					{
						AssertNotImplemented ();
						return WeakSharedPtr<T,BASE_SharedPtr_TRAITS>::SharedPtrType ();
					}


		}
	}
}
#endif	/*_Stroika_Foundation_Memory_WeakSharedPtr_inl_*/
