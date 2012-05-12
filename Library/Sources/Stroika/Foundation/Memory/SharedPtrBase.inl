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
				fCount (0)
				{
				}
			inline	SharedPtrBase::~SharedPtrBase ()
				{
				}





		//	class	Private::SharedPtrBase_Envelope_<T>
			template	<typename	T>
				inline	Private::SharedPtrBase_Envelope_<T>::SharedPtrBase_Envelope_ (T* ptr, T* ptr2)
						: fPtr (ptr)
					{
						Require (ptr == ptr2);
					}
			template	<typename	T>
				template <typename T2>
					Private::SharedPtrBase_Envelope_<T>::SharedPtrBase_Envelope_ (const SharedPtrBase_Envelope_<T2>& from)
						: fPtr (from.fPtr)
					{
					}
			template	<typename	T>
				inline	T*	Private::SharedPtrBase_Envelope_<T>::GetPtr () const 	
					{
						return fPtr;
					}
			template	<typename	T>
				inline	void	Private::SharedPtrBase_Envelope_<T>::SetPtr (T* p)
					{
						fPtr = p;
					}
			template	<typename	T>
				inline	typename Private::ReferenceCountType_	Private::SharedPtrBase_Envelope_<T>::CurrentRefCount () const
					{
						return fPtr==nullptr? 0: fPtr->fCount;
					}
			template	<typename	T>
				inline	void	Private::SharedPtrBase_Envelope_<T>::Increment ()
					{
						RequireNotNull (fPtr);
						Execution::AtomicIncrement (&fPtr->fCount);
					}
			template	<typename	T>
				inline	bool	Private::SharedPtrBase_Envelope_<T>::Decrement ()
					{
						Require (CurrentRefCount () > 0);
						if (Execution::AtomicDecrement (&fPtr->fCount) == 0) {
							return true;
						}
						return false;
					}
			template	<typename	T>
				inline	typename	SharedPtrBase*	Private::SharedPtrBase_Envelope_<T>::GetCounterPointer () const
					{
						return fPtr;
					}


		}


	}
}
#endif	/*_Stroika_Foundation_Memory_SharedPtrBase_inl_*/
