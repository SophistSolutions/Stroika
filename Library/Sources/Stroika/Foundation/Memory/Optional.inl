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
             ************************** Optional<T, TRAITS>::ConstHolder_ ***************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::ConstHolder_::ConstHolder_ (const Optional* p)
                : fVal (p)
#if     qDebug
                , fCritSec_ { p->fDebugMutex_ }
#endif
            {
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::ConstHolder_::ConstHolder_ (ConstHolder_&& from)
                : fVal (from.fVal)
            {
                from.fVal = nullptr;
            }
            template    <typename T, typename TRAITS>
            inline  const T* Optional<T, TRAITS>::ConstHolder_::operator-> () const
            {
                return fVal->get ();
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::ConstHolder_::operator const T& () const
            {
                return *fVal->get ();
            }
            template    <typename T, typename TRAITS>
            inline  const T& Optional<T, TRAITS>::ConstHolder_::operator* () const
            {
                return *fVal->get ();
            }


            /*
             ********************************************************************************
             *************************** Optional<T, TRAITS> ********************************
             ********************************************************************************
             */
#if qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
            template    <typename T, typename TRAITS>
            inline      void    Optional<T, TRAITS>::destroy_ (T* p)
            {
                AssertNotNull (p);
                p->~T ();
            }
#endif
            template    <typename T, typename TRAITS>
            inline  void    Optional<T, TRAITS>::clear_ ()
            {
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                if (fStorage_.fValue_ != nullptr) {
                    destroy_ (fStorage_.fValue_);
                }
#else
                delete fStorage_.fValue_;
#endif
                fStorage_.fValue_ = nullptr;
            }
            template    <typename T, typename TRAITS>
            inline
#if     !qCompilerAndStdLib_constexpr_Buggy
            constexpr
#endif
            Optional<T, TRAITS>::Optional ()
            {
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (const T& from)
            {
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                fStorage_.fValue_ = new (fStorage_.fBuffer_) T (from);
#else
                fStorage_.fValue_  = new AutomaticallyBlockAllocated<T> (from);
#endif
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (T&& from)
            {
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                fStorage_.fValue_ = new (fStorage_.fBuffer_) T (std::move (from));
#else
                fStorage_.fValue_ = new AutomaticallyBlockAllocated<T> (std::move (from));
#endif
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (const Optional& from)
            {
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
#if     qDebug
                auto critSec { Execution::make_unique_lock (from.fDebugMutex_) };
#endif
                if (from.fStorage_.fValue_ != nullptr) {
                    fStorage_.fValue_ = new (fStorage_.fBuffer_) T (*from.fStorage_.fValue_);
                }
#else
                fStorage_.fValue_ = (from.fStorage_.fValue_ == nullptr ? nullptr : new AutomaticallyBlockAllocated<T> (*from));
#endif
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (Optional&& from)
            {
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
#if     qDebug
                auto    critSec2 { Execution::make_unique_lock (from.fDebugMutex_) };
#endif
                if (from.fStorage_.fValue_ != nullptr) {
                    fStorage_.fValue_ = new (fStorage_.fBuffer_) T (move (*from.fStorage_.fValue_));
                    from.clear_ ();
                }
#else
                fStorage_.fValue_ = from.fStorage_.fValue_;
                from.fStorage_.fValue_ = nullptr;
#endif
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (const T* from)
            {
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                fStorage_.fValue_ = (from == nullptr ? nullptr : new (fBuffer_) T (*from));
#else
                fStorage_.fValue_ = (from == nullptr ? nullptr : new AutomaticallyBlockAllocated<T> (*from));
#endif
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::~Optional ()
            {
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                if (fStorage_.fValue_ != nullptr) {
                    destroy_ (fStorage_.fValue_);
                }
#else
                delete fStorage_.fValue_;
#endif
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (const T& rhs)
            {
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                if (fStorage_.fValue_ == &rhs) {
                    // No need to copy in this case and would be bad to try
                    //  Optional<T> x;
                    //  x = *x;
                }
                else {
                    if (fStorage_.fValue_ == nullptr) {
                        fStorage_.fValue_ = new (fStorage_.fBuffer_) T (rhs);
                    }
                    else {
                        *fStorage_.fValue_ = rhs;
                    }
                }
#else
                if (fStorage_.fValue_->get () == &rhs) {
                    // No need to copy in this case and would be bad to try
                    //  Optional<T> x;
                    //  x = *x;
                }
                else {
                    if (fStorage_.fValue_ == nullptr) {
                        fStorage_.fValue_ = new AutomaticallyBlockAllocated<T> (rhs);
                    }
                    else {
                        *fStorage_.fValue_ = rhs;
                    }
                }
#endif
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (T && rhs)
            {
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                if (fStorage_.fValue_ == &rhs) {
                    // No need to move in this case and would be bad to try
                    //  Optional<T> x;
                    //  x = *x;
                }
                else {
                    if (fStorage_.fValue_ == nullptr) {
                        fStorage_.fValue_ = new (fStorage_.fBuffer_) T (std::move (rhs));
                    }
                    else {
                        *fStorage_.fValue_ = std::move (rhs);
                    }
                }
#else
                if (fStorage_.fValue_->get () == &rhs) {
                    // No need to copy in this case and would be bad to try
                    //  Optional<T> x;
                    //  x = *x;
                }
                else {
                    if (fStorage_.fValue_ == nullptr) {
                        fStorage_.fValue_ = new AutomaticallyBlockAllocated<T> (std::move (rhs));
                    }
                    else {
                        *fStorage_.fValue_ = std::move (rhs);
                    }
                }
#endif
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (const Optional& rhs)
            {
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                if (fStorage_.fValue_ != rhs.fStorage_.fValue_) {
                    if (fStorage_.fValue_ != nullptr) {
                        destroy_ (fStorage_.fValue_);
                        fStorage_.fValue_ = nullptr;
                    }
#if     qDebug
                    auto    critSec2 { Execution::make_unique_lock (rhs.fDebugMutex_) };
#endif
                    if (rhs.fStorage_.fValue_ != nullptr) {
                        fStorage_.fValue_ = new (fStorage_.fBuffer_) T (*rhs.fStorage_.fValue_);
                    }
                }
#else
                if (fStorage_.fValue_->get () != rhs.fStorage_.fValue_->get ()) {
                    delete fStorage_.fValue_;
                    fStorage_.fValue_ = nullptr;
                    if (rhs.fStorage_.fValue_ != nullptr) {
                        fStorage_.fValue_ = new AutomaticallyBlockAllocated<T> (*rhs);
                    }
                }
#endif
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (Optional && rhs)
            {
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                if (fStorage_.fValue_ != rhs.fStorage_.fValue_) {
                    clear_ ();
#if     qDebug
                    auto    critSec2 { Execution::make_unique_lock (rhs.fDebugMutex_) };
#endif
                    if (rhs.fStorage_.fValue_ != nullptr) {
                        fStorage_.fValue_ = new (fStorage_.fBuffer_) T (move (*rhs.fStorage_.fValue_));
                        rhs.clear_ ();
                    }
                }
#else
                if (fStorage_.fValue_->get () != rhs.fStorage_.fValue_->get ()) {
                    delete fStorage_.fValue_;
                    fStorage_.fValue_ = rhs.fStorage_.fValue_;
                    rhs.fStorage_.fValue_ = nullptr;
                }
#endif
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (const T* rhs)
            {
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                if (fStorage_.fValue_ != rhs) {
                    if (rhs == nullptr) {
                        clear_ ();
                    }
                    else {
                        if (fStorage_.fValue_ == nullptr) {
                            fStorage_.fValue_ = new (fStorage_.fBuffer_) T (*rhs);
                        }
                        else {
                            *fStorage_.fValue_ = *rhs;
                        }
                    }
                }
#else
                if (fStorage_.fValue_->get () != rhs) {
                    delete fStorage_.fValue_;
                    fStorage_.fValue_ = nullptr;
                    if (rhs != nullptr) {
                        fStorage_.fValue_ = new AutomaticallyBlockAllocated<T> (*rhs);
                    }
                }
#endif
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  void    Optional<T, TRAITS>::clear ()
            {
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
                clear_ ();
                Ensure (fStorage_.fValue_ == nullptr);
            }
            template    <typename T, typename TRAITS>
            inline  const T*    Optional<T, TRAITS>::get () const
            {
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                return fStorage_.fValue_;
#else
                return fStorage_.fValue_ == nullptr ? nullptr : fStorage_.fValue_->get ();
#endif
            }
            template    <typename T, typename TRAITS>
            inline  constexpr   bool    Optional<T, TRAITS>::IsMissing () const noexcept
            {
                return fStorage_.fValue_ == nullptr;
            }
            template    <typename T, typename TRAITS>
            inline  constexpr   bool    Optional<T, TRAITS>::IsPresent () const noexcept
            {
                return fStorage_.fValue_ != nullptr;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::operator bool () const noexcept
            {
                return IsPresent ();
            }
            template    <typename T, typename TRAITS>
            inline  T Optional<T, TRAITS>::Value (T defaultValue) const
            {
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                return IsMissing () ? defaultValue : *fStorage_.fValue_;
#else
                return IsMissing () ? defaultValue : *fStorage_.fValue_->get ();
#endif
            }
            template    <typename T, typename TRAITS>
            template    <typename   THROW_IF_MISSING_TYPE>
            inline  T   Optional<T, TRAITS>::CheckedValue (const THROW_IF_MISSING_TYPE& exception2ThrowIfMissing) const
            {
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
                if (IsMissing ()) {
                    Execution::DoThrow (exception2ThrowIfMissing);
                }
                else {
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                    return *fStorage_.fValue_;
#else
                    return *fStorage_.fValue_->get ();
#endif
                }
            }
            template    <typename T, typename TRAITS>
            template    <typename   CONVERTABLE_TO_TYPE>
            nonvirtual  void    Optional<T, TRAITS>::AssignIf (CONVERTABLE_TO_TYPE* to) const
            {
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
                RequireNotNull (to);
                if (IsPresent ()) {
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                    *to = *fStorage_.fValue_;
#else
                    *to = *fStorage_.fValue_->get ();
#endif
                }
            }
            template    <typename T, typename TRAITS>
            inline  auto Optional<T, TRAITS>::operator-> () const -> ConstHolder_
            {
                // No lock on fDebugMutex_ cuz done in ConstHolder_
                Require (IsPresent ());
                AssertNotNull (fStorage_.fValue_);
                return move (ConstHolder_ { this });
            }
            template    <typename T, typename TRAITS>
            inline  auto   Optional<T, TRAITS>::operator* () const -> T
            {
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
                Require (IsPresent ());
                AssertNotNull (fStorage_.fValue_);
                //return ConstHolder_ { this };  when we embed mutex into holder
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                // No lock on fDebugMutex_ cuz done in ConstHolder_
                Require (IsPresent ());
                AssertNotNull (fStorage_.fValue_);
                return *fStorage_.fValue_;
#else
                EnsureNotNull (fStorage_.fValue_->get ());
                return *fStorage_.fValue_->get ();
#endif
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&    Optional<T, TRAITS>::operator+= (const T& rhs)
            {
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
                Require (IsPresent ());
                AssertNotNull (fStorage_.fValue_);
                *fStorage_.fValue_ += rhs;
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&    Optional<T, TRAITS>::operator-= (const T& rhs)
            {
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
                Require (IsPresent ());
                AssertNotNull (fStorage_.fValue_);
                *fStorage_.fValue_ -= rhs;
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&    Optional<T, TRAITS>::operator*= (const T& rhs)
            {
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
                Require (IsPresent ());
                AssertNotNull (fStorage_.fValue_);
                *fStorage_.fValue_ *= rhs;
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&    Optional<T, TRAITS>::operator/= (const T& rhs)
            {
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
                Require (IsPresent ());
                AssertNotNull (fStorage_.fValue_);
                *fStorage_.fValue_ /= rhs;
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Optional<T, TRAITS>::Equals (const Optional<T, TRAITS>& rhs) const
            {
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
                if (fStorage_.fValue_ == nullptr) {
                    return rhs.fStorage_.fValue_ == nullptr;
                }
                if (rhs.fStorage_.fValue_ == nullptr) {
                    AssertNotNull (fStorage_.fValue_);
                    return false;
                }
                AssertNotNull (fStorage_.fValue_);
                AssertNotNull (rhs.fStorage_.fValue_);
                return Common::ComparerWithWellOrder<T>::Equals (*fStorage_.fValue_, *rhs.fStorage_.fValue_);
            }
            template    <typename T, typename TRAITS>
            inline  bool    Optional<T, TRAITS>::Equals (T rhs) const
            {
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
                if (fStorage_.fValue_ == nullptr) {
                    return false;
                }
                AssertNotNull (fStorage_.fValue_);
                return Common::ComparerWithWellOrder<T>::Equals (*fStorage_.fValue_, rhs);
            }
            template    <typename T, typename TRAITS>
            inline  int Optional<T, TRAITS>::Compare (const Optional& rhs) const
            {
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
                if (fStorage_.fValue_ == nullptr) {
                    return (rhs.fStorage_.fValue_ == nullptr) ? 0 : 1; // arbitrary choice - but assume if lhs is empty thats less than any T value
                }
                if (rhs.fStorage_.fValue_ == nullptr) {
                    AssertNotNull (fStorage_.fValue_);
                    return -1;
                }
                AssertNotNull (fStorage_.fValue_);
                AssertNotNull (rhs.fStorage_.fValue_);
                return Common::ComparerWithWellOrder<T>::Compare (*fStorage_.fValue_, *rhs.fStorage_.fValue_);
            }
            template    <typename T, typename TRAITS>
            inline  int Optional<T, TRAITS>::Compare (T rhs) const
            {
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
                if (fStorage_.fValue_ == nullptr) {
                    return 1; // arbitrary choice - but assume if lhs is empty thats less than any T value
                }
                AssertNotNull (fStorage_.fValue_);
                return Common::ComparerWithWellOrder<T>::Compare (*fStorage_.fValue_, rhs);
            }


            /*
             ********************************************************************************
             ************************************ operator< *********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  bool    operator< (const Optional<T, TRAITS>& lhs, const Optional<T, TRAITS>& rhs)
            {
                return lhs.Compare (rhs) < 0;
            }
            template    <typename T, typename TRAITS, typename RHS_CONVERTIBLE_TO_OPTIONAL_OF_T>
            inline  bool    operator< (const Optional<T, TRAITS>& lhs, const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T& rhs)
            {
                return lhs.Compare (Optional<T, TRAITS> (rhs)) < 0;
            }


            /*
             ********************************************************************************
             ************************************ operator<= ********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  bool    operator<= (const Optional<T, TRAITS>& lhs, const Optional<T, TRAITS>& rhs)
            {
                return lhs.Compare (rhs) <= 0;
            }
            template    <typename T, typename TRAITS, typename RHS_CONVERTIBLE_TO_OPTIONAL_OF_T>
            inline  bool    operator<= (const Optional<T, TRAITS>& lhs, const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T& rhs)
            {
                return lhs.Compare (Optional<T, TRAITS> (rhs)) <= 0;
            }


            /*
             ********************************************************************************
             *********************************** operator== *********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  bool    operator== (const Optional<T, TRAITS>& lhs, T rhs)
            {
                return lhs.Equals (rhs);
            }
            template    <typename T, typename TRAITS>
            inline  bool    operator== (T lhs, const Optional<T, TRAITS>& rhs)
            {
                return rhs.Equals (lhs);    // symetric so reverse
            }
            template    <typename T, typename TRAITS>
            inline  bool    operator== (const Optional<T, TRAITS>& lhs, const Optional<T, TRAITS>& rhs)
            {
                return lhs.Equals (rhs);
            }
            template    <typename T, typename TRAITS, typename RHS_CONVERTABLE_TO_OPTIONAL, typename SFINAE_CHECK>
            inline  bool    operator== (const Optional<T, TRAITS>& lhs, RHS_CONVERTABLE_TO_OPTIONAL rhs)
            {
                return lhs.Equals (Optional<T, TRAITS> { rhs });
            }


            /*
             ********************************************************************************
             *********************************** operator!= *********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  bool    operator!= (const Optional<T, TRAITS>& lhs, T rhs)
            {
                return not lhs.Equals (rhs);
            }
            template    <typename T, typename TRAITS>
            inline  bool    operator!= (T lhs, const Optional<T, TRAITS>& rhs)
            {
                return not rhs.Equals (lhs);    // take advantage of commutativity
            }
            template    <typename T, typename TRAITS>
            inline  bool    operator!= (const Optional<T, TRAITS>& lhs, const Optional<T, TRAITS>& rhs)
            {
                return not lhs.Equals (rhs);
            }
            template    <typename T, typename TRAITS, typename RHS_CONVERTABLE_TO_OPTIONAL, typename SFINAE_CHECK>
            inline  bool    operator!= (const Optional<T, TRAITS>& lhs, RHS_CONVERTABLE_TO_OPTIONAL rhs)
            {
                return not lhs.Equals (Optional<T, TRAITS> { rhs });
            }


            /*
             ********************************************************************************
             *********************************** operator>= *********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  bool    operator>= (const Optional<T, TRAITS>& lhs, const Optional<T, TRAITS>& rhs)
            {
                return lhs.Compare (rhs) >= 0;
            }
            template    <typename T, typename TRAITS, typename   RHS_CONVERTIBLE_TO_OPTIONAL_OF_T>
            inline  bool    operator>= (const Optional<T, TRAITS>& lhs, const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T& rhs)
            {
                return lhs.Compare (Optional<T, TRAITS> (rhs)) >= 0;
            }


            /*
             ********************************************************************************
             ************************************ operator> *********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  bool    operator> (const Optional<T, TRAITS>& lhs, const Optional<T, TRAITS>& rhs)
            {
                return lhs.Compare (rhs) > 0;
            }
            template    <typename T, typename TRAITS, typename   RHS_CONVERTIBLE_TO_OPTIONAL_OF_T>
            inline  bool    operator> (const Optional<T, TRAITS>& lhs, const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T& rhs)
            {
                return lhs.Compare (Optional<T, TRAITS> (rhs)) > 0;
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Memory_Optional_inl_*/
