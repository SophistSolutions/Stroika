/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_SharedByValue_inl_
#define _Stroika_Foundation_Memory_SharedByValue_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Execution/AtomicOperations.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {




#if     !qCompilerAndStdLib_Supports_lambda_default_argument
            template    <typename   T>
            T*  SharedByValue_CopyByFunction<T>::DefaultElementCopier_ (const T& t)
            {
                return new T (t);
            }
#endif
            template    <typename   T>
            inline  SharedByValue_CopyByFunction<T>::SharedByValue_CopyByFunction (T * (*copier) (const T&))
                : fCopier (copier)
            {
            }
            template    <typename   T>
            inline  T*  SharedByValue_CopyByFunction<T>::Copy (const T& t) const
            {
                return (*fCopier) (t);
            }



            template    <typename   T>
            inline  T*  SharedByValue_CopyByDefault<T>::Copy (const T& t)
            {
                return new T (t);
            }




            template    <typename   T, typename COPIER, typename SHARED_IMLP>
            inline  SharedByValue<T, COPIER, SHARED_IMLP>::SharedByValue ()
                : fCopier_ (COPIER ())
                , fSharedImpl_ ()
            {
            }
            template    <typename   T, typename COPIER, typename SHARED_IMLP>
            inline  SharedByValue<T, COPIER, SHARED_IMLP>::SharedByValue (const SharedByValue<T, COPIER, SHARED_IMLP>& from)
                : fCopier_ (from.fCopier_)
                , fSharedImpl_ (from.fSharedImpl_)
            {
            }
            template    <typename   T, typename COPIER, typename SHARED_IMLP>
            inline  SharedByValue<T, COPIER, SHARED_IMLP>::SharedByValue (const SHARED_IMLP& from, const COPIER& copier)
                : fCopier_ (copier)
                , fSharedImpl_ (from.fSharedImpl_)
            {
            }
            template    <typename   T, typename COPIER, typename SHARED_IMLP>
            inline  SharedByValue<T, COPIER, SHARED_IMLP>::SharedByValue (T* from, const COPIER& copier)
                : fCopier_ (copier)
                , fSharedImpl_ (from)
            {
            }
            template    <typename   T, typename COPIER, typename SHARED_IMLP>
            inline  SharedByValue<T, COPIER, SHARED_IMLP>& SharedByValue<T, COPIER, SHARED_IMLP>::operator= (const SharedByValue<T, COPIER, SHARED_IMLP>& src)
            {
                fCopier_ = src.fCopier_;
                fSharedImpl_ = src.fSharedImpl_;
                return *this;
            }
            template    <typename   T, typename COPIER, typename SHARED_IMLP>
            inline  const T*    SharedByValue<T, COPIER, SHARED_IMLP>::get () const
            {
                return (fSharedImpl_.get ());
            }
            template    <typename   T, typename COPIER, typename SHARED_IMLP>
            T* SharedByValue<T, COPIER, SHARED_IMLP>::get ()
            {
                T*  ptr =   fSharedImpl_.get ();
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
                ptr = fSharedImpl_.get ();
                EnsureNotNull (ptr);
                return (ptr);
            }
            template    <typename   T, typename COPIER, typename SHARED_IMLP>
            inline  const T*    SharedByValue<T, COPIER, SHARED_IMLP>::operator-> () const
            {
                return (fSharedImpl_.get ());
            }
            template    <typename   T, typename COPIER, typename SHARED_IMLP>
            inline  T* SharedByValue<T, COPIER, SHARED_IMLP>::operator-> ()
            {
                return get ();
            }
            template    <typename   T, typename COPIER, typename SHARED_IMLP>
            inline  const T&    SharedByValue<T, COPIER, SHARED_IMLP>::operator* () const
            {
                const T*  ptr =   get ();
                EnsureNotNull (ptr);
                return (*ptr);
            }
            template    <typename   T, typename COPIER, typename SHARED_IMLP>
            T& SharedByValue<T, COPIER, SHARED_IMLP>::operator* ()
            {
                T*  ptr =   get ();
                /*
                 * For non-const dereferencing, we must clone ourselves (if there are
                 * extra referneces).
                 */
                Assure1Reference ();
                EnsureNotNull (ptr);
                return (*ptr);
            }
            template    <typename   T, typename COPIER, typename SHARED_IMLP>
            bool SharedByValue<T, COPIER, SHARED_IMLP>::operator== (const SharedByValue<T, COPIER, SHARED_IMLP>& rhs) const
            {
                return fSharedImpl_ == rhs.fSharedImpl_;
            }
            template    <typename   T, typename COPIER, typename SHARED_IMLP>
            bool SharedByValue<T, COPIER, SHARED_IMLP>::operator!= (const SharedByValue<T, COPIER, SHARED_IMLP>& rhs) const
            {
                return fSharedImpl_ != rhs.fSharedImpl_;
            }
            template    <typename   T, typename COPIER, typename SHARED_IMLP>
            bool    SharedByValue<T, COPIER, SHARED_IMLP>::unique () const
            {
                return fSharedImpl_.unique ();
            }
            template    <typename   T, typename COPIER, typename SHARED_IMLP>
            inline  void    SharedByValue<T, COPIER, SHARED_IMLP>::Assure1Reference ()
            {
                if (!fSharedImpl_.unique ()) {
                    BreakReferences_ ();
                }
            }
            template    <typename   T, typename COPIER, typename SHARED_IMLP>
            void    SharedByValue<T, COPIER, SHARED_IMLP>::BreakReferences_ ()
            {
                T*  ptr =   fSharedImpl_.get ();
                RequireNotNull (ptr);
                /*
                 *      For a valid pointer that is reference counted and multiply shared,
                 *  make a copy of that pointer via our fCloner function, and assign
                 *  that cloned reference to this.
                 *
                 *      Note that by doing so, we remove any references to the current
                 *  item, and end up with our having the sole reference to the new copy of fPtr.
                 *
                 *      Since we will be cloning the given pointer, we assume(assert) that
                 *  it is non-nullptr.
                 */
                //Require (!SHARED_IMLP::unique ());    This is not NECESSARILY so. Another thread could have just released this pointer, in which case
                // the creation of a new object was pointless, but harmless, as the assignemnt should decrement to zero the old
                // value and it should go away.
                *this = SharedByValue<T, COPIER, SHARED_IMLP> (fCopier_.Copy (*ptr), fCopier_);
                Ensure (fSharedImpl_.unique ());
            }

        }
    }
}
#endif  /*_Stroika_Foundation_Memory_SharedByValue_inl_*/
