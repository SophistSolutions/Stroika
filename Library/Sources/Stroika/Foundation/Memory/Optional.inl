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
             ********************* Optional_Traits_Inplace_Storage<T> ***********************
             ********************************************************************************
             */
            template    <typename T>
            inline  Optional_Traits_Inplace_Storage<T>::StorageType::StorageType ()
            {
            }
            template    <typename T>
            inline  Optional_Traits_Inplace_Storage<T>::StorageType::StorageType (T* p)
                : fValue_ { p } {
            }
            template    <typename T>
            template    <typename ...ARGS>
            inline  T*  Optional_Traits_Inplace_Storage<T>::StorageType::alloc (ARGS&& ...args)
            {
                return new (fBuffer_) T (forward<ARGS> (args)...);
            }
            template    <typename T>
            inline  void    Optional_Traits_Inplace_Storage<T>::StorageType::destroy ()
            {
                // up to caller (for efficiency reasons) to make sure fValue_ cleared out (null) if neeeded
                if (fValue_ != nullptr) {
                    fValue_->~T ();
                }
            }
            template    <typename T>
            inline  void    Optional_Traits_Inplace_Storage<T>::StorageType::moveInitialize (StorageType&&  rhs)
            {
                Require (this != &rhs);
                Require (fValue_ == nullptr);
                if (rhs.fValue_ == nullptr) {
                    fValue_ = nullptr;
                }
                else {
                    fValue_ = alloc (move (*rhs.fValue_));
                    rhs.fValue_->~T ();
                    rhs.fValue_ = nullptr;
                }
                Ensure (rhs.fValue_ == nullptr);
            }
            template    <typename T>
            inline    T*  Optional_Traits_Inplace_Storage<T>::StorageType::peek ()
            {
                return fValue_;
            }
            template    <typename T>
            inline  const T*    Optional_Traits_Inplace_Storage<T>::StorageType::peek () const
            {
                return fValue_;
            }


            /*
             ********************************************************************************
             ********************* Optional_Traits_Inplace_Storage<T> ***********************
             ********************************************************************************
             */
            template    <typename T>
            inline  Optional_Traits_Blockallocated_Indirect_Storage<T>::StorageType::StorageType ()
            {
            }
            template    <typename T>
            inline  Optional_Traits_Blockallocated_Indirect_Storage<T>::StorageType::StorageType (AutomaticallyBlockAllocated<T>* p)
                : fValue_ { p } {
            }
            template    <typename T>
            template    <typename ...ARGS>
            inline  AutomaticallyBlockAllocated<T>*  Optional_Traits_Blockallocated_Indirect_Storage<T>::StorageType::alloc (ARGS&& ...args)
            {
                return new AutomaticallyBlockAllocated<T> (forward<ARGS> (args)...);
            }
            template    <typename T>
            inline  void    Optional_Traits_Blockallocated_Indirect_Storage<T>::StorageType::destroy ()
            {
                delete fValue_;
            }
            template    <typename T>
            inline  void    Optional_Traits_Blockallocated_Indirect_Storage<T>::StorageType::moveInitialize (StorageType&&  rhs)
            {
                // This is the ONE case where Optional_Traits_Blockallocated_Indirect_Storage can perform better than Optional_Traits_Inplace_Storage
                Require (this != &rhs);
                Require (fValue_ == nullptr);
                fValue_ = rhs.fValue_;
                rhs.fValue_ = nullptr;
                Ensure (rhs.fValue_ == nullptr);
            }
            template    <typename T>
            inline  T*  Optional_Traits_Blockallocated_Indirect_Storage<T>::StorageType::peek ()
            {
                return fValue_ == nullptr ? nullptr : fValue_->get ();
            }
            template    <typename T>
            inline  const T*    Optional_Traits_Blockallocated_Indirect_Storage<T>::StorageType::peek () const
            {
                return fValue_ == nullptr ? nullptr : fValue_->get ();
            }


            /*
             ********************************************************************************
             ********************* Optional<T, TRAITS>::ConstHolder_ ************************
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
                return fVal->peek ();
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::ConstHolder_::operator const T& () const
            {
                return *fVal->peek ();
            }
            template    <typename T, typename TRAITS>
            inline  const T& Optional<T, TRAITS>::ConstHolder_::operator* () const
            {
                return *fVal->peek ();
            }


            /*
             ********************************************************************************
             ********************* Optional<T, TRAITS>::MutableHolder_ ************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::MutableHolder_::MutableHolder_ (Optional* p)
                : fVal (p)
#if     qDebug
                , fCritSec_ { p->fDebugMutex_ }
#endif
            {
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::MutableHolder_::MutableHolder_ (MutableHolder_&& from)
                : fVal (from.fVal)
            {
                from.fVal = nullptr;
            }
            template    <typename T, typename TRAITS>
            inline  T* Optional<T, TRAITS>::MutableHolder_::operator-> ()
            {
                return fVal->peek ();
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::MutableHolder_::operator T& ()
            {
                return *fVal->peek ();
            }
            template    <typename T, typename TRAITS>
            inline  T& Optional<T, TRAITS>::MutableHolder_::operator* ()
            {
                return *fVal->peek ();
            }


            /*
             ********************************************************************************
             *************************** Optional<T, TRAITS> ********************************
             ********************************************************************************
             */
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
                fStorage_.fValue_ = fStorage_.alloc (from);
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (T&& from)
            {
                fStorage_.fValue_ = fStorage_.alloc (move (from));
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (const Optional& from)
            {
#if     qDebug
                auto fromCritSec { Execution::make_unique_lock (from.fDebugMutex_) };
#endif
                if (from.fStorage_.peek () != nullptr) {
                    fStorage_.fValue_ = fStorage_.alloc (*from.fStorage_.peek ());
                }
            }
            template    <typename T, typename TRAITS>
            template    <typename TRAITS2>
            inline  Optional<T, TRAITS>::Optional (const Optional<T, TRAITS2>& from)
            {
#if     qDebug
                auto fromCritSec { Execution::make_unique_lock (from.fDebugMutex_) };
#endif
                if (from.fStorage_.peek () != nullptr) {
                    fStorage_.fValue_ = fStorage_.alloc (*from.fStorage_.peek ());
                }
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (Optional&& from)
            {
#if     qDebug
                auto    rhsCritSec { Execution::make_unique_lock (from.fDebugMutex_) };
#endif
                if (from.fStorage_.peek () != nullptr) {
                    fStorage_.moveInitialize (move (from.fStorage_));
                    Assert (from.fStorage_.fValue_ == nullptr);
                }
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (const T* from)
            {
                if (from != nullptr) {
                    fStorage_.fValue_ = fStorage_.alloc (*from);
                }
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::~Optional ()
            {
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
                fStorage_.destroy ();
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (const T& rhs)
            {
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
                if (fStorage_.peek () == &rhs) {
                    // No need to copy in this case and would be bad to try
                    //  Optional<T> x;
                    //  x = *x;
                }
                else {
                    if (fStorage_.peek () == nullptr) {
                        fStorage_.fValue_ = fStorage_.alloc (rhs);
                    }
                    else {
                        *fStorage_.fValue_ = rhs;
                    }
                }
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (T && rhs)
            {
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
                if (fStorage_.peek () == &rhs) {
                    // No need to move in this case and would be bad to try
                    //  Optional<T> x;
                    //  x = *x;
                }
                else {
                    if (fStorage_.peek () == nullptr) {
                        fStorage_.fValue_ = fStorage_.alloc (move (rhs));
                    }
                    else {
                        *fStorage_.peek () = std::move (rhs);
                    }
                }
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (const Optional& rhs)
            {
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
                if (fStorage_.peek () != rhs.fStorage_.peek ()) {
                    clear_ ();
#if     qDebug
                    auto    rhsCritSec { Execution::make_unique_lock (rhs.fDebugMutex_) };
#endif
                    if (rhs.fStorage_.peek () != nullptr) {
                        fStorage_.fValue_ = fStorage_.alloc (*rhs.fStorage_.peek ());
                    }
                }
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (Optional && rhs)
            {
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
                if (fStorage_.peek () != rhs.fStorage_.peek ()) {
                    clear_ ();
#if     qDebug
                    auto    rhsCritSec { Execution::make_unique_lock (rhs.fDebugMutex_) };
#endif
                    if (rhs.fStorage_.peek () != nullptr) {
                        fStorage_.moveInitialize (move (rhs.fStorage_));
                        Assert (rhs.fStorage_.fValue_ == nullptr);
                    }
                }
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (const T* rhs)
            {
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
                if (fStorage_.fValue_ != rhs) {
                    if (rhs == nullptr) {
                        clear_ ();
                    }
                    else {
                        if (fStorage_.fValue_ == nullptr) {
                            fStorage_.fValue_ = fStorage_.alloc (*rhs);
                        }
                        else {
                            *fStorage_.fValue_ = *rhs;
                        }
                    }
                }
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  void    Optional<T, TRAITS>::clear ()
            {
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
                clear_ ();
                Ensure (fStorage_.peek () == nullptr);
            }
            template    <typename T, typename TRAITS>
            inline  const T*    Optional<T, TRAITS>::peek () const
            {
                // Don't bother checking fDebugMutex_ lock here since we advertise this as an unsafe API (unchecked).
                // Caller beware!
                return fStorage_.peek ();
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
                return IsMissing () ? defaultValue : *fStorage_.peek ();
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
                    return *fStorage_.peek ();
                }
            }
            template    <typename T, typename TRAITS>
            template    <typename   CONVERTABLE_TO_TYPE>
            inline  void    Optional<T, TRAITS>::AssignIf (CONVERTABLE_TO_TYPE* to) const
            {
                RequireNotNull (to);
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
                if (IsPresent ()) {
                    *to = *fStorage_.peek ();
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
#if     qOptional_SupportNonConstOperatorArrow
            template    <typename T, typename TRAITS>
            inline  auto Optional<T, TRAITS>::operator-> () -> MutableHolder_ {
                // No lock on fDebugMutex_ cuz done in MutableHolder_
                Require (IsPresent ());
                AssertNotNull (fStorage_.fValue_);
                return move (MutableHolder_ { this });
            }
#endif

            template    <typename T, typename TRAITS>
            inline  auto   Optional<T, TRAITS>::operator* () const -> T
            {
#if     qDebug
                auto    critSec { Execution::make_unique_lock (fDebugMutex_) };
#endif
                Require (IsPresent ());
                AssertNotNull (fStorage_.fValue_);
                //return ConstHolder_ { this };  when we embed mutex into holder
                Require (IsPresent ());
                AssertNotNull (fStorage_.fValue_);
                return *fStorage_.peek ();
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
            template    <typename T, typename TRAITS>
            inline  T   Optional<T, TRAITS>::value () const
            {
                return CheckedValue (domain_error ("std::bad_optional_access"));
            }
            template    <typename T, typename TRAITS>
            inline  void    Optional<T, TRAITS>::clear_ ()
            {
                fStorage_.destroy ();
                fStorage_.fValue_ = nullptr;
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
