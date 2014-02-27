/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_SharedPtr_inl_
#define _Stroika_Foundation_Memory_SharedPtr_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BlockAllocated.h"

#include    "../Execution/Exceptions.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            /*
             * IMPLEMENTATION NOTES:
             *
             *      We have Decrement() return a boolean and if it returns true to delete in the
             *  caller because otehrwise the PRIVATE stuff (if you use SharedRep<> of a private type) -
             *  generates errors. At least it causes problems on Visual Studio.Net 2010. I'm not
             *  sure if this is MY bug or a compiler  bug. Anyhow - this is fine for now...
             *          -- LGP 2012-05-15
             */


            namespace   Private_ {
                namespace   SharedPtr_Default_Traits_Helpers_ {

                }
            }


            namespace   Private_ {
                namespace   enable_shared_from_this_Traits_Helpers_ {

                }
            }


            /*
             ********************************************************************************
             ***************************** SharedPtr<T> ***************************
             ********************************************************************************
             */
            template    <typename T>
            inline  SharedPtr<T>::SharedPtr () noexcept
:
            fEnvelope_ (nullptr, nullptr, false)
            {
            }
            template    <typename T>
            inline  SharedPtr<T>::SharedPtr (nullptr_t) noexcept
:
            fEnvelope_ (nullptr, nullptr, false)
            {
            }
            template    <typename T>
            template    <typename CHECK_KEY>
            inline  SharedPtr<T>::SharedPtr (T* from, typename enable_if < is_convertible<CHECK_KEY*, enable_shared_from_this<CHECK_KEY>*>::value>::type*)
                : fEnvelope_ (from, from, false)
            {
                from->fPtr_ = from;
                if (fEnvelope_.GetPtr () != nullptr) {
                    // NB: the fEnvelope_.CurrentRefCount () USUALLY == 0, but not necessarily, if the refcount is stored
                    // in the 'from' - (see SharedPtrBase) - in which case the refcount might already be larger.
                    fEnvelope_.Increment ();
                }
            }
            template    <typename T>
            template    <typename CHECK_KEY>
            inline  SharedPtr<T>::SharedPtr (T* from, typename enable_if < !is_convertible<CHECK_KEY*, enable_shared_from_this<CHECK_KEY>*>::value >::type*)
#if     qAllowBlockAllocation
                : fEnvelope_ (from, BlockAllocator<Private_::ReferenceCounterContainerType_>::New (), true)
#else
                : fEnvelope_ (from, new Private_::ReferenceCounterContainerType_ (), true)
#endif

            {
                if (fEnvelope_.GetPtr () != nullptr) {
                    // NB: the fEnvelope_.CurrentRefCount () USUALLY == 0, but not necessarily, if the refcount is stored
                    // in the 'from' - (see SharedPtrBase) - in which case the refcount might already be larger.
                    fEnvelope_.Increment ();
                }
            }
            template    <typename T>
            inline  SharedPtr<T>::SharedPtr (const Envelope_& from) noexcept
:
            fEnvelope_ (from)
            {
                if (fEnvelope_.GetPtr () != nullptr) {
                    fEnvelope_.Increment ();
                }
            }
            template    <typename T>
            inline  SharedPtr<T>::SharedPtr (const SharedPtr<T>& from) noexcept
:
            fEnvelope_ (from.fEnvelope_)
            {
                if (fEnvelope_.GetPtr () != nullptr) {
                    fEnvelope_.Increment ();
                }
            }
            template    <typename T>
            inline  SharedPtr<T>::SharedPtr (SharedPtr<T>&& from) noexcept
:
            fEnvelope_ (move (from.fEnvelope_))
            {
                // no need to increment refcount here because the entire envelope moved from from to this, and so total counts same
            }
            template    <typename T>
            template    <typename T2>
            SharedPtr<T>::SharedPtr (const SharedPtr<T2>& from) noexcept
:
            fEnvelope_ (from.fEnvelope_)
            {
                if (fEnvelope_.GetPtr () != nullptr) {
                    fEnvelope_.Increment ();
                }
            }
            template    <typename T>
            inline  SharedPtr<T>& SharedPtr<T>::operator= (const SharedPtr<T>& rhs) noexcept {
                if (rhs.fEnvelope_.GetPtr () != fEnvelope_.GetPtr ())
                {
                    if (fEnvelope_.GetPtr () != nullptr) {
                        if (fEnvelope_.Decrement ()) {
                            fEnvelope_.DoDeleteCounter ();
                            delete fEnvelope_.GetPtr ();
                            fEnvelope_.SetPtr (nullptr);
                        }
                    }
                    fEnvelope_ = rhs.fEnvelope_;
                    if (fEnvelope_.GetPtr () != nullptr) {
                        Assert (fEnvelope_.CurrentRefCount () > 0);
                        fEnvelope_.Increment ();
                    }
                }
                return *this;
            }
            template    <typename T>
            inline  SharedPtr<T>::~SharedPtr ()
            {
                if (fEnvelope_.GetPtr () != nullptr) {
                    if (fEnvelope_.Decrement ()) {
                        fEnvelope_.DoDeleteCounter ();
                        delete fEnvelope_.GetPtr ();
                    }
                }
            }
            template    <typename T>
            inline  bool    SharedPtr<T>::IsNull () const noexcept
            {
                return fEnvelope_.GetPtr () == nullptr;
            }
            template    <typename T>
            inline  T&  SharedPtr<T>::GetRep () const noexcept
            {
                RequireNotNull (fEnvelope_.GetPtr ());
                Assert (fEnvelope_.CurrentRefCount () > 0);
                return *fEnvelope_.GetPtr ();
            }
            template    <typename T>
            inline  T* SharedPtr<T>::operator-> () const noexcept
            {
                return &GetRep ();
            }
            template    <typename T>
            inline  T& SharedPtr<T>::operator* () const noexcept
            {
                return GetRep ();
            }
            template    <typename T>
            inline  SharedPtr<T>::operator T* () const noexcept
            {
                return fEnvelope_.GetPtr ();
            }
            template    <typename T>
            inline  T*  SharedPtr<T>::get () const noexcept
            {
                return fEnvelope_.GetPtr ();
            }
            template    <typename T>
            inline  void    SharedPtr<T>::release () noexcept {
                *this = SharedPtr<T> (nullptr);
            }
            template    <typename T>
            inline  void    SharedPtr<T>::clear () noexcept {
                release ();
            }
            template    <typename T>
            inline  void    SharedPtr<T>::reset (T* p)
            {
                if (fEnvelope_.GetPtr () != p) {
                    *this = SharedPtr<T> (p);
                }
            }
            template    <typename T>
            template    <typename T2>
            inline  SharedPtr<T2> SharedPtr<T>::Dynamic_Cast () const  noexcept
            {
                return SharedPtr<T2> (typename SharedPtr<T2>::Envelope_ (fEnvelope_, dynamic_cast<T2*> (get ())));
            }
            template    <typename T>
            inline  typename SharedPtr<T>::ReferenceCountType   SharedPtr<T>::CurrentRefCount () const noexcept
            {
                return fEnvelope_.CurrentRefCount ();
            }
            template    <typename T>
            inline  typename SharedPtr<T>::ReferenceCountType   SharedPtr<T>::use_count () const noexcept
            {
                return fEnvelope_.CurrentRefCount ();
            }
            template    <typename T>
            inline  bool    SharedPtr<T>::IsUnique () const noexcept
            {
                return fEnvelope_.CurrentRefCount () == 1;
            }
            template    <typename T>
            inline  bool    SharedPtr<T>::unique () const noexcept
            {
                // respect the stl-ish names
                return IsUnique ();
            }
            template    <typename T>
            inline  bool    SharedPtr<T>::operator< (const SharedPtr<T>& rhs) const noexcept
            {
                // not technically legal to compare pointers this way, but its is legal to convert to int, and then compare, and
                // this does the same thing...
                //      -- LGP 2009-01-11
                return fEnvelope_.GetPtr () < rhs.fEnvelope_.GetPtr ();
            }
            template    <typename T>
            inline  bool    SharedPtr<T>::operator<= (const SharedPtr<T>& rhs) const noexcept
            {
                // not technically legal to compare pointers this way, but its is legal to convert to int, and then compare, and
                // this does the same thing...
                //      -- LGP 2009-01-11
                return fEnvelope_.GetPtr () <= rhs.fEnvelope_.GetPtr ();
            }
            template    <typename T>
            inline  bool    SharedPtr<T>::operator> (const SharedPtr<T>& rhs) const noexcept
            {
                // not technically legal to compare pointers this way, but its is legal to convert to int, and then compare, and
                // this does the same thing...
                //      -- LGP 2009-01-11
                return fEnvelope_.GetPtr () > rhs.fEnvelope_.GetPtr ();
            }
            template    <typename T>
            inline  bool    SharedPtr<T>::operator>= (const SharedPtr<T>& rhs) const noexcept
            {
                // not technically legal to compare pointers this way, but its is legal to convert to int, and then compare, and
                // this does the same thing...
                //      -- LGP 2009-01-11
                return fEnvelope_.GetPtr () >= rhs.fEnvelope_.GetPtr ();
            }
            template    <typename T>
            inline  bool    SharedPtr<T>::operator== (const SharedPtr<T>& rhs) const noexcept
            {
                return fEnvelope_.GetPtr () == rhs.fEnvelope_.GetPtr ();
            }
            template    <typename T>
            inline  bool    SharedPtr<T>::operator!= (const SharedPtr<T>& rhs) const noexcept
            {
                return fEnvelope_.GetPtr () != rhs.fEnvelope_.GetPtr ();
            }


            /*
             ********************************************************************************
             ************************* enable_shared_from_this<T> ***************************
             ********************************************************************************
             */
            template    <typename   T>
            inline  enable_shared_from_this<T>::enable_shared_from_this ()
#if     qDebug
            // only initialized for assertion in shared_from_this()
                : fPtr_ (nullptr)
#endif
            {
            }
#if 0
            template    <typename   T>
            inline  enable_shared_from_this<T>::~enable_shared_from_this ()
            {
            }
#endif
            template    <typename   T>
            inline  SharedPtr<T> enable_shared_from_this<T>::shared_from_this ()
            {
                /*
                 * The Constructor for SharedPtr<T> expects a T*. However, we don't have a T*. But recall,
                 * the ONLY legal way to use this enable_shared_from_this is:
                 *
                 *       struct  TTT : Memory::enable_shared_from_this<TTT> {
                 *           string x;
                 *           ....
                 *       };
                 *
                 *   and so if we have a legal pointer to enable_shared_from_this<T>, then it MUST also be castable to a pointer to T*!!!
                 */
                //T*  tStarThis   =   dynamic_cast<T*> (this);
                //return (SharedPtr<T> (SharedPtr<T>::Envelope_ (tStarThis, this, false)));
                AssertNotNull (fPtr_);
                return (SharedPtr<T> (typename SharedPtr<T>::Envelope_ (fPtr_, this, false)));
                //return (SharedPtr<T> (Private_::SharedFromThis_Envelope_<T> (fPtr_)));
            }


        }


        namespace   Execution {
            template    <typename T>
            inline  void    ThrowIfNull (const Memory::SharedPtr<T>& p)
            {
                if (p.get () == nullptr) {
                    Execution::DoThrow (bad_alloc (), "ThrowIfNull (SharedPtr<T> ()) - throwing bad_alloc ()");
                }
            }
        }


    }
}
namespace std {
    template    <class TO_TYPE_T,   class FROM_TYPE_T>
    inline  Stroika::Foundation::Memory::SharedPtr<TO_TYPE_T>   dynamic_pointer_cast (const Stroika::Foundation::Memory::SharedPtr<FROM_TYPE_T>& sp) noexcept {
        return sp.template Dynamic_Cast<TO_TYPE_T> ();
    }
}
#endif  /*_Stroika_Foundation_Memory_SharedPtr_inl_*/
