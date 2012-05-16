/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Memory_WeakSharedPtr_h_
#define	_Stroika_Foundation_Memory_WeakSharedPtr_h_	1

#include	"../StroikaPreComp.h"

#include	<list>
#include	<memory>

#include	"../Configuration/Common.h"

#include	"../Execution/CriticalSection.h"

#include	"SharedPtr.h"



//////EXTREMELY INCOMPLETE - JUST AT PROTOTYPE LEVEL!!!!



/*
 *		TODO:
 *
 *
 *			o	Basic idea is - maybe single link list in SHARPEREPENVOELVEP of REPS for WeakSharedPtr. 
 *				Then critical section added to that sharedenvoelelp. Then crit section when degremennt
 *				refcount to zeor before delete and then also critsectiona round SHARENVOLEOPE LOCK -
 *				which - NO - CRITSECITON MUS TGO OUTSIDE DECRMENT TO ZERO
 *
 *			o	Then actual WeakSharedPtr class becomes trivial - its lock calls envolopelock to get
 *				SharedPtr<T>. Not much else to it.
 *
 *			o	WeakSharedPtr contains PLAIN BARE pointer to SharedPtr_WEAKREF_ENVOLOPE.
 *
 */






namespace	Stroika {	
	namespace	Foundation {
		namespace	Memory {



			namespace	Private {

				template	<typename T, typename BASE_SharedPtr_TRAITS>
					class	WeakSharedPtrRep_;


				/*
				 * This is an implementation detail of the WeakSharedPtr's 'SharedPtr' traits class.
				 * It is the extended 'Envelope' class used by the SharedPtr
				 * including info needed for the WeakSharedPtr tracking.
				 */
				template	<typename T, typename BASE_SharedPtr_TRAITS>
					struct	WeakSharedPtrEnvelope_ : BASE_SharedPtr_TRAITS::Envelope {
						Execution::CriticalSection							fCriticalSection;
						list<WeakSharedPtrRep_<T,BASE_SharedPtr_TRAITS>*>	fWeakSharedPtrs;

						WeakSharedPtrEnvelope_ (T* ptr, typename BASE_SharedPtr_TRAITS::ReferenceCounterContainerType* countHolder)
							: BASE_SharedPtr_TRAITS::Envelope (ptr, countHolder)
							, fCriticalSection ()
							, fWeakSharedPtrs ()
							{
							}
						template <typename T2, typename T2_BASE_SharedPtr_TRAITS>
							inline	WeakSharedPtrEnvelope_ (const WeakSharedPtrEnvelope_<T2, T2_BASE_SharedPtr_TRAITS>& from)
								: BASE_SharedPtr_TRAITS::Envelope (from)
								, fCriticalSection ()
								, fWeakSharedPtrs ()
							{
							}
						inline	bool	Decrement ()
							{
								Execution::AutoCriticalSection critSec (fCriticalSection);
								return BASE_SharedPtr_TRAITS::Envelope::Decrement ();
							}
						inline	void	DoDeleteCounter ()
							{
								/*
								 * NOTE - this function is ALWAYS and ONLY called from inside Decrement() and so therefore always within the critical section.
								 */
								// Assert (fCriticalSection.IsLocked ());		-- would do if there was such an API - maybe it should be added?
								BASE_SharedPtr_TRAITS::Envelope::DoDeleteCounter ();
								for (list<WeakSharedPtrRep_<T,BASE_SharedPtr_TRAITS>*>::iterator i = fWeakSharedPtrs.begin (); i != fWeakSharedPtrs.end (); ++i) {
									i->fSharedPtrEnvelope = nullptr;
								}
							}

					};


				
				template	<typename T, typename BASE_SharedPtr_TRAITS>
					class	WeakSharedPtrRep_ {
						WeakSharedPtrEnvelope_<T,BASE_SharedPtr_TRAITS>*	fSharedPtrEnvelope;

#if 0
						SharedPtr<T,WeakSharedPtrCapableSharedPtrTraits<T,BASE_SharedPtr_TRAITS>>	Lock ()
							{
								SharedPtr<T,WeakSharedPtrCapableSharedPtrTraits<T,BASE_SharedPtr_TRAITS>> tmp;
// Not sure if its possible to fix with this architecture!!! How to get criticalsection lock on fSharedPtrEnvelope? Maybe must use a STATIC critical section? YES - I THINK THATS IT!

								return tmp;
							}
#endif

					};

			}


			/*
			 * To use the WeakSharedPtr<> - you need to construct a special SharedPtr<> with specially augmented traits.
			 * These are the specially augmented traits to use on the SharedPtr<> template. The BASE_SharedPtr_TRAITS
			 * are typically equal to SharedPtr_Default_Traits<T>.
			 */
			template	<typename T, typename BASE_SharedPtr_TRAITS = SharedPtr_Default_Traits<T>>
				struct	WeakSharedPtrCapableSharedPtrTraits : BASE_SharedPtr_TRAITS {
					typedef	Private::WeakSharedPtrEnvelope_<T, BASE_SharedPtr_TRAITS>				Envelope;
				};


			template	<typename T, typename BASE_SharedPtr_TRAITS = SharedPtr_Default_Traits<T>>
				class	WeakSharedPtr {
					public:
						typedef	SharedPtr<T,WeakSharedPtrCapableSharedPtrTraits<T,BASE_SharedPtr_TRAITS>>	SharedPtrType;
					public:
						WeakSharedPtr ();
						WeakSharedPtr (const WeakSharedPtr<T,BASE_SharedPtr_TRAITS>& from);
						explicit WeakSharedPtr (const SharedPtrType& from);
						nonvirtual	const WeakSharedPtr<T,BASE_SharedPtr_TRAITS>& operator= (const WeakSharedPtr<T,BASE_SharedPtr_TRAITS>& rhs);

					public:
						/*
						 * This is how you get a 'SharedPtr' from ....
						 */
						nonvirtual	SharedPtrType	Lock () const;

					private:
						SharedPtr<Private::WeakSharedPtrRep_<T,BASE_SharedPtr_TRAITS>>	fRep_;
				};




			/*
			 * The builtin SharedPtr<> doesn't support automatic WeakSharedPtr tracking. You must use a specialized
			 * 'WeakSharedPtrTraits' todo that. This class template takes the base.
			 *
			 * Note - WeakCapableSharedPtr is just a convenience. You can just use
			 *			typedef	SharedPtr<T,WeakSharedPtrCapableSharedPtrTraits<T,BASE_SharedPtr_TRAITS>>	WeakCapableSharedPtr
			 * 
			 *		or even
			 *
			 *			typedef	SharedPtr<T,WeakSharedPtrCapableSharedPtrTraits<T>>	WeakCapableSharedPtr
			 *
			 *	except that typedefs arent' allowed  as templates (maybe in C++11 with using?).
			 */
			template	<typename T, typename BASE_SharedPtr_TRAITS = SharedPtr_Default_Traits<T>>
				class	WeakCapableSharedPtr : public SharedPtr<T,WeakSharedPtrCapableSharedPtrTraits<T,BASE_SharedPtr_TRAITS>> {
					public:
						WeakCapableSharedPtr ()
							: SharedPtr<T,WeakSharedPtrCapableSharedPtrTraits<T,BASE_SharedPtr_TRAITS>> ()
							{
							}
						explicit WeakCapableSharedPtr (T* from)
							: SharedPtr<T,WeakSharedPtrCapableSharedPtrTraits<T,BASE_SharedPtr_TRAITS>> (from)
							{
							}
						explicit WeakCapableSharedPtr (T* from, typename BASE_SharedPtr_TRAITS::ReferenceCounterContainerType* useCounter)
							: SharedPtr<T,WeakSharedPtrCapableSharedPtrTraits<T,BASE_SharedPtr_TRAITS>> (from, useCounter)
							{
							}
						WeakCapableSharedPtr (const WeakCapableSharedPtr<T,BASE_SharedPtr_TRAITS>& from)
							: SharedPtr<T,WeakSharedPtrCapableSharedPtrTraits<T,BASE_SharedPtr_TRAITS>> (from)
							{
							}
						template <typename T2, typename T2_TRAITS>
							WeakCapableSharedPtr (const WeakCapableSharedPtr<T2, T2_TRAITS>& from)
								: SharedPtr<T,WeakSharedPtrCapableSharedPtrTraits<T,BASE_SharedPtr_TRAITS>> (from)
								{
								}
				};


		}
	}
}
#endif	/*_Stroika_Foundation_Memory_WeakSharedPtr_h_*/







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"WeakSharedPtr.inl"
