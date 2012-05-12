/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Memory_SharedPtrBase_inl_
#define	_Stroika_Foundation_Memory_SharedPtrBase_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"../Execution/AtomicOperations.h"


namespace	Stroika {
	namespace	Foundation {
		namespace	Memory {






		//	class	SharedPtrBase
			inline	SharedPtrBase::SharedPtrBase ():
				fCount_ (0)
				{
				}
			inline	SharedPtrBase::~SharedPtrBase ()
				{
				}





		//	class	Private::SharedPtrBase_Default_Traits_Helpers_::Envelope_<T>
			template	<typename	T>
				inline	Private::SharedPtrBase_Default_Traits_Helpers_::Envelope_<T>::Envelope_ (T* ptr, T* ptr2)
						: fPtr (ptr)
					{
						// Either they must be the same, or hte ptr2 (counter object) must be null - telling us to make a new one...
						Require (ptr == ptr2 or ptr2 == nullptr);
					}
			template	<typename	T>
				template <typename T2>
					Private::SharedPtrBase_Default_Traits_Helpers_::Envelope_<T>::Envelope_ (const Envelope_<T2>& from)
						: fPtr (from.fPtr)
					{
					}
			template	<typename	T>
				inline	T*	Private::SharedPtrBase_Default_Traits_Helpers_::Envelope_<T>::GetPtr () const 	
					{
						return fPtr;
					}
			template	<typename	T>
				inline	void	Private::SharedPtrBase_Default_Traits_Helpers_::Envelope_<T>::SetPtr (T* p)
					{
						fPtr = p;
					}
			template	<typename	T>
				inline	typename Private::SharedPtrBase_Default_Traits_Helpers_::ReferenceCountType_	Private::SharedPtrBase_Default_Traits_Helpers_::Envelope_<T>::CurrentRefCount () const
					{
						return fPtr==nullptr? 0: fPtr->fCount_;
					}
			template	<typename	T>
				inline	void	Private::SharedPtrBase_Default_Traits_Helpers_::Envelope_<T>::Increment ()
					{
						RequireNotNull (fPtr);
						Execution::AtomicIncrement (&fPtr->fCount_);
					}
			template	<typename	T>
				inline	bool	Private::SharedPtrBase_Default_Traits_Helpers_::Envelope_<T>::Decrement ()
					{
						Require (CurrentRefCount () > 0);
						if (Execution::AtomicDecrement (&fPtr->fCount_) == 0) {
							return true;
						}
						return false;
					}
			template	<typename	T>
				inline	Memory::SharedPtrBase*	Private::SharedPtrBase_Default_Traits_Helpers_::Envelope_<T>::GetCounterPointer () const
					{
						return fPtr;
					}


		}


	}
}
#endif	/*_Stroika_Foundation_Memory_SharedPtrBase_inl_*/
