/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Memory_WeakPtr_h_
#define	_Stroika_Foundation_Memory_WeakPtr_h_	1

#include	"../StroikaPreComp.h"

#include	<list>
#include	<memory>

#include	"../Configuration/Common.h"

#include	"SharedPtr.h"




/*
 *		TODO:
 *
 *
 *			o	Basic idea is - maybe single link list in SHARPEREPENVOELVEP of REPS for WeakPtr. 
 *				Then critical section added to that sharedenvoelelp. Then crit section whwn degremennt refcount to zeor before 
 *				delete and then also critsectiona round SHARENVOLEOPE LOCK - which - NO - CRITSECITON MUS TGO OUTSIDE DECRMENT TO ZERO
 *
 *			o	Then actual WeakPtr class becomes trivial - its lock calls envolopelock to get SharedPtr<T>. Not much else to it.
 *			o	WeakPtr contains PLAIN BARE pointer to SharedPtr_WEAKREF_ENVOLOPE.
 *
 */






namespace	Stroika {	
	namespace	Foundation {
		namespace	Memory {


#if 0
			template	<typename T, typename BaseSharedPtrTraits>
				class	WeakSharedPtrRep {
				};



			// This is an implementation detail of the WeakPtr's 'SharedPtr' traits class. It is the extended 'Envelope' class used by the SharedPtr
			// including info needed for the WeakPtr tracking
			template	<typename T, typename BaseSharedPtrTraits>
				struct	WeakSharedPtrEnvelope_ : BaseSharedPtrTraits::Envelope {
					list<WeakSharedPtrRep<T,BaseSharedPtrTraits>*>	fWeakPtrs;
				};

			/*
			 * To use the WeakSharedPtr<> - you need to construct a special SharedPtr<> with specially augmented traits. These
			 * are the specially augmented traits to use on the SharedPtr<> template. The BaseSharedPtrTraits are typically equal to SharedPtr_Default_Traits<T>.
			 */
			template	<typename T, typename BaseSharedPtrTraits>
				struct	WeakSharedPtrTraits : BaseSharedPtrTraits {
				};


			template	<typename T, typename BaseSharedPtrTraits = SharedPtr_Default_Traits<T>>
				class	WeakSharedPtr {
					public:
						WeakSharedPtr ();

					public:
						/*
						 * This is how you get a 'SharedPtr' from ....
						 */
						nonvirtual	WeakSharedPtr<T,BaseSharedPtrTraits>	Lock () const;
				};


			/*
			 * The builtin SharedPtr<> doesn't support automatic WeakPtr tracking. You must use a specialized 'WeakPtrTraits' todo that. This class template takes the base 
			 */
			template	<typename T, typename BaseSharedPtrTraits = SharedPtr_Default_Traits<T>>
				class	WeakCapableSharedPtr : public SharedPtr<T,WeakSharedPtrTraits<BaseSharedPtrTraits>> {
					WeakCapableSharedPtr ()
						: SharedPtr<T,WeakSharedPtrTraits<BaseSharedPtrTraits>> ()
						{
						}
					explicit WeakCapableSharedPtr (T* from)
						: SharedPtr<T,WeakSharedPtrTraits<BaseSharedPtrTraits>> (from)
						{
						}
					explicit WeakCapableSharedPtr (T* from, typename T_TRAITS::ReferenceCounterContainerType* useCounter)
						: SharedPtr<T,WeakSharedPtrTraits<BaseSharedPtrTraits>> (from, useCounter)
						{
						}
					WeakCapableSharedPtr (const WeakCapableSharedPtr<T,T_TRAITS>& from)
						: SharedPtr<T,WeakSharedPtrTraits<BaseSharedPtrTraits>> (from)
						{
						}

					template <typename T2, typename T2_TRAITS>
						WeakCapableSharedPtr (const WeakCapableSharedPtr<T2, T2_TRAITS>& from)
							: SharedPtr<T,WeakSharedPtrTraits<BaseSharedPtrTraits>> (from)
							{
							}

				};
#endif

		}
	}
}
#endif	/*_Stroika_Foundation_Memory_WeakPtr_h_*/







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"WeakPtr.inl"
