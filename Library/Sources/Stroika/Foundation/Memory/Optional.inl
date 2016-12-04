/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
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
            inline  constexpr Optional_Traits_Inplace_Storage<T>::StorageType::StorageType () noexcept
                : fEmpty_ {}
            {
            }
            template    <typename T>
#if     !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
            constexpr
#endif
            inline  Optional_Traits_Inplace_Storage<T>::StorageType::StorageType (const T& src)
                : fValue_ { new (fBuffer_) T (src) }
            {
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
             ************ Optional_Traits_Blockallocated_Indirect_Storage<T> ****************
             ********************************************************************************
             */
            template    <typename T>
            inline  Optional_Traits_Blockallocated_Indirect_Storage<T>::StorageType::StorageType (const T& src)
                : fValue_ { new AutomaticallyBlockAllocated<T> (src) }
            {
            }
            template    <typename T>
            inline  Optional_Traits_Blockallocated_Indirect_Storage<T>::StorageType::~StorageType ()
            {
                destroy ();
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
                // up to caller (for efficiency reasons) to make sure fValue_ cleared out (null) if neeeded
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
            inline  Optional<T, TRAITS>::ConstHolder_::ConstHolder_ (ConstHolder_ && from)
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
            inline  Optional<T, TRAITS>::MutableHolder_::MutableHolder_ (MutableHolder_ && from)
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


#if 0
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::~Optional ()
            {
                lock_guard<_MutexBase> critSec{ *this };
                _fStorage.destroy ();
            }
#endif

            /*
             ********************************************************************************
             *************************** Optional<T, TRAITS> ********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  constexpr   Optional<T, TRAITS>::Optional (nullopt_t)
                : Optional ()
            {
            }
            template    <typename T, typename TRAITS>
            constexpr   inline  Optional<T, TRAITS>::Optional (const T& from)
                : inherited { from }
            {
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (T&& from)
            {
                _fStorage.fValue_ = _fStorage.alloc (move (from));
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (const Optional& from)
            {
                shared_lock<const _MutexBase> fromCritSec { from };
                if (from._fStorage.peek () != nullptr) {
                    _fStorage.fValue_ = _fStorage.alloc (*from._fStorage.peek ());
                }
            }
            template    <typename T, typename TRAITS>
            template    <typename T2, typename TRAITS2, typename SFINAE_SAFE_CONVERTIBLE>
            inline  Optional<T, TRAITS>::Optional (const Optional<T2, TRAITS2>& from)
            {
                shared_lock<const _MutexBase> fromCritSec { from };
                if (from._fStorage.peek () != nullptr) {
                    // explicit static cast avoided because we want to allow warning for Optional<uint64_t> aa; Optional<double> x1 = Optional<double> (aa);
                    _fStorage.fValue_ = _fStorage.alloc (*from._fStorage.peek ());
                }
            }
            template    <typename T, typename TRAITS>
            template    <typename T2, typename TRAITS2, typename SFINAE_UNSAFE_CONVERTIBLE>
            inline  Optional<T, TRAITS>::Optional (const Optional<T2, TRAITS2>& from, SFINAE_UNSAFE_CONVERTIBLE*)
            {
                shared_lock<const _MutexBase> fromCritSec { from };
                if (from._fStorage.peek () != nullptr) {
                    // static_cast<T> to silence warnings, because this overload of Optional (const Optional<T2, TRAITS2> is explicit)
                    _fStorage.fValue_ = _fStorage.alloc (static_cast<T> (*from._fStorage.peek ()));
                }
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (Optional&& from)
            {
                lock_guard<_MutexBase> fromCritSec { from };
                if (from._fStorage.peek () != nullptr) {
                    _fStorage.moveInitialize (move (from._fStorage));
                    Assert (from._fStorage.fValue_ == nullptr);
                }
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (nullopt_t)
            {
                lock_guard<_MutexBase> critSec { *this };
                clear_ ();
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (T&& rhs)
            {
                lock_guard<_MutexBase> critSec { *this };
                if (_fStorage.peek () == reinterpret_cast<const T*> (&rhs)) {
                    // No need to move in this case and would be bad to try
                    //  Optional<T> x;
                    //  x = *x;
                }
                else {
                    if (_fStorage.peek () == nullptr) {
                        _fStorage.fValue_ = _fStorage.alloc (move (rhs));
                    }
                    else {
                        *_fStorage.peek () = std::move (rhs);
                    }
                }
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (const Optional& rhs)
            {
                lock_guard<_MutexBase> critSec { *this };
                if (_fStorage.peek () != rhs._fStorage.peek ()) {
                    clear_ ();
                    lock_guard<const _MutexBase> rhsCritSec { rhs };
                    if (rhs._fStorage.peek () != nullptr) {
                        _fStorage.fValue_ = _fStorage.alloc (*rhs._fStorage.peek ());
                    }
                }
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (Optional&& rhs)
            {
                lock_guard<_MutexBase> critSec { *this };
                if (_fStorage.peek () != rhs._fStorage.peek ()) {
                    clear_ ();
                    lock_guard<_MutexBase> rhsCritSec { rhs };
                    if (rhs._fStorage.peek () != nullptr) {
                        _fStorage.moveInitialize (move (rhs._fStorage));
                        Assert (rhs._fStorage.fValue_ == nullptr);
                    }
                }
                return *this;
            }
            template    <typename T, typename TRAITS>
            template    <typename RHS_CONVERTIBLE_TO_OPTIONAL_OF_T, typename SFINAE_SAFE_CONVERTIBLE>
            inline  Optional<T, TRAITS> Optional<T, TRAITS>::OptionalFromNullable (const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T* from)
            {
                return from == nullptr ? Optional<T, TRAITS> {} :
                       Optional<T, TRAITS> { static_cast<T> (*from) };
            }
            template    <typename T, typename TRAITS>
            inline  void    Optional<T, TRAITS>::clear ()
            {
                lock_guard<_MutexBase> critSec { *this };
                clear_ ();
                Ensure (_fStorage.peek () == nullptr);
            }
            template    <typename T, typename TRAITS>
            inline  T*    Optional<T, TRAITS>::peek ()
            {
                // Don't bother checking fDebugMutex_ lock here since we advertise this as an unsafe API (unchecked).
                // Caller beware!
                return _fStorage.peek ();
            }
            template    <typename T, typename TRAITS>
            inline  const T*    Optional<T, TRAITS>::peek () const
            {
                // Don't bother checking fDebugMutex_ lock here since we advertise this as an unsafe API (unchecked).
                // Caller beware!
                return _fStorage.peek ();
            }
            template    <typename T, typename TRAITS>
            inline  constexpr   bool    Optional<T, TRAITS>::IsMissing () const noexcept
            {
                return _fStorage.fValue_ == nullptr;
            }
            template    <typename T, typename TRAITS>
            inline  constexpr   bool    Optional<T, TRAITS>::IsPresent () const noexcept
            {
                return _fStorage.fValue_ != nullptr;
            }
            template    <typename T, typename TRAITS>
            inline  constexpr   bool    Optional<T, TRAITS>::engaged () const noexcept
            {
                return _fStorage.fValue_ != nullptr;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::operator bool () const noexcept
            {
                return IsPresent ();
            }
            template    <typename T, typename TRAITS>
            inline  T Optional<T, TRAITS>::Value (T defaultValue) const
            {
                shared_lock<const _MutexBase> critSec { *this };
                return IsMissing () ? defaultValue : *_fStorage.peek ();
            }
            template    <typename T, typename TRAITS>
            template    <typename   THROW_IF_MISSING_TYPE>
            inline  T   Optional<T, TRAITS>::CheckedValue (const THROW_IF_MISSING_TYPE& exception2ThrowIfMissing) const
            {
                shared_lock<const _MutexBase> critSec { *this };
                if (IsMissing ()) {
                    Execution::Throw (exception2ThrowIfMissing);
                }
                else {
                    return *_fStorage.peek ();
                }
            }
            template    <typename T, typename TRAITS>
            template    <typename   CONVERTABLE_TO_TYPE>
            inline  void    Optional<T, TRAITS>::CopyToIf (CONVERTABLE_TO_TYPE* to) const
            {
                RequireNotNull (to);
                shared_lock<const _MutexBase> critSec { *this };
                if (IsPresent ()) {
                    // Static cast in case conversion was explicit - because call to CopyToIf() was explicit
                    DISABLE_COMPILER_MSC_WARNING_START(4244)// MSVC WARNING ABOUT conversions (see comment above about explicit)
                    *to = static_cast<CONVERTABLE_TO_TYPE> (*_fStorage.peek ());
                    DISABLE_COMPILER_MSC_WARNING_END(4244)
                }
            }
            template    <typename T, typename TRAITS>
            void     Optional<T, TRAITS>::AccumulateIf (Optional<T> rhsOptionalValue, const function<T(T, T)>& op)
            {
                lock_guard<const _MutexBase> critSec { *this };
                if (*this or rhsOptionalValue) {
                    *this = op (Value (), rhsOptionalValue.Value ());
                }
            }
            template    <typename T, typename TRAITS>
            inline  auto Optional<T, TRAITS>::operator-> () const -> ConstHolder_
            {
                // No lock on fDebugMutex_ cuz done in ConstHolder_
                Require (IsPresent ());
                AssertNotNull (_fStorage.fValue_);
                return move (ConstHolder_ { this });
            }
            template    <typename T, typename TRAITS>
            inline  auto Optional<T, TRAITS>::operator-> () -> MutableHolder_ {
                // No lock on fDebugMutex_ cuz done in MutableHolder_
                Require (IsPresent ());
                AssertNotNull (_fStorage.fValue_);
                return move (MutableHolder_ { this });
            }
            template    <typename T, typename TRAITS>
            inline  auto   Optional<T, TRAITS>::operator* () const -> T
            {
                shared_lock<const _MutexBase> critSec { *this };
                Require (IsPresent ());
                AssertNotNull (_fStorage.fValue_);
                //return ConstHolder_ { this };  when we embed mutex into holder
                Require (IsPresent ());
                AssertNotNull (_fStorage.fValue_);
                return *_fStorage.peek ();
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
                shared_lock<const _MutexBase> critSec { *this };
                if (_fStorage.fValue_ == nullptr) {
                    return rhs._fStorage.fValue_ == nullptr;
                }
                if (rhs._fStorage.fValue_ == nullptr) {
                    AssertNotNull (_fStorage.fValue_);
                    return false;
                }
                AssertNotNull (_fStorage.fValue_);
                AssertNotNull (rhs._fStorage.fValue_);
                return Common::DefaultEqualsComparer<T>::Equals (*_fStorage.fValue_, *rhs._fStorage.fValue_);
            }
            template    <typename T, typename TRAITS>
            inline  bool    Optional<T, TRAITS>::Equals (T rhs) const
            {
                shared_lock<const _MutexBase> critSec { *this };
                if (_fStorage.fValue_ == nullptr) {
                    return false;
                }
                AssertNotNull (_fStorage.fValue_);
                return Common::DefaultEqualsComparer<T>::Equals (*_fStorage.fValue_, rhs);
            }
            template    <typename T, typename TRAITS>
            inline  int Optional<T, TRAITS>::Compare (const Optional& rhs) const
            {
                shared_lock<const _MutexBase> critSec { *this };
                if (_fStorage.fValue_ == nullptr) {
                    return (rhs._fStorage.fValue_ == nullptr) ? 0 : 1; // arbitrary choice - but assume if lhs is empty thats less than any T value
                }
                if (rhs._fStorage.fValue_ == nullptr) {
                    AssertNotNull (_fStorage.fValue_);
                    return -1;
                }
                AssertNotNull (_fStorage.fValue_);
                AssertNotNull (rhs._fStorage.fValue_);
                return Common::ComparerWithWellOrder<T>::Compare (*_fStorage.fValue_, *rhs._fStorage.fValue_);
            }
            template    <typename T, typename TRAITS>
            inline  int Optional<T, TRAITS>::Compare (T rhs) const
            {
                shared_lock<const _MutexBase> critSec { *this };
                if (_fStorage.fValue_ == nullptr) {
                    return 1; // arbitrary choice - but assume if lhs is empty thats less than any T value
                }
                AssertNotNull (_fStorage.fValue_);
                return Common::ComparerWithWellOrder<T>::Compare (*_fStorage.fValue_, rhs);
            }
            template    <typename T, typename TRAITS>
            inline  T   Optional<T, TRAITS>::value () const
            {
                return CheckedValue (domain_error ("std::bad_optional_access"));
            }
            template    <typename T, typename TRAITS>
            inline  void    Optional<T, TRAITS>::clear_ ()
            {
                _fStorage.destroy ();
                _fStorage.fValue_ = nullptr;
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
                return lhs.Compare (rhs) < 0;
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
                return lhs.Compare (Optional<T, TRAITS> { rhs }) <= 0;
            }


            /*
             ********************************************************************************
             *********************************** operator== *********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  bool    operator== (const Optional<T, TRAITS>& lhs, T rhs)
            {
                return lhs.Equals (Optional<T, TRAITS> { rhs });
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
