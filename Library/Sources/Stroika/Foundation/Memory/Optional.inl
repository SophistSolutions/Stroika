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
            template    <typename TT>
            inline  constexpr Optional_Traits_Inplace_Storage<T>::StorageType_<TT, false>::StorageType_ () noexcept
                : fEmpty_ {}
            {
            }
            template    <typename T>
            template    <typename TT>
#if     !qCompilerAndStdLib_constexpr_functions_opNewMaybe_Buggy
            constexpr
#endif
            inline  Optional_Traits_Inplace_Storage<T>::StorageType_<TT, false>::StorageType_ (const T& src)
                : fValue_{ new (fBuffer_) T (src) }
            {
            }
            template    <typename T>
            template    <typename TT>
#if     !qCompilerAndStdLib_constexpr_functions_opNewMaybe_Buggy
            constexpr
#endif
            inline  Optional_Traits_Inplace_Storage<T>::StorageType_<TT, false>::StorageType_ (T&& src)
                : fValue_{ new (fBuffer_) T (move (src)) }
            {
            }
            template    <typename T>
            template    <typename TT>
#if     !qCompilerAndStdLib_constexpr_functions_opNewMaybe_Buggy
            constexpr
#endif
            inline  Optional_Traits_Inplace_Storage<T>::StorageType_<TT, false>::StorageType_ (const StorageType_& src)
                : fValue_{ src.fValue_ == nullptr ? nullptr : new (fBuffer_) T (*src.fValue_) }
            {
            }
            template    <typename T>
            template    <typename TT>
#if     !qCompilerAndStdLib_constexpr_functions_opNewMaybe_Buggy
            constexpr
#endif
            inline  Optional_Traits_Inplace_Storage<T>::StorageType_<TT, false>::StorageType_ (StorageType_&& src)
                : fValue_{ src.fValue_ == nullptr ? nullptr : new (fBuffer_) T (move (*src.fValue_)) }
            {
                src.fValue_ = nullptr;
            }
            template    <typename T>
            template    <typename TT>
            inline  void    Optional_Traits_Inplace_Storage<T>::StorageType_<TT, false>::destroy ()
            {
                if (fValue_ != nullptr) {
                    fValue_->~T ();
                    fValue_ = nullptr;
                }
            }
            template    <typename T>
            template    <typename TT>
            inline  auto    Optional_Traits_Inplace_Storage<T>::StorageType_<TT, false>::operator= (const T& rhs) -> StorageType_& {
                destroy ();
                fValue_ = new (fBuffer_) T (rhs);
                return *this;
            }
            template    <typename T>
            template    <typename TT>
            inline  auto    Optional_Traits_Inplace_Storage<T>::StorageType_<TT, false>::operator= (T&& rhs) -> StorageType_& {
                destroy ();
                fValue_ = new (fBuffer_) T (move (rhs));
                return *this;
            }
            template    <typename T>
            template    <typename TT>
            inline  auto    Optional_Traits_Inplace_Storage<T>::StorageType_<TT, false>::operator= (StorageType_&& rhs) -> StorageType_& {
                Require (peek () == nullptr or peek () != rhs.peek ());
                destroy ();
                if (rhs.fValue_ != nullptr)
                {
                    fValue_ = new (fBuffer_) T (move (*rhs.fValue_));
                    rhs.fValue_ = nullptr;
                }
                return *this;
            }
            template    <typename T>
            template    <typename TT>
            inline  auto    Optional_Traits_Inplace_Storage<T>::StorageType_<TT, false>::operator= (const StorageType_& rhs) -> StorageType_& {
                Require (peek () == nullptr or peek () != rhs.peek ());
                destroy ();
                if (rhs.fValue_ != nullptr)
                {
                    fValue_ = new (fBuffer_) T (*rhs.fValue_);
                }
                return *this;
            }
            template    <typename T>
            template    <typename TT>
            inline    T*  Optional_Traits_Inplace_Storage<T>::StorageType_<TT, false>::peek ()
            {
                return fValue_;
            }
            template    <typename T>
            template    <typename TT>
            inline  const T*    Optional_Traits_Inplace_Storage<T>::StorageType_<TT, false>::peek () const
            {
                return fValue_;
            }
            template    <typename T>
            template    <typename TT>
            inline  constexpr Optional_Traits_Inplace_Storage<T>::StorageType_<TT, true>::StorageType_ () noexcept
                : fEmpty_ {}
            {
            }
            template    <typename T>
            template    <typename TT>
            inline  Optional_Traits_Inplace_Storage<T>::StorageType_<TT, true>::StorageType_ (const T& src)
                : fValue_{ new (fBuffer_) T (src) }
            {
            }
            template    <typename T>
            template    <typename TT>
            inline  Optional_Traits_Inplace_Storage<T>::StorageType_<TT, true>::StorageType_ (T&& src)
                : fValue_{ new (fBuffer_) T (move (src)) }
            {
            }
            template    <typename T>
            template    <typename TT>
            inline  Optional_Traits_Inplace_Storage<T>::StorageType_<TT, true>::StorageType_ (const StorageType_& src)
                : fValue_{ src.fValue_ == nullptr ? nullptr : new (fBuffer_) T (*src.fValue_) }
            {
            }
            template    <typename T>
            template    <typename TT>
            inline  Optional_Traits_Inplace_Storage<T>::StorageType_<TT, true>::StorageType_ (StorageType_&& src)
                : fValue_{ src.fValue_ == nullptr ? nullptr : new (fBuffer_) T (move (*src.fValue_)) }
            {
                src.fValue_ = nullptr;
            }
            template    <typename T>
            template    <typename TT>
            inline  Optional_Traits_Inplace_Storage<T>::StorageType_<TT, true>::~StorageType_ ()
            {
                destroy ();
            }
            template    <typename T>
            template    <typename TT>
            inline  void    Optional_Traits_Inplace_Storage<T>::StorageType_<TT, true>::destroy ()
            {
                if (fValue_ != nullptr) {
                    fValue_->~T ();
                    fValue_ = nullptr;
                }
            }
            template    <typename T>
            template    <typename TT>
            inline  auto    Optional_Traits_Inplace_Storage<T>::StorageType_<TT, true>::operator= (const T& rhs) -> StorageType_& {
                destroy ();
                fValue_ = new (fBuffer_) T (rhs);
                return *this;
            }
            template    <typename T>
            template    <typename TT>
            inline  auto    Optional_Traits_Inplace_Storage<T>::StorageType_<TT, true>::operator= (T&& rhs) -> StorageType_& {
                destroy ();
                fValue_ = new (fBuffer_) T (move (rhs));
                return *this;
            }
            template    <typename T>
            template    <typename TT>
            inline  auto    Optional_Traits_Inplace_Storage<T>::StorageType_<TT, true>::operator= (StorageType_&& rhs) -> StorageType_& {
                Require (peek () == nullptr or peek () != rhs.peek ());
                destroy ();
                if (rhs.fValue_ != nullptr)
                {
                    fValue_ = new (fBuffer_) T (move (*rhs.fValue_));
                    rhs.fValue_ = nullptr;
                }
                return *this;
            }
            template    <typename T>
            template    <typename TT>
            inline  auto    Optional_Traits_Inplace_Storage<T>::StorageType_<TT, true>::operator= (const StorageType_& rhs) -> StorageType_& {
                Require (peek () == nullptr or peek () != rhs.peek ());
                destroy ();
                if (rhs.fValue_ != nullptr)
                {
                    fValue_ = new (fBuffer_) T (*rhs.fValue_);
                }
                return *this;
            }
            template    <typename T>
            template    <typename TT>
            inline    T*  Optional_Traits_Inplace_Storage<T>::StorageType_<TT, true>::peek ()
            {
                return fValue_;
            }
            template    <typename T>
            template    <typename TT>
            inline  const T*    Optional_Traits_Inplace_Storage<T>::StorageType_<TT, true>::peek () const
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
                : fValue_{ new AutomaticallyBlockAllocated<T> (src) }
            {
            }
            template    <typename T>
            inline  Optional_Traits_Blockallocated_Indirect_Storage<T>::StorageType::StorageType (T&& src)
                : fValue_{ new AutomaticallyBlockAllocated<T> (move (src)) }
            {
            }
            template    <typename T>
            inline  Optional_Traits_Blockallocated_Indirect_Storage<T>::StorageType::StorageType (const StorageType& src)
                : fValue_{ src.fValue_ == nullptr ? nullptr : new AutomaticallyBlockAllocated<T> (*src.fValue_) }
            {
            }
            template    <typename T>
            inline  Optional_Traits_Blockallocated_Indirect_Storage<T>::StorageType::StorageType (StorageType&& src)
                : fValue_{ src.fValue_ }
            {
                src.fValue_ = nullptr;
            }
            template    <typename T>
            inline  Optional_Traits_Blockallocated_Indirect_Storage<T>::StorageType::~StorageType ()
            {
                destroy (); // no need to reset fValue_ here
            }
            template    <typename T>
            inline  void    Optional_Traits_Blockallocated_Indirect_Storage<T>::StorageType::destroy ()
            {
                delete fValue_;
                fValue_ = nullptr;
            }
            template    <typename T>
            inline  auto    Optional_Traits_Blockallocated_Indirect_Storage<T>::StorageType::operator= (const T& rhs) -> StorageType& {
                destroy ();
                fValue_ = new AutomaticallyBlockAllocated<T> (rhs);
                return *this;
            }
            template    <typename T>
            inline  auto    Optional_Traits_Blockallocated_Indirect_Storage<T>::StorageType::operator= (T&& rhs) -> StorageType& {
                destroy ();
                fValue_ = new AutomaticallyBlockAllocated<T> (move (rhs));
                return *this;
            }
            template    <typename T>
            inline  auto    Optional_Traits_Blockallocated_Indirect_Storage<T>::StorageType::operator= (StorageType&& rhs) -> StorageType& {
                // This is the ONE case where Optional_Traits_Blockallocated_Indirect_Storage can perform better than Optional_Traits_Inplace_Storage
                Require (this != &rhs);
                destroy ();
                fValue_ = rhs.fValue_;
                rhs.fValue_ = nullptr;
                Ensure (rhs.fValue_ == nullptr);
                return *this;
            }
            template    <typename T>
            inline  auto    Optional_Traits_Blockallocated_Indirect_Storage<T>::StorageType::operator= (const StorageType& rhs) -> StorageType& {
                // This is the ONE case where Optional_Traits_Blockallocated_Indirect_Storage can perform better than Optional_Traits_Inplace_Storage
                Require (this != &rhs);
                destroy ();
                if (rhs.fValue_ != nullptr)
                {
                    fValue_ = new AutomaticallyBlockAllocated<T> (*rhs.fValue_);
                }
                return *this;
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


            /*
             ********************************************************************************
             ***** Private_::Optional_Helper_Base_<T, TRAITS, true><T, HAS_DESTRUCTOR>*******
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            constexpr   inline  Private_::Optional_Helper_Base_<T, TRAITS, false>::Optional_Helper_Base_ (const T& from)
                : _fStorage { from }
            {
            }
            template    <typename T, typename TRAITS>
            constexpr   inline  Private_::Optional_Helper_Base_<T, TRAITS, false>::Optional_Helper_Base_ (T&& from)
                : _fStorage { move (from) }
            {
            }
            template    <typename T, typename TRAITS>
            constexpr   inline  Private_::Optional_Helper_Base_<T, TRAITS, false>::Optional_Helper_Base_ (const typename TRAITS::StorageType& storage)
                : _fStorage{ storage }
            {
            }
            template    <typename T, typename TRAITS>
            constexpr   inline  Private_::Optional_Helper_Base_<T, TRAITS, false>::Optional_Helper_Base_ (typename TRAITS::StorageType&& storage)
                : _fStorage{ move (storage) }
            {
            }
            template    <typename T, typename TRAITS>
            constexpr   inline  Private_::Optional_Helper_Base_<T, TRAITS, true>::Optional_Helper_Base_ (const T& from)
                : _fStorage{ from }
            {
            }
            template    <typename T, typename TRAITS>
            constexpr   inline  Private_::Optional_Helper_Base_<T, TRAITS, true>::Optional_Helper_Base_ (T&& from)
                : _fStorage{ move (from) }
            {
            }
            template    <typename T, typename TRAITS>
            constexpr   inline  Private_::Optional_Helper_Base_<T, TRAITS, true>::Optional_Helper_Base_ (const typename TRAITS::StorageType& storage)
                : _fStorage{ storage }
            {
            }
            template    <typename T, typename TRAITS>
            constexpr   inline  Private_::Optional_Helper_Base_<T, TRAITS, true>::Optional_Helper_Base_ (typename TRAITS::StorageType&& storage)
                : _fStorage{ move (storage) }
            {
            }
            template    <typename T, typename TRAITS>
            inline  Private_::Optional_Helper_Base_<T, TRAITS, true>::~Optional_Helper_Base_ ()
            {
                lock_guard<_MutexBase> critSec{ *this };
                _fStorage.destroy ();
            }


            /*
             ********************************************************************************
             *************************** Optional<T, TRAITS> ********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  constexpr   Optional<T, TRAITS>::Optional (nullopt_t)
                : inherited {}
            {
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (const Optional& from)
                : inherited { from._fStorage }
            {
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (Optional&& from)
                : inherited{ move (from._fStorage) }        // @todo add lock_guard<_MutexBase> fromCritSec{ from }; somehow - during context of the move (not critical cuz only to debug races - not needed for correctness)
            {
                Assert (from._fStorage.fValue_ == nullptr);
            }
            template    <typename T, typename TRAITS>
            template    <typename T2, typename TRAITS2, typename SFINAE_SAFE_CONVERTIBLE>
            inline  Optional<T, TRAITS>::Optional (const Optional<T2, TRAITS2>& from)
                : inherited{ from ? (*from) : inherited{} }   // explicit static cast avoided because we want to allow warning for Optional<uint64_t> aa; Optional<double> x1 = Optional<double> (aa);
            {
            }
            template    <typename T, typename TRAITS>
            template    <typename T2, typename TRAITS2, typename SFINAE_UNSAFE_CONVERTIBLE>
            inline  Optional<T, TRAITS>::Optional (const Optional<T2, TRAITS2>& from, SFINAE_UNSAFE_CONVERTIBLE*)
                : inherited { from ? static_cast<T> (*from) : inherited{} }    // static_cast<T> to silence warnings, because this overload of Optional (const Optional<T2, TRAITS2> is explicit)
            {
            }
            template    <typename T, typename TRAITS>
            template    <typename T2, typename TRAITS2, typename SFINAE_SAFE_CONVERTIBLE>
            inline  Optional<T, TRAITS>::Optional (Optional<T2, TRAITS2>&& from)
                : inherited (from ? Optional<T, TRAITS> (move (*from)) : Optional<T, TRAITS> {})
            {
            }
            template    <typename T, typename TRAITS>
            template    <typename T2, typename TRAITS2, typename SFINAE_UNSAFE_CONVERTIBLE>
            inline  Optional<T, TRAITS>::Optional (Optional<T2, TRAITS2>&& from, SFINAE_UNSAFE_CONVERTIBLE*)
                : inherited (from ? Optional<T, TRAITS> (move (*from)) : Optional<T, TRAITS> {})
            {
            }
            template    <typename T, typename TRAITS>
            template    <typename U, typename SFINAE_SAFE_CONVERTIBLE>
            constexpr inline  Optional<T, TRAITS>::Optional (U&& from)
                : inherited (forward<U> (from))
            {
            }
            template    <typename T, typename TRAITS>
            template    <typename U, typename SFINAE_UNSAFE_CONVERTIBLE >
            constexpr inline  Optional<T, TRAITS>::Optional (U&& from, SFINAE_UNSAFE_CONVERTIBLE*)
                : inherited (forward<U> (from))
            {
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (nullopt_t)
            {
                lock_guard<_MutexBase> critSec { *this };
                this->_fStorage.destroy ();
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (const Optional& rhs)
            {
                lock_guard<_MutexBase> critSec{ *this };
                lock_guard<const _MutexBase> rhsCritSec{ rhs };
                this->_fStorage = rhs._fStorage;
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (Optional&& rhs)
            {
                lock_guard<_MutexBase> critSec{ *this };
                Require (this->_fStorage.peek () == nullptr or this->_fStorage.peek () != rhs._fStorage.peek ()); // dont allow self-move-assign - so we dont need to check
                lock_guard<_MutexBase> rhsCritSec{ rhs };
                this->_fStorage = move (rhs._fStorage);
                return *this;
            }
            template    <typename T, typename TRAITS>
            template    < typename U, typename SFINAE_SAFE_CONVERTIBLE>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (U&& rhs)
            {
                lock_guard<_MutexBase> critSec { *this };
                if (this->_fStorage.peek () == reinterpret_cast<const T*> (&rhs)) {
                    // No need to move in this case and would be bad to try
                    //  Optional<T> x;
                    //  x = *x;
                    WeakAssert (false); // dont think we want to allow this - so we dont need to check
                }
                else {
                    this->_fStorage = std::forward<U> (rhs);
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
                this->_fStorage.destroy ();
                Ensure (this->_fStorage.peek () == nullptr);
            }
            template    <typename T, typename TRAITS>
            inline  T*    Optional<T, TRAITS>::peek ()
            {
                // Don't bother checking fDebugMutex_ lock here since we advertise this as an unsafe API (unchecked).
                // Caller beware!
                return this->_fStorage.peek ();
            }
            template    <typename T, typename TRAITS>
            inline  const T*    Optional<T, TRAITS>::peek () const
            {
                // Don't bother checking fDebugMutex_ lock here since we advertise this as an unsafe API (unchecked).
                // Caller beware!
                return this->_fStorage.peek ();
            }
            template    <typename T, typename TRAITS>
            inline  constexpr   bool    Optional<T, TRAITS>::IsMissing () const noexcept
            {
                return this->_fStorage.fValue_ == nullptr;
            }
            template    <typename T, typename TRAITS>
            inline  constexpr   bool    Optional<T, TRAITS>::IsPresent () const noexcept
            {
                return this->_fStorage.fValue_ != nullptr;
            }
            template    <typename T, typename TRAITS>
            inline  constexpr   bool    Optional<T, TRAITS>::engaged () const noexcept
            {
                return this->_fStorage.fValue_ != nullptr;
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
                return IsMissing () ? defaultValue : *this->_fStorage.peek ();
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
                    return *this->_fStorage.peek ();
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
                    *to = static_cast<CONVERTABLE_TO_TYPE> (*this->_fStorage.peek ());
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
                AssertNotNull (this->_fStorage.fValue_);
                return move (ConstHolder_ { this });
            }
            template    <typename T, typename TRAITS>
            inline  auto Optional<T, TRAITS>::operator-> () -> MutableHolder_ {
                // No lock on fDebugMutex_ cuz done in MutableHolder_
                Require (IsPresent ());
                AssertNotNull (this->_fStorage.fValue_);
                return move (MutableHolder_ { this });
            }
            template    <typename T, typename TRAITS>
            inline  auto   Optional<T, TRAITS>::operator* () const -> T
            {
                shared_lock<const _MutexBase> critSec { *this };
                Require (IsPresent ());
                AssertNotNull (this->_fStorage.fValue_);
                //return ConstHolder_ { this };  when we embed mutex into holder
                Require (IsPresent ());
                AssertNotNull (this->_fStorage.fValue_);
                return *this->_fStorage.peek ();
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
                if (this->_fStorage.fValue_ == nullptr) {
                    return rhs._fStorage.fValue_ == nullptr;
                }
                if (rhs._fStorage.fValue_ == nullptr) {
                    AssertNotNull (this->_fStorage.fValue_);
                    return false;
                }
                AssertNotNull (this->_fStorage.fValue_);
                AssertNotNull (rhs._fStorage.fValue_);
                return Common::DefaultEqualsComparer<T>::Equals (*this->_fStorage.fValue_, *rhs._fStorage.fValue_);
            }
            template    <typename T, typename TRAITS>
            inline  bool    Optional<T, TRAITS>::Equals (T rhs) const
            {
                shared_lock<const _MutexBase> critSec { *this };
                if (this->_fStorage.fValue_ == nullptr) {
                    return false;
                }
                AssertNotNull (this->_fStorage.fValue_);
                return Common::DefaultEqualsComparer<T>::Equals (*this->_fStorage.fValue_, rhs);
            }
            template    <typename T, typename TRAITS>
            inline  int Optional<T, TRAITS>::Compare (const Optional& rhs) const
            {
                shared_lock<const _MutexBase> critSec { *this };
                if (this->_fStorage.fValue_ == nullptr) {
                    return (rhs._fStorage.fValue_ == nullptr) ? 0 : 1; // arbitrary choice - but assume if lhs is empty thats less than any T value
                }
                if (rhs._fStorage.fValue_ == nullptr) {
                    AssertNotNull (this->_fStorage.fValue_);
                    return -1;
                }
                AssertNotNull (this->_fStorage.fValue_);
                AssertNotNull (rhs._fStorage.fValue_);
                return Common::ComparerWithWellOrder<T>::Compare (*this->_fStorage.fValue_, *rhs._fStorage.fValue_);
            }
            template    <typename T, typename TRAITS>
            inline  int Optional<T, TRAITS>::Compare (T rhs) const
            {
                shared_lock<const _MutexBase> critSec { *this };
                if (this->_fStorage.fValue_ == nullptr) {
                    return 1; // arbitrary choice - but assume if lhs is empty thats less than any T value
                }
                AssertNotNull (this->_fStorage.fValue_);
                return Common::ComparerWithWellOrder<T>::Compare (*this->_fStorage.fValue_, rhs);
            }
            template    <typename T, typename TRAITS>
            inline  T   Optional<T, TRAITS>::value () const
            {
                return CheckedValue (domain_error ("std::bad_optional_access"));
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
