/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_SharedByValue_inl_
#define _Stroika_Foundation_Memory_SharedByValue_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */


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
            inline  SharedByValue_CopyByFunction<T, SHARED_IMLP>::SharedByValue_CopyByFunction (SHARED_IMLP (*copier) (const T&))
                : fCopier (copier)
            {
            }
            template    <typename   T, typename SHARED_IMLP>
            inline  SHARED_IMLP  SharedByValue_CopyByFunction<T, SHARED_IMLP>::Copy (const T& t) const
            {
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
            ****************************** SharedByValue<TRAITS> ***************************
            ********************************************************************************
            */
            template    <typename TRAITS>
            inline  SharedByValue<TRAITS>::SharedByValue ()
                : fCopier_ (copier_type ())
                , fSharedImpl_ ()
            {
            }
            template    <typename TRAITS>
            inline  SharedByValue<TRAITS>::SharedByValue (nullptr_t n)
                : fCopier_ (copier_type ())
                , fSharedImpl_ ()
            {
            }
            template    <typename TRAITS>
            inline  SharedByValue<TRAITS>::SharedByValue (const SharedByValue<TRAITS>& from)
                : fCopier_ (from.fCopier_)
                , fSharedImpl_ (from.fSharedImpl_)
            {
            }
            template    <typename TRAITS>
            inline  SharedByValue<TRAITS>::SharedByValue (const SharedByValue<TRAITS>&&  from)
                : fCopier_ (std::move (from.fCopier_))
                , fSharedImpl_ (std::move (from.fSharedImpl_))
            {
            }
            template    <typename TRAITS>
            inline  SharedByValue<TRAITS>::SharedByValue (const shared_ptr_type& from, const copier_type& copier)
                : fCopier_ (copier)
                , fSharedImpl_ (from)
            {
            }
            template    <typename TRAITS>
            inline  SharedByValue<TRAITS>::SharedByValue (const shared_ptr_type&&  from, const copier_type& copier)
                : fCopier_ (std::move (copier))
                , fSharedImpl_ (std::move (from))
            {
            }
            template    <typename TRAITS>
            inline  SharedByValue<TRAITS>::SharedByValue (element_type* from, const copier_type& copier)
                : fCopier_ (copier)
                , fSharedImpl_ (from)
            {
            }
            template    <typename TRAITS>
            inline  SharedByValue<TRAITS>& SharedByValue<TRAITS>::operator= (const SharedByValue<TRAITS>& src)
            {
                if (this != &src) {
                    fCopier_ = src.fCopier_;
                    fSharedImpl_ = src.fSharedImpl_;
                }
                return *this;
            }
            template    <typename TRAITS>
            inline  const typename SharedByValue<TRAITS>::element_type*    SharedByValue<TRAITS>::get () const
            {
                return (fSharedImpl_.get ());
            }
            template    <typename TRAITS>
            typename SharedByValue<TRAITS>::element_type* SharedByValue<TRAITS>::get ()
            {
                element_type*  ptr =   fSharedImpl_.get ();
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
            template    <typename TRAITS>
            inline  const typename SharedByValue<TRAITS>::element_type*    SharedByValue<TRAITS>::operator-> () const
            {
                return (fSharedImpl_.get ());
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
                return (*ptr);
            }
            template    <typename TRAITS>
            typename SharedByValue<TRAITS>::element_type& SharedByValue<TRAITS>::operator* ()
            {
                /*
                 * For non-const dereferencing, we must clone ourselves (if there are
                 * extra referneces).
                 */
                Assure1Reference ();
                element_type*  ptr =   get ();
                EnsureNotNull (ptr);
                return (*ptr);
            }
            template    <typename TRAITS>
            bool SharedByValue<TRAITS>::operator== (const SharedByValue<TRAITS>& rhs) const
            {
                return fSharedImpl_ == rhs.fSharedImpl_;
            }
            template    <typename TRAITS>
            bool SharedByValue<TRAITS>::operator!= (const SharedByValue<TRAITS>& rhs) const
            {
                return fSharedImpl_ != rhs.fSharedImpl_;
            }
            template    <typename TRAITS>
            bool    SharedByValue<TRAITS>::unique () const
            {
                return fSharedImpl_.unique ();
            }
            template    <typename TRAITS>
            inline  void    SharedByValue<TRAITS>::Assure1Reference ()
            {
                if (!fSharedImpl_.unique ()) {
                    BreakReferences_ ();
                }
            }
            template    <typename TRAITS>
            void    SharedByValue<TRAITS>::BreakReferences_ ()
            {
                element_type*  ptr =   fSharedImpl_.get ();
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
                *this = SharedByValue<TRAITS> (fCopier_.Copy (*ptr), fCopier_);
                Ensure (fSharedImpl_.unique ());
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Memory_SharedByValue_inl_*/
