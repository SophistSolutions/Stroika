/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Optional_inl_
#define _Stroika_Foundation_Memory_Optional_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"
#include    "../Execution/Common.h"
#include    "../Execution/Exceptions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            /*
             ********************************************************************************
             *********************** Optional_DefaultTraits<T> ******************************
             ********************************************************************************
             */
            template    <typename T>
            inline  int     Optional_DefaultTraits<T>::Compare (T lhs, T rhs)
            {
                return Common::ComparerWithWellOrder<T>::Compare (lhs, rhs);
            }
            template    <typename T>
            inline  bool    Optional_DefaultTraits<T>::Equals (T lhs, T rhs)
            {
                return Common::ComparerWithEquals<T>::Equals (lhs, rhs);
            }


            /*
             ********************************************************************************
             *********************************** Optional<T> ********************************
             ********************************************************************************
             */
#if qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
            template    <typename T>
            inline      void    Optional<T>::destroy_ (T* p)
            {
                AssertNotNull (p);
                p->~T ();
            }
#endif
            template    <typename T>
            inline
#if     !qCompilerAndStdLib_constexpr_Buggy
            constexpr
#endif
            Optional<T>::Optional ()
            {
            }
            template    <typename T>
            inline  Optional<T>::Optional (const T& from)
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                : fValue_ (new (fBuffer_) T (from))
#else
                : fValue_ (new AutomaticallyBlockAllocated<T> (from))
#endif
            {
            }
            template    <typename T>
            inline  Optional<T>::Optional (T&& from)
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                : fValue_ (new (fBuffer_) T (std::move (from)))
#else
                : fValue_ (new AutomaticallyBlockAllocated<T> (std::move (from)))
#endif
            {
            }
            template    <typename T>
            inline  Optional<T>::Optional (const Optional<T>& from)
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                : fValue_ (from.fValue_ == nullptr ? nullptr : new (fBuffer_) T (*from.fValue_))
#else
                : fValue_ (from.fValue_ == nullptr ? nullptr : new AutomaticallyBlockAllocated<T> (*from))
#endif
            {
            }
            template    <typename T>
            inline  Optional<T>::Optional (Optional<T>&& from)
#if     !qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                : fValue_ (from.fValue_)
#endif
            {
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                if (from.fValue_ != nullptr) {
                    fValue_ = new (fBuffer_) T (move (*from.fValue_));
                    from.clear ();
                }
#else
                from.fValue_ = nullptr;
#endif
            }
            template    <typename T>
            inline  Optional<T>::Optional (const T* from)
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                : fValue_ (from == nullptr ? nullptr : new (fBuffer_) T (*from))
#else
                : fValue_ (from == nullptr ? nullptr : new AutomaticallyBlockAllocated<T> (*from))
#endif
            {
            }
            template    <typename T>
            inline  Optional<T>::~Optional ()
            {
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                if (fValue_ != nullptr) {
                    destroy_ (fValue_);
                }
#else
                delete fValue_;
#endif
            }
            template    <typename T>
            inline  Optional<T>&   Optional<T>::operator= (const T& rhs)
            {
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                if (fValue_ == &rhs) {
                    // No need to copy in this case and would be bad to try
                    //  Optional<T> x;
                    //  x = *x;
                }
                else {
                    if (fValue_ == nullptr) {
                        fValue_ = new (fBuffer_) T (rhs);
                    }
                    else {
                        *fValue_ = rhs;
                    }
                }
#else
                if (fValue_->get () == &rhs) {
                    // No need to copy in this case and would be bad to try
                    //  Optional<T> x;
                    //  x = *x;
                }
                else {
                    if (fValue_ == nullptr) {
                        fValue_ = new AutomaticallyBlockAllocated<T> (rhs);
                    }
                    else {
                        *fValue_ = rhs;
                    }
                }
#endif
                return *this;
            }
            template    <typename T>
            inline  Optional<T>&   Optional<T>::operator= (T && rhs)
            {
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                if (fValue_ == &rhs) {
                    // No need to move in this case and would be bad to try
                    //  Optional<T> x;
                    //  x = *x;
                }
                else {
                    if (fValue_ == nullptr) {
                        fValue_ = new (fBuffer_) T (std::move (rhs));
                    }
                    else {
                        *fValue_ = std::move (rhs);
                    }
                }
#else
                if (fValue_->get () == &rhs) {
                    // No need to copy in this case and would be bad to try
                    //  Optional<T> x;
                    //  x = *x;
                }
                else {
                    if (fValue_ == nullptr) {
                        fValue_ = new AutomaticallyBlockAllocated<T> (std::move (rhs));
                    }
                    else {
                        *fValue_ = std::move (rhs);
                    }
                }
#endif
                return *this;
            }
            template    <typename T>
            inline  Optional<T>&   Optional<T>::operator= (const Optional<T>& rhs)
            {
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                if (fValue_ != rhs.fValue_) {
                    if (fValue_ != nullptr) {
                        destroy_ (fValue_);
                        fValue_ = nullptr;
                    }
                    if (rhs.fValue_ != nullptr) {
                        fValue_ = new (fBuffer_) T (*rhs.fValue_);
                    }
                }
#else
                if (fValue_->get () != rhs.fValue_->get ()) {
                    delete fValue_;
                    fValue_ = nullptr;
                    if (rhs.fValue_ != nullptr) {
                        fValue_ = new AutomaticallyBlockAllocated<T> (*rhs);
                    }
                }
#endif
                return *this;
            }
            template    <typename T>
            inline  Optional<T>&   Optional<T>::operator= (Optional<T> && rhs)
            {
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                if (fValue_ != rhs.fValue_) {
                    clear ();
                    if (rhs.fValue_ != nullptr) {
                        fValue_ = new (fBuffer_) T (move (*rhs.fValue_));
                        rhs.clear ();
                    }
                }
#else
                if (fValue_->get () != rhs.fValue_->get ()) {
                    delete fValue_;
                    fValue_ = rhs.fValue_;
                    rhs.fValue_ = nullptr;
                }
#endif
                return *this;
            }
            template    <typename T>
            inline  Optional<T>&   Optional<T>::operator= (const T* rhs)
            {
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                if (fValue_ != rhs) {
                    if (rhs == nullptr) {
                        clear ();
                    }
                    else {
                        if (fValue_ == nullptr) {
                            fValue_ = new (fBuffer_) T (*rhs);
                        }
                        else {
                            *fValue_ = *rhs;
                        }
                    }
                }
#else
                if (fValue_->get () != rhs) {
                    delete fValue_;
                    fValue_ = nullptr;
                    if (rhs != nullptr) {
                        fValue_ = new AutomaticallyBlockAllocated<T> (*rhs);
                    }
                }
#endif
                return *this;
            }
            template    <typename T>
            inline  void    Optional<T>::clear ()
            {
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                if (fValue_ != nullptr) {
                    destroy_ (fValue_);
                }
#else
                delete fValue_;
#endif
                fValue_ = nullptr;
            }
#if 0
            template    <typename T>
            inline  T*    Optional<T>::get ()
            {
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                return fValue_;
#else
                return fValue_ == nullptr ? nullptr : fValue_->get ();
#endif
            }
#endif
            template    <typename T>
            inline  const T*    Optional<T>::get () const
            {
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                return fValue_;
#else
                return fValue_ == nullptr ? nullptr : fValue_->get ();
#endif
            }
            template    <typename T>
            inline  constexpr   bool    Optional<T>::IsMissing () const noexcept
            {
                return fValue_ == nullptr;
            }
            template    <typename T>
            inline  constexpr   bool    Optional<T>::IsPresent () const noexcept
            {
                return fValue_ != nullptr;
            }
            template    <typename T>
            inline  Optional<T>::operator bool () const noexcept
            {
                return IsPresent ();
            }
            template    <typename T>
            inline  T Optional<T>::Value (T defaultValue) const
            {
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                return IsMissing () ? defaultValue : *fValue_;
#else
                return IsMissing () ? defaultValue : *fValue_->get ();
#endif
            }
            template    <typename T>
            template    <typename   THROW_IF_MISSING_TYPE>
            inline  T   Optional<T>::CheckedValue (const THROW_IF_MISSING_TYPE& exception2ThrowIfMissing) const
            {
                if (IsMissing ()) {
                    Execution::DoThrow (exception2ThrowIfMissing);
                }
                else {
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                    return *fValue_;
#else
                    return *fValue_->get ();
#endif
                }
            }
            template    <typename T>
            template    <typename   CONVERTABLE_TO_TYPE>
            nonvirtual  void    Optional<T>::AssignIf (CONVERTABLE_TO_TYPE* to) const
            {
                RequireNotNull (to);
                if (IsPresent ()) {
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                    *to = *fValue_;
#else
                    *to = *fValue_->get ();
#endif
                }
            }
            template    <typename T>
            inline  auto Optional<T>::operator-> () const -> Holder_
            {
                Require (IsPresent ());
                AssertNotNull (fValue_);
                return Holder_ { this };
#if 0
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                EnsureNotNull (fValue_);
                return fValue_;
#else
                EnsureNotNull (fValue_->get ());
                return fValue_->get ();
#endif
#endif
            }
#if 0
            template    <typename T>
            inline  T* Optional<T>::operator-> ()
            {
                Require (IsPresent ());
                AssertNotNull (fValue_);
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                EnsureNotNull (fValue_);
                return fValue_;
#else
                EnsureNotNull (fValue_->get ());
                return fValue_->get ();
#endif
            }
#endif
            template    <typename T>
            inline  T   Optional<T>::operator* () const
            {
                Require (IsPresent ());
                AssertNotNull (fValue_);
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                EnsureNotNull (fValue_);
                return *fValue_;
#else
                EnsureNotNull (fValue_->get ());
                return *fValue_->get ();
#endif
            }
            template    <typename T>
            inline  Optional<T>&    Optional<T>::operator+= (const T& rhs)
            {
                Require (IsPresent ());
                AssertNotNull (fValue_);
                *fValue_ += rhs;
                return *this;
            }
            template    <typename T>
            inline  Optional<T>&    Optional<T>::operator-= (const T& rhs)
            {
                Require (IsPresent ());
                AssertNotNull (fValue_);
                *fValue_ -= rhs;
                return *this;
            }
            template    <typename T>
            inline  Optional<T>&    Optional<T>::operator*= (const T& rhs)
            {
                Require (IsPresent ());
                AssertNotNull (fValue_);
                *fValue_ *= rhs;
                return *this;
            }
            template    <typename T>
            inline  Optional<T>&    Optional<T>::operator/= (const T& rhs)
            {
                Require (IsPresent ());
                AssertNotNull (fValue_);
                *fValue_ /= rhs;
                return *this;
            }
            template    <typename T>
            inline  bool    Optional<T>::Equals (const Optional<T>& rhs) const
            {
                if (fValue_ == nullptr) {
                    return rhs.fValue_ == nullptr;
                }
                if (rhs.fValue_ == nullptr) {
                    AssertNotNull (fValue_);
                    return false;
                }
                AssertNotNull (fValue_);
                AssertNotNull (rhs.fValue_);
                return Optional_DefaultTraits<T>::Equals (*fValue_, *rhs.fValue_);
            }
            template    <typename T>
            inline  bool    Optional<T>::Equals (T rhs) const
            {
                if (fValue_ == nullptr) {
                    return false;
                }
                AssertNotNull (fValue_);
                return Optional_DefaultTraits<T>::Equals (*fValue_, rhs);
            }
            template    <typename T>
            inline  int Optional<T>::Compare (const Optional<T>& rhs) const
            {
                if (fValue_ == nullptr) {
                    return (rhs.fValue_ == nullptr) ? 0 : 1; // arbitrary choice - but assume if lhs is empty thats less than any T value
                }
                if (rhs.fValue_ == nullptr) {
                    AssertNotNull (fValue_);
                    return -1;
                }
                AssertNotNull (fValue_);
                AssertNotNull (rhs.fValue_);
                return Optional_DefaultTraits<T>::Compare (*fValue_, *rhs.fValue_);
            }
            template    <typename T>
            inline  int Optional<T>::Compare (T rhs) const
            {
                if (fValue_ == nullptr) {
                    return 1; // arbitrary choice - but assume if lhs is empty thats less than any T value
                }
                AssertNotNull (fValue_);
                return Optional_DefaultTraits<T>::Compare (*fValue_, rhs);
            }


            /*
             ********************************************************************************
             ************************************ operator< *********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  bool    operator< (const Optional<T>& lhs, const Optional<T>& rhs)
            {
                return lhs.Compare (rhs) < 0;
            }
            template    <typename T, typename RHS_CONVERTIBLE_TO_OPTIONAL_OF_T>
            inline  bool    operator< (const Optional<T>& lhs, const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T& rhs)
            {
                return lhs.Compare (Optional<T> (rhs)) < 0;
            }


            /*
             ********************************************************************************
             ************************************ operator<= ********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  bool    operator<= (const Optional<T>& lhs, const Optional<T>& rhs)
            {
                return lhs.Compare (rhs) <= 0;
            }
            template    <typename T, typename RHS_CONVERTIBLE_TO_OPTIONAL_OF_T>
            inline  bool    operator<= (const Optional<T>& lhs, const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T& rhs)
            {
                return lhs.Compare (Optional<T> (rhs)) <= 0;
            }


            /*
             ********************************************************************************
             *********************************** operator== *********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  bool    operator== (const Optional<T>& lhs, T rhs)
            {
                return lhs.Equals (rhs);
            }
            template    <typename T>
            inline  bool    operator== (T lhs, const Optional<T>& rhs)
            {
                return rhs.Equals (lhs);    // symetric so reverse
            }
            template    <typename T>
            inline  bool    operator== (const Optional<T>& lhs, const Optional<T>& rhs)
            {
                return lhs.Equals (rhs);
            }
            template    <typename T, typename RHS_CONVERTABLE_TO_OPTIONAL, typename SFINAE_CHECK>
            inline  bool    operator== (const Optional<T>& lhs, RHS_CONVERTABLE_TO_OPTIONAL rhs)
            {
                return lhs.Equals (Optional<T> { rhs });
            }


            /*
             ********************************************************************************
             *********************************** operator!= *********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  bool    operator!= (const Optional<T>& lhs, T rhs)
            {
                return not lhs.Equals (rhs);
            }
            template    <typename T>
            inline  bool    operator!= (T lhs, const Optional<T>& rhs)
            {
                return not rhs.Equals (lhs);    // take advantage of commutativity
            }
            template    <typename T>
            inline  bool    operator!= (const Optional<T>& lhs, const Optional<T>& rhs)
            {
                return not lhs.Equals (rhs);
            }
            template    <typename T, typename RHS_CONVERTABLE_TO_OPTIONAL, typename SFINAE_CHECK>
            inline  bool    operator!= (const Optional<T>& lhs, RHS_CONVERTABLE_TO_OPTIONAL rhs)
            {
                return not lhs.Equals (Optional<T> { rhs });
            }


            /*
             ********************************************************************************
             *********************************** operator>= *********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  bool    operator>= (const Optional<T>& lhs, const Optional<T>& rhs)
            {
                return lhs.Compare (rhs) >= 0;
            }
            template    <typename T, typename   RHS_CONVERTIBLE_TO_OPTIONAL_OF_T>
            inline  bool    operator>= (const Optional<T>& lhs, const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T& rhs)
            {
                return lhs.Compare (Optional<T> (rhs)) >= 0;
            }


            /*
             ********************************************************************************
             ************************************ operator> *********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  bool    operator> (const Optional<T>& lhs, const Optional<T>& rhs)
            {
                return lhs.Compare (rhs) > 0;
            }
            template    <typename T, typename   RHS_CONVERTIBLE_TO_OPTIONAL_OF_T>
            inline  bool    operator> (const Optional<T>& lhs, const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T& rhs)
            {
                return lhs.Compare (Optional<T> (rhs)) > 0;
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Memory_Optional_inl_*/
