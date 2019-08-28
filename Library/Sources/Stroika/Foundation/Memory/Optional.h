/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Optional_h_
#define _Stroika_Foundation_Memory_Optional_h_ 1

#include "../StroikaPreComp.h"

#include <mutex>
#include <optional>
#include <shared_mutex>

#include "../Common/Compare.h"
#include "../Configuration/Common.h"
#include "../Configuration/Empty.h"
#include "../Containers/Adapters/Adder.h"
#include "../Debug/AssertExternallySynchronizedLock.h"
#include "../Execution/NullMutex.h"
#include "BlockAllocated.h"
#include "Common.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *
 *      NOTE TO SUGGEST TO C++ standards - 
 *          Things I miss most about my Optional versus std::optional
 *              >   Value () - what they call value_or - should take T{} as default argument. About 25% of teh time
 *                  thats what I want, and its much more clear/terse.
 *
 *              >   Accumulate method, and operator +=, operator-= etc overloads calling Accumulate(). Much simpler
 *                  and more elegant code with those methods.
 *
 *      @todo   BIG ITEM - MOSTLY get rid of this. Maybe not totally. But std::optional is pretty good, and sensible
 *              to use in Stroika. If I can find a way to extend it compatibly (say by subclassing and object slicing)
 *              then perhaps keep using this class, but change all my API calls to reference std::optional.
 *
 *
 *      @todo   Consider if we should maintain thread unsfafe peek() method.
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-456 opertor= cleanups (typename U U&&;
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-557 - Use (lock_guard<MutexBase_> { from },..) in mem-initializers (constexpr functions problem)
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-553 better integrate Optional with std::optional - so easy to go back and forth
 *
 *      @todo   See if I can get operator* working with ConstHolder_ (maybe more efficient). Or could return const&
 *              in release builds and T in DEBUG builds (so we can do context based debug lock/check).
 *
 *      @todo   FIX operator<, etc to match what we did for operator== and operator!=, and document!!!
 *              since COMPARE is part of traits we do NOT want to allow compare with different traits (so MUST FIX EUQalas as well)
 *
 *      @todo   COULD make operator==, operator<, etc compares more efficeint when comparing with T
 *              by adding a slew (3x) more overloads. (DID TO SOME EXTENT BUT COULD IMPROVE); And could improve iterop
 *              by adding operator XXX (where XXX is ==, etc) with (T, OPTIONAL);
 */

namespace Stroika::Foundation::Characters {
    class String;
    template <typename T>
    String ToString (const T&);
}

namespace Stroika::Foundation::Memory {

    using std::byte;

    /**
     *  This storage style is the default, and is usually fastest. However, it requires the
     *  sizeof (T) be know at the time Optional<T> is used (so not forward declared).
     *
     *  \note   This is NOT meant to be used by itself. This is only to be used as a template argument to Optional<>
     *
     *  \note   \em Implementation Detail
     *              We put fValue_ ahead of fBuffer_ because the data is nearly always accessed through
     *              fValue_ and putting it first probably makes its offset zero which may make for a faster
     *              reference (deref without offset)
     */
    template <typename T>
    struct Optional_Traits_Inplace_Storage {
        static constexpr bool kIncludeDebugExternalSync = qDebug and not is_literal_type<T>::value;

        /*
         *  To make Optional<> work with constexpr construction (by value) - we may need eliminate the Optional
         *  destructor (to satisfy the rules for LiteralType in C++14). But there doesnt appear to be a direct
         *  SFINAE way to conditionally provide a destructor, besides providing alternates for the base class (or member classes)
         *  and selecting whole base (or member) clases. Thats what this StorageType_<HAS_DESTRUCTOR> does.
         */
        template <typename TT, bool HAS_DESTRUCTOR>
        struct StorageType_;
        template <typename TT>
        struct StorageType_<TT, false> {
        private:
            /*
             *  Use union - so we can initialize CTOR for constexpr right part of union, and have no DTOR.
             *  Use fEngaged bool instead of pointer - because C++ (at least g++/clang++) don't allow pointer to
             *  constexpr value, where fEngagedValue_ not clearly marked constexpr (maybe no matter what).
             */
            union {
                Configuration::Empty fEmpty_;
                T                    fEngagedValue_;
            };
            bool fEngaged_{false};

        public:
            constexpr StorageType_ () noexcept;
            constexpr StorageType_ (const T& src);
            constexpr StorageType_ (T&& src);
            constexpr StorageType_ (const StorageType_& src);
            constexpr StorageType_ (StorageType_&& src);

            nonvirtual StorageType_& operator= (const T& rhs);
            nonvirtual StorageType_& operator= (T&& rhs);
            nonvirtual StorageType_& operator= (const StorageType_& rhs);
            nonvirtual StorageType_& operator= (StorageType_&& rhs);

            nonvirtual void destroy ();
            nonvirtual T*                 peek ();
            nonvirtual constexpr const T* peek () const;

        private:
            template <typename ARGT, typename USE_T = T, enable_if_t<is_copy_assignable_v<USE_T>>* = nullptr>
            nonvirtual void Assign_ (ARGT&& arg);
            template <typename ARGT, typename USE_T = T, enable_if_t<not is_copy_assignable_v<USE_T>>* = nullptr>
            nonvirtual void Assign_ (ARGT&& arg /*, const T_IS_NOT_ASSIGNABLE* = nullptr*/);
        };
        template <typename TT>
        struct StorageType_<TT, true> {
        private:
            T* fValue_{nullptr};
            DISABLE_COMPILER_MSC_WARNING_START (4324) // structure was padded due to alignment specifier
            union {
                Configuration::Empty fEmpty_;

#if qCompilerAndStdLib_alignas_Sometimes_Mysteriously_Buggy
                // VERY weirdly - alignas(alignment_of<T>)   - though WRONG (needs ::value - and that uses alignas) works
#else
                alignas (T)
#endif
                byte fBuffer_[sizeof (T)]; // intentionally uninitialized
            };
            DISABLE_COMPILER_MSC_WARNING_END (4324)

        public:
            constexpr StorageType_ () noexcept;
            StorageType_ (const T& src);
            StorageType_ (T&& src);
            StorageType_ (const StorageType_& src);
            StorageType_ (StorageType_&& src);
            ~StorageType_ ();

            nonvirtual StorageType_& operator= (const T& rhs);
            nonvirtual StorageType_& operator= (T&& rhs);
            nonvirtual StorageType_& operator= (const StorageType_& rhs);
            nonvirtual StorageType_& operator= (StorageType_&& rhs);

            nonvirtual void destroy ();
            nonvirtual T*    peek ();
            nonvirtual const T* peek () const;

        private:
            template <typename ARGT, typename USE_T = T, enable_if_t<is_copy_assignable_v<USE_T>>* = nullptr>
            nonvirtual void Assign_ (ARGT&& arg);
            template <typename ARGT, typename USE_T = T, enable_if_t<not is_copy_assignable_v<USE_T>>* = nullptr>
            nonvirtual void Assign_ (ARGT&& arg /*, const T_IS_NOT_ASSIGNABLE* = nullptr*/);
        };
        using StorageType = StorageType_<T, not is_trivially_destructible<T>::value>;
    };

    /**
     *  This storage is usually somewhat slower than Optional_Traits_Inplace_Storage (except
     *  if doing lots of moves).
     *
     *  Its main advantage is that it doesn't require the sizeof (T) be know at the time Optional<T>
     *  is used (so T can be forward declared).
     *
     *  \note   This is NOT meant to be used by itself. This is only to be used as a template argument to Optional<>
     */
    template <typename T>
    struct Optional_Traits_Blockallocated_Indirect_Storage {
        static constexpr bool kIncludeDebugExternalSync = qDebug;
        struct StorageType {
        private:
            T* fValue_{nullptr};

        public:
            constexpr StorageType () = default;
            StorageType (const T&);
            StorageType (T&& src);
            StorageType (const StorageType& src);
            StorageType (StorageType&& src) noexcept;
            ~StorageType ();

            nonvirtual StorageType& operator= (const T& rhs);
            nonvirtual StorageType& operator= (T&& rhs);
            nonvirtual StorageType& operator= (const StorageType& rhs);
            nonvirtual StorageType& operator= (StorageType&& rhs) noexcept;

            nonvirtual void destroy ();
            nonvirtual T*    peek ();
            nonvirtual const T* peek () const;
        };
    };

    /**
     *  By default, Optional (and Optional_Traits_Default) use  Optional_Traits_Inplace_Storage
     *  since its probably generally more efficient, and more closely aligned with
     *  std::optional<> behavior. Also - only Optional_Traits_Inplace_Storage works with constexpr.
     */
    template <typename T>
    using Optional_Traits_Default = Optional_Traits_Inplace_Storage<T>;

    template <typename T, typename TRAITS>
    class Optional;

    namespace Private_ {

        template <typename, typename>
        struct IsOptional_impl_ : false_type {
        };
        template <typename T, typename TRAITS>
        struct IsOptional_impl_<Optional<T, TRAITS>, TRAITS> : true_type {
        };

        template <typename T, typename TRAITS>
        struct IsOptional_ : public IsOptional_impl_<std::remove_cv_t<std::remove_reference_t<T>>, TRAITS> {
        };
    }

    /**
     *  EXPERIEMNTAL - 2015-04-24
     *  \brief  AccumulateIf () add in the rhs argument value to lhs optional, but if both were missing leave 'lhs'
     *          as still missing, and if only RHS available, assign it to the left.
     *
     *  \par Example Usage
     *      \code
     *          optional<int>   accumulator;
     *          optional<int>   SomeFunctionToGetOptionalValue();
     *          if (accumulator or (tmp = SomeFunctionToGetOptionalValue())) {
     *              accumulator = accumulator.Value () + tmp;
     *          }
     *      \endcode
     *      VERSUS
     *      \code
     *          accumulator.AccumulateIf (SomeFunctionToGetOptionalValue ());
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          optional<Sequence<InternetAddress>>>   addresses;
     *          Memory::AccumulateIf (&addresses, IO::Network::InternetAddress{connection.GET ().GetDataTextInputStream ().ReadAll ().Trim ()});
     *      \endcode
     *
     *
     *      \note   ITS CONFUSING direction of if-test for this versus CopyToIf
     *
     *      \note
     *          typical OP arguments would be:
     *              std::plus{} **the default**
     *              std::minus{}
     *              std::multiplies{}
     *              std::divides{}
     */
    template <typename T, typename CONVERTIBLE_TO_T, typename OP = plus<T>, enable_if_t<is_convertible_v<CONVERTIBLE_TO_T, T> and is_convertible_v<OP, function<T (T, T)>>>* = nullptr>
    void AccumulateIf (optional<T>* lhsOptionalValue, const optional<CONVERTIBLE_TO_T>& rhsOptionalValue, const OP& op = OP{});
    template <typename T, typename OP = plus<T>, enable_if_t<is_convertible_v<OP, function<T (T, T)>>>* = nullptr>
    void AccumulateIf (optional<T>* lhsOptionalValue, const T& rhsValue, const OP& op = OP{});
    template <typename T, template <typename> typename CONTAINER, enable_if_t<is_convertible_v<typename Containers::Adapters::Adder<CONTAINER<T>>::value_type, T>>* = nullptr>
    void AccumulateIf (optional<CONTAINER<T>>* lhsOptionalValue, const optional<T>& rhsOptionalValue);
    template <typename T, template <typename> typename CONTAINER, enable_if_t<is_convertible_v<typename Containers::Adapters::Adder<CONTAINER<T>>::value_type, T>>* = nullptr>
    void AccumulateIf (optional<CONTAINER<T>>* lhsOptionalValue, const T& rhsValue);

    /**
     *  Assign the value held by this optional if one is present to destination argument (pointer). Assigns from left to right.
     *
     *  The point of this to to faciltate a common idiom, where you want to maintain an existing value unless you
     *  get an update. This function is ANALAGOUS to
     *      if (o.has_value()) {
     *          destArgVal = *o;
     *      }
     *
     *  but can be done in a single line.
     *
     *  \par Example Usage
     *      \code
     *          int curValue = 3;
     *          optional<long>  oVal = someMap.Lookup (KEY_VALUE);
     *          oVal.CopyToIf (&curValue);
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          optional<int> curValue;
     *          optional<long>  oVal = someMap.Lookup (KEY_VALUE);
     *          oVal.CopyToIf (&curValue);      // curValue retains its value from before CopyToIf if oVal was missing
     *      \endcode
     *
     *  @see Value
     *
     *  \note - the overload with the second arg optional<CONVERTABLE_TO_OPTIONAL_OF_TYPE> is just to silence an msvc compiler warning.
     */
    template <typename T, typename CONVERTABLE_TO_TYPE>
    void CopyToIf (const optional<T>& copyFromIfHasValue, CONVERTABLE_TO_TYPE* to);
    template <typename T, typename CONVERTABLE_TO_OPTIONAL_OF_TYPE>
    void CopyToIf (const optional<T>& copyFromIfHasValue, optional<CONVERTABLE_TO_OPTIONAL_OF_TYPE>* to);

    /**
     *  Always safe to call. If has_value returns it, else returns argument 'default'. Like value_or () but has default value for default value.
     *
     *  \note This is handy because there is no default argument for std::optional<>::value_or () - there should be (like this).
     */
    template <typename T>
    T ValueOrDefault (const optional<T>& o, T defaultValue = T{});

    /**
     * \brief return one of *this, or o, with first preference for which is engaged, and second preference for left-to-right.
     *
     *  So Equivilent to this->has_value ()? *this : o;
     *
     *  This is VERY similar to ValueOrDefault () - except that the default maybe optional, and this therefore returns an Optional<T>
     *
     *  @see Value ()
     */
    template <typename T>
    optional<T> OptionalValue (const optional<T>& l, const optional<T>& r);

    /**
         *  'Constructor' taking const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T* is to allow easier interoperability
         *  with code that uses null-pointers to mean 'is-missing': nullptr means missing, and if non null,
         *  derefrence and copy.
         *
         *  \par Example Usage
         *      \code
         *      float*  d1  =   nullptr;
         *      double* d2  =   nullptr;
         *      Assert (not Optional<double>::OptionalFromNullable (d1).has_value ());
         *      Assert (not Optional<double>::OptionalFromNullable (d2).has_value ());
         *      \endcode
         *
         *  \note   I tried making this an Optional<T> constructor overload, but it lead to dangerous confusion with things like
         *          URL url = URL (L"dyn:/StyleSheet.css?ThemeName=Cupertino", URL::eStroikaPre20a50BackCompatMode);
         *          VerifyTestResult (url.GetScheme () == L"dyn");
         *          // wchar_t* overload is optional gets STRING with value "d";
         */
    template <typename RHS_CONVERTIBLE_TO_OPTIONAL_OF_T, typename T = RHS_CONVERTIBLE_TO_OPTIONAL_OF_T, typename SFINAE_SAFE_CONVERTIBLE = enable_if_t<Configuration::is_explicitly_convertible<RHS_CONVERTIBLE_TO_OPTIONAL_OF_T, T>::value>>
    optional<T> OptionalFromNullable (const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T* from);

    /**
     *  Simple overloaded operator which calls @Optional<T>::operator+= (const Optional<T>& rhs)
     *
     *  \note this uses AccumulateIf(), so if both sides missing, result is missing, but if only one side is
     *        empty, the other side defaults to T{}.
     */
    template <typename T>
    optional<T> operator+ (const optional<T>& lhs, const optional<T>& rhs);

    /**
     *  Simple overloaded operator which calls @Optional<T>::operator-= (const Optional<T>& rhs)
     *
     *  \note this uses AccumulateIf(), so if both sides missing, result is missing, but if only one side is
     *        empty, the other side defaults to T{}.
     */
    template <typename T>
    optional<T> operator- (const optional<T>& lhs, const optional<T>& rhs);

    /**
     *  Simple overloaded operator which calls @Optional<T>::operator*= (const Optional<T>& rhs)
     *
     *  \note this uses AccumulateIf(), so if both sides missing, result is missing, but if only one side is
     *        empty, the other side defaults to T{}.
     */
    template <typename T>
    optional<T> operator* (const optional<T>& lhs, const optional<T>& rhs);

    /**
     *  Simple overloaded operator which calls @Optional<T>::operator/= (const Optional<T>& rhs)
     *
     *  \note this uses AccumulateIf(), so if both sides missing, result is missing, but if only one side is
     *        empty, the other side defaults to T{}.
     */
    template <typename T>
    optional<T> operator/ (const optional<T>& lhs, const optional<T>& rhs);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Optional.inl"

#endif /*_Stroika_Foundation_Memory_Optional_h_*/
