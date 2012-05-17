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


			// class	Private::WeakSharedPtrEnvelope_<T,BASE_SharedPtr_TRAITS>
			template	<typename T, typename BASE_SharedPtr_TRAITS>
				Private::WeakSharedPtrEnvelope_<T,BASE_SharedPtr_TRAITS>::WeakSharedPtrEnvelope_ (T* ptr, typename BASE_SharedPtr_TRAITS::ReferenceCounterContainerType* countHolder)
					: BASE_SharedPtr_TRAITS::Envelope (ptr, countHolder)
					, fWeakSharedPtrs ()
					{
					}
			template	<typename T, typename BASE_SharedPtr_TRAITS>
				template <typename T2, typename T2_BASE_SharedPtr_TRAITS>
					inline	Private::WeakSharedPtrEnvelope_<T,BASE_SharedPtr_TRAITS>::WeakSharedPtrEnvelope_ (const WeakSharedPtrEnvelope_<T2, T2_BASE_SharedPtr_TRAITS>& from)
						: BASE_SharedPtr_TRAITS::Envelope (from)
						, fWeakSharedPtrs ()
					{
					}
			template	<typename T, typename BASE_SharedPtr_TRAITS>
				inline	bool	Private::WeakSharedPtrEnvelope_<T,BASE_SharedPtr_TRAITS>::Decrement ()
					{
						Execution::AutoCriticalSection critSec (sCriticalSection);	// critical section is IN CASE we need to delete counter
																					// - must make sure to grab crit section before so WeakPtr class not in middle of a
																					// lock()
						return BASE_SharedPtr_TRAITS::Envelope::Decrement ();
					}
			template	<typename T, typename BASE_SharedPtr_TRAITS>
				void	Private::WeakSharedPtrEnvelope_<T,BASE_SharedPtr_TRAITS>::DoDeleteCounter ()
					{
						/*
						 * NOTE - this function is ALWAYS and ONLY called from inside Decrement() and so therefore always within the critical section.
						 */
						// Assert (sCriticalSection.IsLocked ());		-- would do if there was such an API - maybe it should be added?
						BASE_SharedPtr_TRAITS::Envelope::DoDeleteCounter ();
						for (typename list<WeakSharedPtrRep_<T,BASE_SharedPtr_TRAITS>*>::iterator i = fWeakSharedPtrs.begin (); i != fWeakSharedPtrs.end (); ++i) {
							i->fSharedPtrEnvelope = nullptr;
						}
					}
			template <typename T, typename BASE_SharedPtr_TRAITS> 
				Execution::CriticalSection Private::WeakSharedPtrEnvelope_<T,BASE_SharedPtr_TRAITS>::sCriticalSection;






			// class	Private::WeakSharedPtrRep_<T,BASE_SharedPtr_TRAITS>
			template	<typename T, typename BASE_SharedPtr_TRAITS>
				inline	Private::WeakSharedPtrRep_<T,BASE_SharedPtr_TRAITS>::WeakSharedPtrRep_ (const SharedPtrType& shared)
					: fSharedPtrEnvelope (SharedPtrType (shared).PeekAtEnvelope ())
					{
//NOT RIGHT - CANNOT STORE fSharedPtrEnvelope!!!! - SERIOUS BUG!!!! - DESIGN FLAW!!!!
						Execution::AutoCriticalSection critSec (WeakSharedPtrEnvelope_<T,BASE_SharedPtr_TRAITS>::sCriticalSection);
						fSharedPtrEnvelope->fWeakSharedPtrs.push_back (this);
					}
			template	<typename T, typename BASE_SharedPtr_TRAITS>
				inline	Private::WeakSharedPtrRep_<T,BASE_SharedPtr_TRAITS>::~WeakSharedPtrRep_ ()
					{
						Execution::AutoCriticalSection critSec (WeakSharedPtrEnvelope_<T,BASE_SharedPtr_TRAITS>::sCriticalSection);
						if (fSharedPtrEnvelope != nullptr) {
							typename list<WeakSharedPtrRep_<T,BASE_SharedPtr_TRAITS>*>::iterator i = std::find (fSharedPtrEnvelope->fWeakSharedPtrs.begin (), fSharedPtrEnvelope->fWeakSharedPtrs.end (), this);
							Assert (i != fSharedPtrEnvelope->fWeakSharedPtrs.end ());		// cannot be missing without bug or memory corruption
							fSharedPtrEnvelope->fWeakSharedPtrs.erase (i);
						}
					}
			template	<typename T, typename BASE_SharedPtr_TRAITS>
				SharedPtr<T,WeakSharedPtrCapableSharedPtrTraits<T,BASE_SharedPtr_TRAITS>>	Private::WeakSharedPtrRep_<T,BASE_SharedPtr_TRAITS>::Lock ()
					{
						Execution::AutoCriticalSection critSec (WeakSharedPtrEnvelope_<T,BASE_SharedPtr_TRAITS>::sCriticalSection);
						if (fSharedPtrEnvelope == nullptr) {
							return SharedPtr<T,WeakSharedPtrCapableSharedPtrTraits<T,BASE_SharedPtr_TRAITS>> ();
						}
						return SharedPtr<T,WeakSharedPtrCapableSharedPtrTraits<T,BASE_SharedPtr_TRAITS>> (*fSharedPtrEnvelope);
					}






			// class	WeakSharedPtr<T,BASE_SharedPtr_TRAITS>
			template	<typename T, typename BASE_SharedPtr_TRAITS>
				inline	WeakSharedPtr<T,BASE_SharedPtr_TRAITS>::WeakSharedPtr ()
					: fRep_ ()
					{
					}
			template	<typename T, typename BASE_SharedPtr_TRAITS>
				WeakSharedPtr<T,BASE_SharedPtr_TRAITS>::WeakSharedPtr (const WeakSharedPtr<T,BASE_SharedPtr_TRAITS>& from)
					: fRep_ (from)
					{
					}
			template	<typename T, typename BASE_SharedPtr_TRAITS>
				WeakSharedPtr<T,BASE_SharedPtr_TRAITS>::WeakSharedPtr (const SharedPtrType& from)
					: fRep_ (DEBUG_NEW Private::WeakSharedPtrRep_<T,BASE_SharedPtr_TRAITS> (from))
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
						if (fRep_.IsNull ()) {
							return WeakSharedPtr<T,BASE_SharedPtr_TRAITS>::SharedPtrType ();
						}
						return fRep_->Lock ();
					}







			// class	WeakCapableSharedPtr<T,BASE_SharedPtr_TRAITS>
			template	<typename T, typename BASE_SharedPtr_TRAITS>
				inline	WeakCapableSharedPtr<T,BASE_SharedPtr_TRAITS>::WeakCapableSharedPtr ()
					: SharedPtr<T,WeakSharedPtrCapableSharedPtrTraits<T,BASE_SharedPtr_TRAITS>> ()
					{
					}
			template	<typename T, typename BASE_SharedPtr_TRAITS>
				inline	WeakCapableSharedPtr<T,BASE_SharedPtr_TRAITS>::WeakCapableSharedPtr (T* from)
					: SharedPtr<T,WeakSharedPtrCapableSharedPtrTraits<T,BASE_SharedPtr_TRAITS>> (from)
					{
					}
			template	<typename T, typename BASE_SharedPtr_TRAITS>
				inline	WeakCapableSharedPtr<T,BASE_SharedPtr_TRAITS>::WeakCapableSharedPtr (T* from, typename BASE_SharedPtr_TRAITS::ReferenceCounterContainerType* useCounter)
					: SharedPtr<T,WeakSharedPtrCapableSharedPtrTraits<T,BASE_SharedPtr_TRAITS>> (from, useCounter)
					{
					}
			template	<typename T, typename BASE_SharedPtr_TRAITS>
				inline	WeakCapableSharedPtr<T,BASE_SharedPtr_TRAITS>::WeakCapableSharedPtr (const WeakCapableSharedPtr<T,BASE_SharedPtr_TRAITS>& from)
					: SharedPtr<T,WeakSharedPtrCapableSharedPtrTraits<T,BASE_SharedPtr_TRAITS>> (from)
					{
					}
			template	<typename T, typename BASE_SharedPtr_TRAITS>
				template <typename T2, typename T2_TRAITS>
					inline	WeakCapableSharedPtr<T,BASE_SharedPtr_TRAITS>::WeakCapableSharedPtr (const WeakCapableSharedPtr<T2, T2_TRAITS>& from)
						: SharedPtr<T,WeakSharedPtrCapableSharedPtrTraits<T,BASE_SharedPtr_TRAITS>> (from)
						{
						}



		}
	}
}
#endif	/*_Stroika_Foundation_Memory_WeakSharedPtr_inl_*/
