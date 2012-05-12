/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
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





		//	class	SharedPtr_SharedPtrBase_Traits<T>::Envelope
			template	<typename	T>
				inline	SharedPtr_SharedPtrBase_Traits<T>::Envelope::Envelope (TTYPE* ptr)
						: fPtr (ptr)
					{
					}
			template	<typename	T>
				inline	typename SharedPtr_SharedPtrBase_Traits<T>::TTYPE*	SharedPtr_SharedPtrBase_Traits<T>::Envelope::GetPtr () const 	
					{
						return fPtr;
					}
			template	<typename	T>
				inline	void	SharedPtr_SharedPtrBase_Traits<T>::Envelope::SetPtr (typename SharedPtr_SharedPtrBase_Traits<T>::TTYPE* p)
					{
						fPtr = p;
					}
			template	<typename	T>
				inline	typename SharedPtr_SharedPtrBase_Traits<T>::ReferenceCountType	SharedPtr_SharedPtrBase_Traits<T>::Envelope::CurrentRefCount () const
					{
						return fPtr==nullptr? 0: fPtr->fCount_DONT_ACCESS;
					}
			template	<typename	T>
				inline	void	SharedPtr_SharedPtrBase_Traits<T>::Envelope::Increment ()
					{
						RequireNotNull (fPtr);
						Execution::AtomicIncrement (&fPtr->fCount_DONT_ACCESS);
					}
			template	<typename	T>
				inline	bool	SharedPtr_SharedPtrBase_Traits<T>::Envelope::Decrement ()
					{
						Require (CurrentRefCount () > 0);
						if (Execution::AtomicDecrement (&fPtr->fCount_DONT_ACCESS) == 0) {
							return true;
						}
						return false;
					}
			template	<typename	T>
				inline	typename	SharedPtr_SharedPtrBase_Traits<T>::ReferenceCountObjectType*	SharedPtr_SharedPtrBase_Traits<T>::Envelope::GetCounterPointer () const
					{
						return fPtr;
					}


		}


	}
}
#endif	/*_Stroika_Foundation_Memory_SharedPtrBase_inl_*/
