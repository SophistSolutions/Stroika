/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Memory_WeakSharedPtr_h_
#define	_Stroika_Foundation_Memory_WeakSharedPtr_h_	1

#include	"../StroikaPreComp.h"

#include	<list>
#include	<memory>

#include	"../Configuration/Common.h"

#include	"SharedPtr.h"



//////EXTREMELY INCOMPLETE - JUST AT PROTOTYPE LEVEL!!!!



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



			namespace	Private {

				template	<typename T, typename BASE_SharedPtr_TRAITS>
					class	WeakSharedPtrRep;


				// This is an implementation detail of the WeakPtr's 'SharedPtr' traits class. It is the extended 'Envelope' class used by the SharedPtr
				// including info needed for the WeakPtr tracking
				template	<typename T, typename BASE_SharedPtr_TRAITS>
					struct	WeakSharedPtrEnvelope_ : BASE_SharedPtr_TRAITS::Envelope {
						list<WeakSharedPtrRep<T,BASE_SharedPtr_TRAITS>*>	fWeakPtrs;

						WeakSharedPtrEnvelope_ (T* ptr, typename BASE_SharedPtr_TRAITS::ReferenceCounterContainerType* countHolder)
							: BASE_SharedPtr_TRAITS::Envelope (ptr, countHolder)
							{
							}
						template <typename T2, typename T2_BASE_SharedPtr_TRAITS>
							inline	WeakSharedPtrEnvelope_ (const WeakSharedPtrEnvelope_<T2, T2_BASE_SharedPtr_TRAITS>& from)
								: BASE_SharedPtr_TRAITS::Envelope (from)
							{
							}
						inline	bool	Decrement ()
							{
								// DO LOCK HERE AND THEN INERITED
								// MUST ADD AUTOCRITICALSECTION - where critsection owned by US.
								return BASE_SharedPtr_TRAITS::Envelope::Decrement ();

	#if 0
								/// break Decrement () stuff into 2 parts - and hook/override so if deelting - walk fWeakPtrs list and clear them...
								Require (CurrentRefCount () > 0);
								if (Execution::AtomicDecrement (&fCountHolder_->fCount) == 0) {
									delete fCountHolder_;
									fCountHolder_ = nullptr;
									return true;
								}
	#endif
								return false;
							}
						inline	void	DoDeleteCounter ()
							{
								BASE_SharedPtr_TRAITS::Envelope::DoDeleteCounter ();
								// AND DO MORE HERE - notify...
							}

					};


				
				template	<typename T, typename BASE_SharedPtr_TRAITS>
					class	WeakSharedPtrRep {
						T*	fBarePointer;
					};

			}


			/*
			 * To use the WeakSharedPtr<> - you need to construct a special SharedPtr<> with specially augmented traits. These
			 * are the specially augmented traits to use on the SharedPtr<> template. The BASE_SharedPtr_TRAITS are typically equal to SharedPtr_Default_Traits<T>.
			 */
			template	<typename T, typename BASE_SharedPtr_TRAITS = SharedPtr_Default_Traits<T>>
				struct	WeakPtrCapableSharedPtrTraits : BASE_SharedPtr_TRAITS {
				};


			template	<typename T, typename BASE_SharedPtr_TRAITS = SharedPtr_Default_Traits<T>>
				class	WeakSharedPtr {
					public:
						typedef	SharedPtr<T,WeakPtrCapableSharedPtrTraits<T,BASE_SharedPtr_TRAITS>>	SharedPtrType;
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
						SharedPtr<Private::WeakSharedPtrRep<T,BASE_SharedPtr_TRAITS>>	fRep_;
				};




			/*
			 * The builtin SharedPtr<> doesn't support automatic WeakPtr tracking. You must use a specialized
			 * 'WeakPtrTraits' todo that. This class template takes the base.
			 *
			 * Note - WeakCapableSharedPtr is just a convenience. You can just use
			 *			typedef	SharedPtr<T,WeakPtrCapableSharedPtrTraits<T,BASE_SharedPtr_TRAITS>>	WeakCapableSharedPtr
			 * 
			 *		or even
			 *
			 *			typedef	SharedPtr<T,WeakPtrCapableSharedPtrTraits<T>>	WeakCapableSharedPtr
			 *
			 *	except that typedefs arent' allowed  as templates (maybe in C++11 with using?).
			 */
			template	<typename T, typename BASE_SharedPtr_TRAITS = SharedPtr_Default_Traits<T>>
				class	WeakCapableSharedPtr : public SharedPtr<T,WeakPtrCapableSharedPtrTraits<T,BASE_SharedPtr_TRAITS>> {
					public:
						WeakCapableSharedPtr ()
							: SharedPtr<T,WeakPtrCapableSharedPtrTraits<T,BASE_SharedPtr_TRAITS>> ()
							{
							}
						explicit WeakCapableSharedPtr (T* from)
							: SharedPtr<T,WeakPtrCapableSharedPtrTraits<T,BASE_SharedPtr_TRAITS>> (from)
							{
							}
						explicit WeakCapableSharedPtr (T* from, typename BASE_SharedPtr_TRAITS::ReferenceCounterContainerType* useCounter)
							: SharedPtr<T,WeakPtrCapableSharedPtrTraits<T,BASE_SharedPtr_TRAITS>> (from, useCounter)
							{
							}
						WeakCapableSharedPtr (const WeakCapableSharedPtr<T,BASE_SharedPtr_TRAITS>& from)
							: SharedPtr<T,WeakPtrCapableSharedPtrTraits<T,BASE_SharedPtr_TRAITS>> (from)
							{
							}
						template <typename T2, typename T2_TRAITS>
							WeakCapableSharedPtr (const WeakCapableSharedPtr<T2, T2_TRAITS>& from)
								: SharedPtr<T,WeakPtrCapableSharedPtrTraits<T,BASE_SharedPtr_TRAITS>> (from)
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
