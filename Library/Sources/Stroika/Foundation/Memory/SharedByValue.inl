/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Memory_SharedByValue_inl_
#define	_Stroika_Foundation_Memory_SharedByValue_inl_	1


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




			#if		!qCompilerAndStdLib_Supports_lambda_default_argument
			template	<typename	T>
				T*	SharedByValue_CopyByFunction::DefaultElementCopier_ (const T& t)
					{
						return new T (t);
					}
			#endif
			template	<typename	T>
				inline	SharedByValue_CopyByFunction<T>::SharedByValue_CopyByFunction (T* (*copier) (const T&))
					: fCopier_ (copier)
					{
					}
			template	<typename	T>
				inline	T*	SharedByValue_CopyByFunction<T>::Copy (const T& t) const
					{
						return (*fCopier_) (t);
					}



			template    <typename	T, typename COPIER, typename SHARED_IMLP>
				inline	SharedByValue<T,COPIER,SHARED_IMLP>::SharedByValue ()
					: SHARED_IMLP ()
					, fCopier_ (COPIER ())
					{
					}
			template    <typename	T, typename COPIER, typename SHARED_IMLP>
				inline	SharedByValue<T,COPIER,SHARED_IMLP>::SharedByValue (const SharedByValue<T,COPIER,SHARED_IMLP>& from)
					: SHARED_IMLP (from)
					, fCopier_ (from.fCopier_)
					{
					}
			template    <typename	T, typename COPIER, typename SHARED_IMLP>
				inline	SharedByValue<T,COPIER,SHARED_IMLP>::SharedByValue (const SHARED_IMLP& from, const COPIER& copier)
					: SHARED_IMLP (from)
					, fCopier_ (copier)
					{
					}
			template    <typename	T, typename COPIER, typename SHARED_IMLP>
				inline	SharedByValue<T,COPIER,SHARED_IMLP>::SharedByValue (T* from, const COPIER& copier)
					: SHARED_IMLP (from)
					, fCopier_ (copier)
					{
					}
			template    <typename	T, typename COPIER, typename SHARED_IMLP>
				inline	SharedByValue<T,COPIER,SHARED_IMLP>& SharedByValue<T,COPIER,SHARED_IMLP>::operator= (const SharedByValue<T,COPIER,SHARED_IMLP>& src)
					{
						SHARED_IMLP::operator= (src);
						fCopier_ = src.fCopier_;
						return *this;
					}
			template    <typename	T, typename COPIER, typename SHARED_IMLP>
				inline	const T*	SharedByValue<T,COPIER,SHARED_IMLP>::GetPointer () const
					{
						return (SHARED_IMLP::get ());
					}
			template    <typename	T, typename COPIER, typename SHARED_IMLP>
				T* SharedByValue<T,COPIER,SHARED_IMLP>::GetPointer ()
					{
						T*	ptr	=	SHARED_IMLP::get ();
						/*
						 * For non-const pointing, we must clone ourselves (if there are
						 * extra referneces). If we are a nullptr pointer, nobody could actually
						 * rereference it anyhow, so don't bother with the Assure1Reference()
						 * in that case.
						 */
						if (ptr == nullptr) {
							return nullptr;
						}
						Assure1Reference ();
						ptr = SHARED_IMLP::get ();
						EnsureNotNull (ptr);
						return (ptr); 
					}
			template    <typename	T, typename COPIER, typename SHARED_IMLP>
				inline	const T*	SharedByValue<T,COPIER,SHARED_IMLP>::operator-> () const
					{
						return (SHARED_IMLP::get ());
					}
			template    <typename	T, typename COPIER, typename SHARED_IMLP>
				inline	T* SharedByValue<T,COPIER,SHARED_IMLP>::operator-> ()
					{
						return GetPointer ();
					}
			template    <typename	T, typename COPIER, typename SHARED_IMLP>
				inline	const T&	SharedByValue<T,COPIER,SHARED_IMLP>::operator* () const
					{
						T*	ptr	=	GetPointer ();
						EnsureNotNull (ptr);
						return (*ptr);
					}
			template    <typename	T, typename COPIER, typename SHARED_IMLP>
				T& SharedByValue<T,COPIER,SHARED_IMLP>::operator* ()
					{
						T*	ptr	=	GetPointer ();
						/*
						 * For non-const dereferencing, we must clone ourselves (if there are
						 * extra referneces).
						 */
						Assure1Reference ();
						EnsureNotNull (ptr);
						return (*ptr); 
					}
			template    <typename	T, typename COPIER, typename SHARED_IMLP>
				inline	void	SharedByValue<T,COPIER,SHARED_IMLP>::Assure1Reference ()
					{
						if (!SHARED_IMLP::unique ()) {
							BreakReferences_ ();
						}
					}
			template    <typename	T, typename COPIER, typename SHARED_IMLP>
				void	SharedByValue<T,COPIER,SHARED_IMLP>::BreakReferences_ ()
					{
						T*	ptr	=	SHARED_IMLP::get ();
						RequireNotNull (ptr);
						/*
						 *		For a valid pointer that is reference counted and multiply shared,
						 *	make a copy of that pointer via our fCloner function, and assign
						 *	that cloned reference to this.
						 *
						 *		Note that by doing so, we remove any references to the current
						 *	item, and end up with our having the sole reference to the new copy of fPtr.
						 *
						 *		Since we will be cloning the given pointer, we assume(assert) that
						 *	it is non-nullptr.
						 */
						//Require (!SHARED_IMLP::unique ());	This is not NECESSARILY so. Another thread could have just released this pointer, in which case
						// the creation of a new object was pointless, but harmless, as the assignemnt should decrement to zero the old
						// value and it shoudl go away.
						*this = SharedByValue<T,COPIER,SHARED_IMLP> (fCopier_.Copy (*ptr), fCopier_);
						Ensure (SHARED_IMLP::unique ());
					}

		}
	}
}
#endif	/*_Stroika_Foundation_Memory_SharedByValue_inl_*/
