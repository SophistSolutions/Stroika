/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_SharedByValue_inl_
#define _Stroika_Foundation_Memory_SharedByValue_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "../Debug/Trace.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            /*
            ********************************************************************************
            ************** SharedByValue_CopyByFunction<T,SHARED_IMLP> *********************
            ********************************************************************************
            */
#if     qCompilerAndStdLib_lambda_default_argument_with_template_param_as_function_cast_Buggy
            template    <typename   T, typename SHARED_IMLP>
            SHARED_IMLP  SharedByValue_CopyByFunction<T, SHARED_IMLP>::DefaultElementCopier_ (const T& t)
            {
                return SHARED_IMLP (new T (t));
            }
#endif
            template    <typename   T, typename SHARED_IMLP>
            inline  SharedByValue_CopyByFunction<T, SHARED_IMLP>::SharedByValue_CopyByFunction (SHARED_IMLP (*copier) (const T&)) noexcept
:
            fCopier (copier)
            {
                RequireNotNull (copier);
            }
            template    <typename   T, typename SHARED_IMLP>
            inline  SHARED_IMLP  SharedByValue_CopyByFunction<T, SHARED_IMLP>::Copy (const T& t) const
            {
                AssertNotNull (fCopier);
                return (*fCopier) (t);
            }


            /*
            ********************************************************************************
            *************** SharedByValue_CopyByDefault<T,SHARED_IMLP> *********************
            ********************************************************************************
            */
            template    <typename   T, typename SHARED_IMLP>
            inline  SHARED_IMLP  SharedByValue_CopyByDefault<T, SHARED_IMLP>::Copy (const T& t)
            {
                return SHARED_IMLP (new T (t));
            }


            /*
            ********************************************************************************
            ****** SharedByValue_CopySharedPtrExternallySynchronized<T,SHARED_IMLP> ********
            ********************************************************************************
            */
            template    <typename   T, typename SHARED_IMLP>
            inline  SHARED_IMLP     SharedByValue_CopySharedPtrExternallySynchronized<T, SHARED_IMLP>::Load (const SHARED_IMLP& copyFrom)
            {
                return copyFrom;
            }
            template    <typename   T, typename SHARED_IMLP>
            inline  void            SharedByValue_CopySharedPtrExternallySynchronized<T, SHARED_IMLP>::Store (SHARED_IMLP* storeTo, const SHARED_IMLP& o)
            {
                RequireNotNull (storeTo);
                *storeTo = o;
            }


            /*
            ********************************************************************************
            ********* SharedByValue_CopySharedPtrAtomicSynchronized<T,SHARED_IMLP> *********
            ********************************************************************************
            */
            template    <typename   T, typename SHARED_IMLP>
            inline  SHARED_IMLP     SharedByValue_CopySharedPtrAtomicSynchronized<T, SHARED_IMLP>::Load (const SHARED_IMLP& copyFrom)
            {
                return atomic_load (&copyFrom);
            }
            template    <typename   T, typename SHARED_IMLP>
            inline  void            SharedByValue_CopySharedPtrAtomicSynchronized<T, SHARED_IMLP>::Store (SHARED_IMLP* storeTo, const SHARED_IMLP& o)
            {
                RequireNotNull (storeTo);
                atomic_store (storeTo, o);
            }


            /*
            ********************************************************************************
            ****************************** SharedByValue<TRAITS> ***************************
            ********************************************************************************
            */
            template    <typename TRAITS>
            inline  SharedByValue<TRAITS>::SharedByValue () noexcept
:
            fCopier_ (element_copier_type ())
            , fSharedImpl_ ()
            {
            }
            template    <typename TRAITS>
            inline  SharedByValue<TRAITS>::SharedByValue (nullptr_t n) noexcept
:
            fCopier_ (element_copier_type ())
            , fSharedImpl_ ()
            {
            }
            template    <typename TRAITS>
            inline  SharedByValue<TRAITS>::SharedByValue (const SharedByValue<TRAITS>& from) noexcept
:
            fCopier_ (from.fCopier_)
            , fSharedImpl_ (shared_impl_copier_type::Load (from.fSharedImpl_))
            {
            }
            template    <typename TRAITS>
            inline  SharedByValue<TRAITS>::SharedByValue (SharedByValue<TRAITS>&& from) noexcept
:
            fCopier_ (from.fCopier_)
            , fSharedImpl_ (std::move (from.fSharedImpl_))
            {
            }
            template    <typename TRAITS>
            inline  SharedByValue<TRAITS>::SharedByValue (const shared_ptr_type& from, const element_copier_type& copier) noexcept
:
            fCopier_ (copier)
            , fSharedImpl_ (shared_impl_copier_type::Load (from))
            {
            }
            template    <typename TRAITS>
            inline  SharedByValue<TRAITS>::SharedByValue (shared_ptr_type&& from, const element_copier_type&& copier) noexcept
:
            fCopier_ (move (copier))
            , fSharedImpl_ (move (from))
            {
            }
            template    <typename TRAITS>
            inline  SharedByValue<TRAITS>::SharedByValue (element_type* from, const element_copier_type& copier)
                : fCopier_ (copier)
                , fSharedImpl_ (from)
            {
            }
            template    <typename TRAITS>
            inline  SharedByValue<TRAITS>& SharedByValue<TRAITS>::operator= (const SharedByValue<TRAITS>& rhs)
            {
                // If the pointers are the same, there is no need to copy, as the reference counts must also be the same,
                // and we can avoid the (common) and costly memory barrier
                if (fSharedImpl_.get () != rhs.fSharedImpl_.get ()) {
                    fCopier_ = rhs.fCopier_;
                    shared_impl_copier_type::Store (&fSharedImpl_, shared_impl_copier_type::Load (rhs.fSharedImpl_));
                }
                return *this;
            }
            template    <typename TRAITS>
            inline  SharedByValue<TRAITS>& SharedByValue<TRAITS>::operator= (SharedByValue<TRAITS> && rhs)
            {
                // If the pointers are the same, there is no need to copy, as the reference counts must also be the same,
                // and we can avoid the (common) and costly memory barrier
                if (fSharedImpl_.get () != rhs.fSharedImpl_.get ()) {
                    fCopier_ = rhs.fCopier_;
                    // ASSUME if doing a move() then this doesn't need to be a multithread safe copy (from the source), since
                    // if its a temporary, you cannot have mulitple peopel referring to me
                    shared_impl_copier_type::Store (&fSharedImpl_, rhs.fSharedImpl_);
                }
                return *this;
            }
            template    <typename TRAITS>
            inline  SharedByValue<TRAITS>& SharedByValue<TRAITS>::operator= (const shared_ptr_type& from)
            {
                // If the pointers are the same, there is no need to copy, as the reference counts must also be the same,
                // and we can avoid the (common) and costly memory barrier
                if (fSharedImpl_.get () != from.get ()) {
                    shared_impl_copier_type::Store (&fSharedImpl_, shared_impl_copier_type::Load (from));
                }
                return *this;
            }
            template    <typename TRAITS>
            inline  SharedByValue<TRAITS>& SharedByValue<TRAITS>::operator= (shared_ptr_type && from)
            {
                // If the pointers are the same, there is no need to copy, as the reference counts must also be the same,
                // and we can avoid the (common) and costly memory barrier
                if (fSharedImpl_.get () != from.get ()) {
                    // ASSUME if doing a move() then this doesn't need to be a multithread safe copy (from the source), since
                    // if its a temporary, you cannot have mulitple peopel referring to me
                    shared_impl_copier_type::Store (&fSharedImpl_, from);
                }
                return *this;
            }
            template    <typename TRAITS>
            inline  const typename SharedByValue<TRAITS>::element_type*    SharedByValue<TRAITS>::get () const
            {
                return fSharedImpl_.get ();
            }
            template    <typename TRAITS>
            template    <typename... COPY_ARGS>
            inline  typename SharedByValue<TRAITS>::element_type* SharedByValue<TRAITS>::get (COPY_ARGS&& ... copyArgs)
            {
                element_type*  ptr =   fSharedImpl_.get ();
                /*
                 * For non-const pointing, we must clone ourselves (if there are
                 * extra referneces). If we are a nullptr pointer, nobody could actually
                 * rereference it anyhow, so don't bother with the Assure1Reference()
                 * in that case.
                 */
                if (ptr != nullptr) {
                    Assure1Reference (forward<COPY_ARGS> (copyArgs)...);
                    ptr = fSharedImpl_.get ();
                    EnsureNotNull (ptr);
                }
                return ptr;
            }
            template    <typename TRAITS>
            inline  const typename SharedByValue<TRAITS>::element_type*    SharedByValue<TRAITS>::cget () const
            {
                return get ();
            }
            template    <typename TRAITS>
            inline  const typename SharedByValue<TRAITS>::element_type*    SharedByValue<TRAITS>::operator-> () const
            {
                return fSharedImpl_.get ();
            }
            template    <typename TRAITS>
            inline  typename SharedByValue<TRAITS>::element_type* SharedByValue<TRAITS>::operator-> ()
            {
                return get ();
            }
            template    <typename TRAITS>
            inline  const typename SharedByValue<TRAITS>::element_type&    SharedByValue<TRAITS>::operator* () const
            {
                const element_type*  ptr =   get ();
                EnsureNotNull (ptr);
                return *ptr;
            }
            template    <typename TRAITS>
            inline  typename SharedByValue<TRAITS>::element_type&   SharedByValue<TRAITS>::operator* ()
            {
                /*
                 * For non-const dereferencing, we must clone ourselves (if there are
                 * extra referneces).
                 */
                Assure1Reference ();
                element_type*  ptr =   get ();
                EnsureNotNull (ptr);
                return *ptr;
            }
            template    <typename TRAITS>
            inline  bool    SharedByValue<TRAITS>::operator== (const SharedByValue<TRAITS>& rhs) const
            {
                return fSharedImpl_ == rhs.fSharedImpl_;
            }
            template    <typename TRAITS>
            inline  bool    SharedByValue<TRAITS>::operator!= (const SharedByValue<TRAITS>& rhs) const
            {
                return fSharedImpl_ != rhs.fSharedImpl_;
            }
            template    <typename TRAITS>
            inline  typename SharedByValue<TRAITS>::element_copier_type SharedByValue<TRAITS>::GetCopier () const
            {
                return fCopier_;
            }
            template    <typename TRAITS>
            inline  SharedByValue_State    SharedByValue<TRAITS>::GetSharingState () const
            {
                switch (fSharedImpl_.use_count ()) {
                    case 0:
                        Assert (fSharedImpl_.get () == nullptr);
                        return SharedByValue_State::eNull;
                    case 1:
                        Assert (fSharedImpl_.get () != nullptr);
                        Assert (fSharedImpl_.unique ());
                        return SharedByValue_State::eSolo;
                    default:
                        Assert (fSharedImpl_.get () != nullptr);
                        Assert (not fSharedImpl_.unique ());
                        return SharedByValue_State::eShared;
                }
            }
            template    <typename TRAITS>
            inline  bool    SharedByValue<TRAITS>::unique () const
            {
                return fSharedImpl_.unique ();
            }
            template    <typename TRAITS>
            inline  unsigned int    SharedByValue<TRAITS>::use_count () const
            {
                return fSharedImpl_.use_count ();
            }
            template    <typename TRAITS>
            template    <typename... COPY_ARGS>
            inline  void    SharedByValue<TRAITS>::Assure1Reference (COPY_ARGS&& ... copyArgs)
            {
                if (not fSharedImpl_.unique ()) {
                    BreakReferences_ (forward<COPY_ARGS> (copyArgs)...);
                }
            }
            template    <typename TRAITS>
            template    <typename... COPY_ARGS>
            void    SharedByValue<TRAITS>::BreakReferences_ (COPY_ARGS&& ... copyArgs)
            {
                shared_ptr_type     ptr2Clone   { shared_impl_copier_type::Load (fSharedImpl_) };       // other thread could change this (if other thread accesses same envelope)
                // but this copy prevents the bare ptr from possibly becoming invalidated
                element_type*  ptr =   ptr2Clone.get ();
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
                *this = SharedByValue<TRAITS> (fCopier_.Copy (*ptr, forward<COPY_ARGS>(copyArgs)...), fCopier_);

#if     qDebug
                //Ensure (fSharedImpl_.unique ());
                // technically not 100% gauranteed if two threads did this at the same time, but so rare interesting if ever triggered.
                // may need to lose this assert - maybe replace with #if qDebug DbgTrace
                if (not fSharedImpl_.unique ()) {
                    DbgTrace ("probably a bug, but not necessarily...");
                }
#endif
            }


            /*
            ********************************************************************************
            ***************** SharedByValue<TRAITS>::ReadOnlyReference *********************
            ********************************************************************************
            */
            template    <typename TRAITS>
            inline  SharedByValue<TRAITS>::ReadOnlyReference::ReadOnlyReference (const SharedByValue<TRAITS>& sp)
                : fSharedPtr_ (shared_impl_copier_type::Load (sp.fSharedImpl_))
            {
                RequireNotNull (fSharedPtr_);
            }
            template    <typename TRAITS>
            inline  SharedByValue<TRAITS>::ReadOnlyReference::ReadOnlyReference (const ReadOnlyReference&& r)
                : fSharedPtr_ (move (r.fSharedPtr_))
            {
            }
            template    <typename TRAITS>
            inline  typename SharedByValue<TRAITS>::ReadOnlyReference& SharedByValue<TRAITS>::ReadOnlyReference::operator= (const ReadOnlyReference && rhs)
            {
                fSharedPtr_ = move (rhs.fSharedPtr_);
                return *this;
            }
            template    <typename TRAITS>
            inline  const typename SharedByValue<TRAITS>::element_type*     SharedByValue<TRAITS>::ReadOnlyReference::cget () const
            {
                EnsureNotNull (fSharedPtr_);
                return fSharedPtr_.get ();
            }
            template    <typename TRAITS>
            const typename SharedByValue<TRAITS>::element_type&     SharedByValue<TRAITS>::ReadOnlyReference::operator* () const
            {
                EnsureNotNull (fSharedPtr_);
                return *fSharedPtr_.get ();
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Memory_SharedByValue_inl_*/
