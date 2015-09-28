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
                , fCritSec_ { *p }
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
             ********************* Optional<T, TRAITS>::MutableHolder_ **********************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::MutableHolder_::MutableHolder_ (Optional* p)
                : fVal (p)
#if     qDebug
                , fCritSec_ { *p }
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
                lock_guard<const AssertExternallySynchronizedLock> fromCritSec { from };
                if (from.fStorage_.peek () != nullptr) {
                    fStorage_.fValue_ = fStorage_.alloc (*from.fStorage_.peek ());
                }
            }
            template    <typename T, typename TRAITS>
            template    <typename T2, typename TRAITS2, typename SFINAE_SAFE_CONVERTIBLE>
            inline  Optional<T, TRAITS>::Optional (const Optional<T2, TRAITS2>& from)
            {
                lock_guard<const AssertExternallySynchronizedLock> fromCritSec { from };
                if (from.fStorage_.peek () != nullptr) {
                    fStorage_.fValue_ = fStorage_.alloc (*from.fStorage_.peek ());
                }
            }
            template    <typename T, typename TRAITS>
            template    <typename T2, typename TRAITS2, typename SFINAE_UNSAFE_CONVERTIBLE>
            inline  Optional<T, TRAITS>::Optional (const Optional<T2, TRAITS2>& from, SFINAE_UNSAFE_CONVERTIBLE*)
            {
                lock_guard<const AssertExternallySynchronizedLock> fromCritSec { from };
                if (from.fStorage_.peek () != nullptr) {
                    // static_cast<T> to silence warnings, because this overload of Optional (const Optional<T2, TRAITS2> is explicit)
                    fStorage_.fValue_ = fStorage_.alloc (static_cast<T> (*from.fStorage_.peek ()));
                }
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (Optional&& from)
            {
                lock_guard<AssertExternallySynchronizedLock> fromCritSec { from };
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
                lock_guard<AssertExternallySynchronizedLock> critSec { *this };
                fStorage_.destroy ();
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (const T& rhs)
            {
                lock_guard<AssertExternallySynchronizedLock> critSec { *this };
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
                lock_guard<AssertExternallySynchronizedLock> critSec { *this };
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
                lock_guard<AssertExternallySynchronizedLock> critSec { *this };
                if (fStorage_.peek () != rhs.fStorage_.peek ()) {
                    clear_ ();
                    lock_guard<const AssertExternallySynchronizedLock> rhsCritSec { rhs };
                    if (rhs.fStorage_.peek () != nullptr) {
                        fStorage_.fValue_ = fStorage_.alloc (*rhs.fStorage_.peek ());
                    }
                }
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (Optional && rhs)
            {
                lock_guard<AssertExternallySynchronizedLock> critSec { *this };
                if (fStorage_.peek () != rhs.fStorage_.peek ()) {
                    clear_ ();
                    lock_guard<AssertExternallySynchronizedLock> rhsCritSec { rhs };
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
                lock_guard<AssertExternallySynchronizedLock> critSec { *this };
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
                lock_guard<AssertExternallySynchronizedLock> critSec { *this };
                clear_ ();
                Ensure (fStorage_.peek () == nullptr);
            }
            template    <typename T, typename TRAITS>
            inline  T*    Optional<T, TRAITS>::peek ()
            {
                // Don't bother checking fDebugMutex_ lock here since we advertise this as an unsafe API (unchecked).
                // Caller beware!
                return fStorage_.peek ();
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
                lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
                return IsMissing () ? defaultValue : *fStorage_.peek ();
            }
            template    <typename T, typename TRAITS>
            template    <typename   THROW_IF_MISSING_TYPE>
            inline  T   Optional<T, TRAITS>::CheckedValue (const THROW_IF_MISSING_TYPE& exception2ThrowIfMissing) const
            {
                lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
                if (IsMissing ()) {
                    Execution::DoThrow (exception2ThrowIfMissing);
                }
                else {
                    return *fStorage_.peek ();
                }
            }
            template    <typename T, typename TRAITS>
            template    <typename   CONVERTABLE_TO_TYPE>
            inline  void    Optional<T, TRAITS>::CopyToIf (CONVERTABLE_TO_TYPE* to) const
            {
                RequireNotNull (to);
                lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
                if (IsPresent ()) {
                    // Static cast in case conversion was explicit - because call to CopyToIf() was explicit
                    DISABLE_COMPILER_MSC_WARNING_START(4244)// MSVC WARNING ABOUT conversions (see comment above about explicit)
                    *to = static_cast<CONVERTABLE_TO_TYPE> (*fStorage_.peek ());
                    DISABLE_COMPILER_MSC_WARNING_END(4244)
                }
            }
            template    <typename T, typename TRAITS>
            void     Optional<T, TRAITS>::AccumulateIf (Optional<T> rhsOptionalValue, const function<T(T, T)>& op)
            {
                //logigcally do this but need recursive mutex lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
                if (*this or rhsOptionalValue) {
                    *this = op (Value (), rhsOptionalValue.Value ());
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
            inline  auto Optional<T, TRAITS>::operator-> () -> MutableHolder_ {
                // No lock on fDebugMutex_ cuz done in MutableHolder_
                Require (IsPresent ());
                AssertNotNull (fStorage_.fValue_);
                return move (MutableHolder_ { this });
            }
            template    <typename T, typename TRAITS>
            inline  auto   Optional<T, TRAITS>::operator* () const -> T
            {
                lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
                Require (IsPresent ());
                AssertNotNull (fStorage_.fValue_);
                //return ConstHolder_ { this };  when we embed mutex into holder
                Require (IsPresent ());
                AssertNotNull (fStorage_.fValue_);
                return *fStorage_.peek ();
            }
#if 0
            // cannot figure out how todo this yet...
            template    <typename T, typename TRAITS>
            template    <typename STRING_TYPE, typename ENABLE_IF>
            STRING_TYPE    Optional<T, TRAITS>::ToString () const
            {
                return IsPresent () ? Characters::ToString (*this) : L"[missing]";
            }
#endif
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&    Optional<T, TRAITS>::operator+= (const Optional& rhs)
            {
                AccumulateIf (rhs, [] (T lhs, T rhs) { return lhs + rhs; });
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&    Optional<T, TRAITS>::operator-= (const Optional& rhs)
            {
                AccumulateIf (rhs, [] (T lhs, T rhs) { return lhs - rhs; });
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&    Optional<T, TRAITS>::operator*= (const Optional& rhs)
            {
                AccumulateIf (rhs, [] (T lhs, T rhs) { return lhs * rhs; });
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&    Optional<T, TRAITS>::operator/= (const Optional& rhs)
            {
                AccumulateIf (rhs, [] (T lhs, T rhs) { return lhs / rhs; });
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Optional<T, TRAITS>::Equals (const Optional<T, TRAITS>& rhs) const
            {
                lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
                if (fStorage_.fValue_ == nullptr) {
                    return rhs.fStorage_.fValue_ == nullptr;
                }
                if (rhs.fStorage_.fValue_ == nullptr) {
                    AssertNotNull (fStorage_.fValue_);
                    return false;
                }
                AssertNotNull (fStorage_.fValue_);
                AssertNotNull (rhs.fStorage_.fValue_);
                return Common::ComparerWithEquals<T>::Equals (*fStorage_.fValue_, *rhs.fStorage_.fValue_);
            }
            template    <typename T, typename TRAITS>
            inline  bool    Optional<T, TRAITS>::Equals (T rhs) const
            {
                lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
                if (fStorage_.fValue_ == nullptr) {
                    return false;
                }
                AssertNotNull (fStorage_.fValue_);
                return Common::ComparerWithEquals<T>::Equals (*fStorage_.fValue_, rhs);
            }
            template    <typename T, typename TRAITS>
            inline  int Optional<T, TRAITS>::Compare (const Optional& rhs) const
            {
                lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
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
                lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
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


            /*
             ********************************************************************************
             ************************************ operator+ *********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>    operator+ (const Optional<T, TRAITS>& lhs, const Optional<T, TRAITS>& rhs)
            {
                Optional<T, TRAITS>  result  { lhs };
                result += rhs;
                return result;
            }


            /*
             ********************************************************************************
             ************************************ operator- *********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>    operator- (const Optional<T, TRAITS>& lhs, const Optional<T, TRAITS>& rhs)
            {
                Optional<T, TRAITS>  result  { lhs };
                result -= rhs;
                return result;
            }


            /*
             ********************************************************************************
             ************************************ operator* *********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>    operator* (const Optional<T, TRAITS>& lhs, const Optional<T, TRAITS>& rhs)
            {
                Optional<T, TRAITS>  result  { lhs };
                result *= rhs;
                return result;
            }


            /*
             ********************************************************************************
             ************************************ operator/ *********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>    operator/ (const Optional<T, TRAITS>& lhs, const Optional<T, TRAITS>& rhs)
            {
                Optional<T, TRAITS>  result  { lhs };
                result /= rhs;
                return result;
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Memory_Optional_inl_*/
