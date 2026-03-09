/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_SharedByValue_h_
#define _Stroika_Foundation_Memory_SharedByValue_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <functional>
#include <memory>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Concepts.h"
#include "Stroika/Foundation/Common/Empty.h"
#include "Stroika/Foundation/Common/StdCompat.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 * 
 *  TODO:
 *      @todo Probably should use Debug::AssertExternallySynchronized in SharedByValue
 *      @todo Understand and either remove or better document why we allow calling rwget etc with
 *            external COPIER - why not always use the same one?
 */

namespace Stroika::Foundation::Memory {

    /**
     * @brief Support types and concepts for SharedByValue template.
     */
    namespace SharedByValueSupport {

        /**
         *  \brief  DefaultValueCopier is the default template parameter for copying SharedByValue
         *
         * DefaultValueCopier is the a simple copying mechanism used by SharedByValue<>.
         * It simply hardwires use of new T() - the default T(T&) constructor to copy elements of type T.
         */
        template <typename T, typename SHARED_IMPL = shared_ptr<T>>
        SHARED_IMPL DefaultValueCopier (const T& t);

        /**
         *  \brief DefaultValueCopier_FunctionObject same as DefaultValueCopier, but as a function object
         * 
         *  \note before Stroika 3.0d23, this was the only style of copier supported.
         */
        template <typename T, typename SHARED_IMPL = shared_ptr<T>>
        struct DefaultValueCopier_FunctionObject {
#if __cplusplus >= kStrokia_Foundation_Common_cplusplus_23 || _HAS_CXX23 /*vis studio uses _HAS_CXX23 */
            static SHARED_IMPL operator() (const T& t);
#else
            SHARED_IMPL operator() (const T& t) const;
#endif
        };

        /**
         *  Sometimes we want to have NO COPIER defined - either in the traits, or the instance value.
         *  Use this sentinel value to check that case. Like optional<> for types.
         */
        using MissingCopierTypeSentinel = nullptr_t;

        /**
         * Is COPIER_TYPE a legit copier of T to SHARED_IMPL.
         */
        template <typename COPIER_TYPE, typename T, typename SHARED_IMPL>
        concept ICopier = requires (COPIER_TYPE copier, SHARED_IMPL, T t) {
            { copier (t) } -> same_as<SHARED_IMPL>;
        };

        /**
         * logically this is optional<ICopier> (@see MissingCopierTypeSentinel)
         */
        template <typename COPIER_TYPE, typename T, typename SHARED_IMPL>
        concept IOptionalCopier = same_as<remove_cvref_t<COPIER_TYPE>, MissingCopierTypeSentinel> or ICopier<COPIER_TYPE, T, SHARED_IMPL>;

        /**
         * Check if a TRAITS is a valid 'traits type' for SharedByValue:
         *      o element_type (this must be T)
         *      o shared_ptr_type (typically shared_ptr<T> or something that looks like that)
         *      o instance_defined_copier_type is ICopier or MissingCopierTypeSentinel
         *      o type of kDefaultCopier is ICopier or MissingCopierTypeSentinel
         *      o element_copier_type is ICopier (at least one must not be a sentinel value)
         * 
         * @tparam TRAITS 
         * @tparam T 
         */
        template <typename TRAITS, typename T>
        concept ITraits =
            requires (TRAITS, T t) {
                typename TRAITS::element_type;
                typename TRAITS::shared_ptr_type;
            } and same_as<typename TRAITS::shared_ptr_type::element_type, T> and Common::ICVRefTd<typename TRAITS::element_type>

            and
            requires (TRAITS) {
                typename TRAITS::default_copier_type;
                { TRAITS::kDefaultCopier } -> IOptionalCopier<T, typename TRAITS::shared_ptr_type>;
            } and Common::ICVRefTd<typename TRAITS::default_copier_type>

            and
            requires (TRAITS) {
                typename TRAITS::instance_defined_copier_type;
                { typename TRAITS::instance_defined_copier_type{} } -> IOptionalCopier<T, typename TRAITS::shared_ptr_type>;
            } and Common::ICVRefTd<typename TRAITS::instance_defined_copier_type>

            and
            requires (TRAITS, T t) {
                typename TRAITS::element_copier_type;
                { typename TRAITS::element_copier_type{} } -> ICopier<T, typename TRAITS::shared_ptr_type>;
            } and Common::ICVRefTd<typename TRAITS::element_copier_type>

            and not(same_as<typename TRAITS::instance_defined_copier_type, MissingCopierTypeSentinel> and
                    same_as<decltype (TRAITS::kDefaultCopier), MissingCopierTypeSentinel>)

            and (same_as<typename TRAITS::instance_defined_copier_type, MissingCopierTypeSentinel> or
                 same_as<decltype (TRAITS::kDefaultCopier), MissingCopierTypeSentinel> or
                 convertible_to<decltype (TRAITS::kDefaultCopier), typename TRAITS::instance_defined_copier_type>);

        /**
         *  \brief  ExplicitTraits is a utility struct to provide parameterized TRAITS support for SharedByValue<>
         *
         * @tparam T 
         * @tparam SHARED_IMPL 
         * @tparam DEFAULT_COPIER_TYPE 
         * @tparam DEFAULT_COPIER 
         * @tparam INSTANCE_COPIER_TYPE     typically nullptr_t, or occasionally function<SHARED_IMPL (const T&)>
         */
        template <typename T, typename SHARED_IMPL, IOptionalCopier<T, SHARED_IMPL> DEFAULT_COPIER_TYPE, DEFAULT_COPIER_TYPE DEFAULT_COPIER, IOptionalCopier<T, SHARED_IMPL> INSTANCE_COPIER_TYPE>
        struct ExplicitTraits {
            /**
             * @brief SharedByValue 'of T' type
             */
            using element_type = T;

            /**
             * @brief shared_ptr<T> typically, but could be another 'shared_ptr'-like class
             */
            using shared_ptr_type = SHARED_IMPL;

            /**
             * @brief satisfies IOptionalCopier, and used as default for instance copier, and if no instance copier, used to copy objects when a change occurs
             */
            using default_copier_type = DEFAULT_COPIER_TYPE;

            /**
             * @brief the default copier - which takes a 'T' and generates the appropriate SHARED_IMPL
             */
            static constexpr default_copier_type kDefaultCopier{DEFAULT_COPIER};

            /**
             * instance_defined_copier_type can be MissingCopierTypeSentinel, to indicate no user-defined (instance-defined) copy function
             * or it refers to the type of the function which converts to the appropriate shared_ptr type (typically function<SHARED_IMPL (const T&)>)
             */
            using instance_defined_copier_type = INSTANCE_COPIER_TYPE;

            /**
             * This is the type returned by GetElementCopier () - its either instance_defined_copier_type, or function<SHARED_IMPL (const T&)>
             */
            using element_copier_type =
                conditional_t<same_as<instance_defined_copier_type, MissingCopierTypeSentinel>, function<SHARED_IMPL (const T&)>, instance_defined_copier_type>;
        };

        // magic to make the templates auto-select of static vs function object work...
        template <typename T, typename SHARED_IMPL, typename COPIER_TYPE>
        constexpr auto GetDefaultCopierInstance_ ()
        {
            if constexpr (is_pointer_v<COPIER_TYPE> && is_function_v<remove_pointer_t<COPIER_TYPE>>) {
                return DefaultValueCopier<T, SHARED_IMPL>;
            }
            else {
                return COPIER_TYPE{};
            }
        }

        /**
         * @brief Unified DefaultTraits_NoInstanceCopier - works with both function objects and function pointers
         * 
         *  This is the default, and most efficient, and nearly always appropriate way to go.
         *  Automatically detects whether COPIER_TYPE is a function object or function pointer and uses appropriate default instance.
         * 
         * @tparam T 
         * @tparam SHARED_IMPL 
         * @tparam COPIER_TYPE - the type of the copier (function object type or function pointer type)
         * @tparam COPIER_INSTANCE - the instance of the copier (defaults to appropriate value based on type)
         */
        template <typename T, typename SHARED_IMPL = shared_ptr<T>, typename COPIER_TYPE = SHARED_IMPL (*) (const T&),
                  auto COPIER_INSTANCE = GetDefaultCopierInstance_<T, SHARED_IMPL, COPIER_TYPE> ()>
        using DefaultTraits_NoInstanceCopier = ExplicitTraits<T, SHARED_IMPL, COPIER_TYPE, COPIER_INSTANCE, MissingCopierTypeSentinel>;
        static_assert (ITraits<DefaultTraits_NoInstanceCopier<int, shared_ptr<int>, DefaultValueCopier_FunctionObject<int, shared_ptr<int>>>, int>);
        static_assert (ITraits<DefaultTraits_NoInstanceCopier<int, shared_ptr<int>, DefaultValueCopier_FunctionObject<int>>, int>);
        static_assert (ITraits<DefaultTraits_NoInstanceCopier<int, shared_ptr<int>, shared_ptr<int> (*) (const int&), DefaultValueCopier<int>>, int>);
        static_assert (ITraits<DefaultTraits_NoInstanceCopier<int, shared_ptr<int>>, int>);
        static_assert (ITraits<DefaultTraits_NoInstanceCopier<int>, int>);
        static_assert (ITraits<DefaultTraits_NoInstanceCopier<int, shared_ptr<int>, shared_ptr<int> (*) (const int&), DefaultValueCopier<int, shared_ptr<int>>>, int>);
        static_assert (ITraits<DefaultTraits_NoInstanceCopier<int, shared_ptr<int>, shared_ptr<int> (*) (const int&), DefaultValueCopier<int>>, int>);

        /**
         * @brief SharedByValue traits object for per-instance constructor specification of shared_ptr copier only
         * 
         *  This has no 'default' value for the copier (must always be explicitly specified in CTOR or copied from other instance through CTOR).
         * 
         * @tparam T 
         * @tparam SHARED_IMPL 
         * @tparam COPIER         
         */
        template <typename T, typename SHARED_IMPL = shared_ptr<T>, typename COPIER = function<shared_ptr<T> (const T&)>>
        using DefaultTraits_InstanceCopierOnly = ExplicitTraits<T, SHARED_IMPL, nullptr_t, nullptr_t{}, COPIER>;
        static_assert (ITraits<DefaultTraits_InstanceCopierOnly<int>, int>);

        /**
         * @brief Both a default copier, and a function<sharedimp(T)> instance copier.
         * 
         * @tparam T 
         * @tparam SHARED_IMPL 
         * @tparam DEFAULT_COPIER 
         * @tparam INSTANCE_COPIER 
         */
        template <typename T, typename SHARED_IMPL = shared_ptr<T>, typename DEFAULT_COPIER = DefaultValueCopier_FunctionObject<T, SHARED_IMPL>,
                  typename INSTANCE_COPIER = function<shared_ptr<T> (const T&)>>
        using DefaultTraits_DefaultAndInstanceCopiers = ExplicitTraits<T, SHARED_IMPL, DEFAULT_COPIER, DEFAULT_COPIER{}, INSTANCE_COPIER>;
        static_assert (ITraits<DefaultTraits_DefaultAndInstanceCopiers<int>, int>);

        /**
         *  \brief  DefaultTraits is a utility struct to provide parameterized support
         *          for SharedByValue<>
         *
         *  This class should allow SHARED_IMPL to be std::shared_ptr (or another shared_ptr implementation).
         * 
         *  \note we selected DefaultTraits_NoInstanceCopier as the default, since its the lowest overhead,
         *        and nearly always easiest to use.
         */
        template <typename T, typename SHARED_IMPL = shared_ptr<T>, typename COPIER_TYPE = SHARED_IMPL (*) (const T&),
                  auto COPIER_INSTANCE = GetDefaultCopierInstance_<T, SHARED_IMPL, COPIER_TYPE> ()>
        using DefaultTraits = DefaultTraits_NoInstanceCopier<T, SHARED_IMPL, COPIER_TYPE, COPIER_INSTANCE>;
        static_assert (ITraits<DefaultTraits<int>, int>);
        static_assert (ITraits<DefaultTraits<int, shared_ptr<int>, shared_ptr<int> (*) (const int&), DefaultValueCopier<int>>, int>);

        /**
         *   This state is meant purely for code that may manage their internal behavior
         *   based on details of reference counting - not for semantic reasons, but to enhance performance.
         */
        enum class SharingState {
            eNull,
            eSolo,
            eShared,
        };

    }

    /**
     *  \brief  SharedByValue is a utility class to implement Copy-On-Write (aka COW) - sort of halfway between unique_ptr and shared_ptr
     *
     *  SharedByValue is a utility class to implement Copy-On-Write (aka Copy on Write, or COW).
     *
     *  This utility class should not be used lightly. Its somewhat tricky to use properly. Its meant
     *  to facilitate implementing the copy-on-write semantics which are often handy in providing
     *  high-performance data structures.
     *
     *  This class should allow SHARED_IMPL to be std::shared_ptr (or another shared_ptr implementation).
     *
     *  This class template was originally called CopyOnWrite.
     * 
     *  \note Though there IS a fCopier, this is only the default copier, and calls to rwget() can always provide
     *        an alternative copier.
     *
     * 
     *  \note - though we theoretically support instance_copier_type, I don't think this has ever been tested, and is
     *        either very little used, or never used.
     *
     *  \par Example Usage
     *      \code
     *          SharedByValue<vector<byte>> b{BLOB::Hex ("abcd1245").Repeat (100).As<vector<byte>> ()};
     *          SharedByValue<vector<byte>> c = b;  // copied by reference until 'c' or 'b' changed values
     *          EXPECT_TRUE (c == b);
     *      \endcode
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     * 
     *          Understand that this works because SharedByValue objects are really shared_ptr, but with copy by value semantics.
     *          C++-Standard-Thread-Safety means that the envelope is always safe because its just following standard c++
     *          rules for copying the shared_ptr.
     * 
     *          And copying the indirected shared_ptr is always safe because the ONLY time anyone can ever MODIFY
     *          an object is if the shared_count == 1 (so no other threads using it).
     *
     *  \note   Design choice: embed fCopier into instance
     *          vs. just constructing the object on the fly the way we do for comparison functions like std::less<T> {} etc.
     * 
     *          PRO embed: If constructor cost for COPIER non-trivial, best todo once. If size can be zero, doesn't really
     *          matter/cost anything ([[no_unique_address]]/qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS). If you want to have some data in copier, and have that specific to
     *          the instance (I can think of no use case for this) - very tricky unless embedded.
     * 
     *          PRO NOT EMBED: Simpler todo access functions (default parameter instead of overload passing fCopier).
     *          For now - go with more flexible approach since not much more complex to implement.
     * 
     *  \note <a href="Design-Overview.md#Comparisons">Comparisons</a>:
     *      o   Only comparison (operator==/!=) with nullptr is supported.
     *
     *      Earlier versions of Stroika (before 2.1a5) supported operator==(SharedByValue) - and this kind of makes sense
     *      but is a little ambiguous if its measuring pointer (shared reference) equality or actual value equality.
     *
     *      Better to let the caller use operator<=> on cget() or *cget() to make clear their intentions.
     * 
     *  TODO:
     *      @todo http://stroika-bugs.sophists.com/browse/STK-798 - review docs and thread safety
     */
    template <typename T, SharedByValueSupport::ITraits<T> TRAITS = SharedByValueSupport::DefaultTraits<T>>
    class SharedByValue {
    public:
        using SharingState = SharedByValueSupport::SharingState;
        using TraitsType   = TRAITS;

    public:
        using element_type                 = typename TRAITS::element_type;
        using element_copier_type          = typename TRAITS::element_copier_type;
        using shared_ptr_type              = typename TRAITS::shared_ptr_type;
        using MissingCopierTypeSentinel    = SharedByValueSupport::MissingCopierTypeSentinel;
        using instance_defined_copier_type = typename TRAITS::instance_defined_copier_type;
        using default_copier_type          = typename TRAITS::default_copier_type;

    public:
        static_assert (same_as<T, typename TRAITS::element_type>);

    public:
        /**
         *  SharedByValue::SharedByValue():
         *  The constructor takes either no/args to nullptr, to construct an empty SharedByValue.
         *
         *  It can be copied by another copy of the same kind (including same kind of copier).
         *
         *  Or it can be explicitly constructed from a SHARED_IMPL (any existing shared_ptr, along
         *  with a copier (defaults to DefaultValueCopier). 
         *
         *  You can also copy a straight 'element_type' value into a SharedByValue.
         * 
         *  \note prior to Stroika v3.0d23 you could pass a bare pointer that
         *        pointer will be wrapped in a shared_ptr and the SharedByValue()
         *        will take ownership of the lifetime of that pointer. BUt I decided rarely useful and 
         *        easy to manually wrap in an explicit shared_ptr{}, and then the behavior provides less
         *        chance to surprise.
         */
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
        SharedByValue () noexcept
            requires (not same_as<default_copier_type, MissingCopierTypeSentinel>)
            : fSharedImpl_{}
        {
            if constexpr (not same_as<instance_defined_copier_type, MissingCopierTypeSentinel>) {
                fCopier_ = TRAITS::kDefaultCopier;
            }
        }
        SharedByValue ([[maybe_unused]] nullptr_t) noexcept
            requires (not same_as<default_copier_type, MissingCopierTypeSentinel>)
            : fSharedImpl_{}
        {
            if constexpr (not same_as<instance_defined_copier_type, MissingCopierTypeSentinel>) {
                fCopier_ = TRAITS::kDefaultCopier;
            }
        }
#else
        SharedByValue () noexcept
            requires (not same_as<default_copier_type, MissingCopierTypeSentinel>);
        SharedByValue (nullptr_t n) noexcept
            requires (not same_as<default_copier_type, MissingCopierTypeSentinel>);
#endif
        SharedByValue (SharedByValue&& from) noexcept      = default;
        SharedByValue (const SharedByValue& from) noexcept = default;
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
        SharedByValue (const shared_ptr_type& from) noexcept
            requires (not same_as<default_copier_type, MissingCopierTypeSentinel>)
            : fSharedImpl_{from}
        {
            if constexpr (not same_as<instance_defined_copier_type, MissingCopierTypeSentinel>) {
                fCopier_ = TRAITS::kDefaultCopier;
            }
        }
        SharedByValue (const element_type& from)
            requires (not same_as<default_copier_type, MissingCopierTypeSentinel>)
            : fSharedImpl_{TRAITS::kDefaultCopier (from)}
        {
            if constexpr (not same_as<instance_defined_copier_type, MissingCopierTypeSentinel>) {
                fCopier_ = TRAITS::kDefaultCopier;
            }
        }
        SharedByValue (shared_ptr_type&& from) noexcept
            requires (not same_as<default_copier_type, MissingCopierTypeSentinel>)
            : fSharedImpl_{move (from)}
        {
            if constexpr (not same_as<instance_defined_copier_type, MissingCopierTypeSentinel>) {
                fCopier_ = TRAITS::kDefaultCopier;
            }
        }
        SharedByValue (const shared_ptr_type& from, const instance_defined_copier_type& copier) noexcept
            requires (not same_as<instance_defined_copier_type, MissingCopierTypeSentinel>)
            : fCopier_{copier}
            , fSharedImpl_{from}
        {
        }
        SharedByValue (const element_type& from, const instance_defined_copier_type& copier)
            requires (not same_as<instance_defined_copier_type, MissingCopierTypeSentinel>)
            : fCopier_{copier}
            , fSharedImpl_{copier (from)}
        {
        }
        SharedByValue (shared_ptr_type&& from, const instance_defined_copier_type&& copier) noexcept
            requires (not same_as<instance_defined_copier_type, MissingCopierTypeSentinel>)
            : fCopier_{move (copier)}
            , fSharedImpl_{move (from)}
        {
        }
#else
        explicit SharedByValue (const element_type& from)
            requires (not same_as<default_copier_type, MissingCopierTypeSentinel>);
        explicit SharedByValue (const shared_ptr_type& from) noexcept
            requires (not same_as<default_copier_type, MissingCopierTypeSentinel>);
        explicit SharedByValue (shared_ptr_type&& from) noexcept
            requires (not same_as<default_copier_type, MissingCopierTypeSentinel>);
        SharedByValue (const element_type& from, const instance_defined_copier_type& copier)
            requires (not same_as<instance_defined_copier_type, MissingCopierTypeSentinel>);
        SharedByValue (const shared_ptr_type& from, const instance_defined_copier_type& copier) noexcept
            requires (not same_as<instance_defined_copier_type, MissingCopierTypeSentinel>);
        SharedByValue (shared_ptr_type&& from, const instance_defined_copier_type&& copier) noexcept
            requires (not same_as<instance_defined_copier_type, MissingCopierTypeSentinel>);
#endif

    public:
        nonvirtual SharedByValue& operator= (SharedByValue&& src) noexcept      = default;
        nonvirtual SharedByValue& operator= (const SharedByValue& src) noexcept = default;
        nonvirtual SharedByValue& operator= (shared_ptr_type&& from) noexcept;
        nonvirtual SharedByValue& operator= (const shared_ptr_type& from) noexcept;

    public:
        /**
         * @brief returns true iff sharedptr is not null
         * 
         *  \see https://en.cppreference.com/w/cpp/memory/shared_ptr/operator_bool.html
         */
        nonvirtual explicit operator bool () const noexcept;

    public:
        /**
         *  \brief access te underlying shared_ptr stored in the SharedByValue. This should be treated as readonly and
         *         only used to make calls that don't change / mutate the underlying object.
         * 
         *  \todo @todo Consider if using const somehow can help make this safer - returning a shared_ptr<const T>?? somehow
         */
        nonvirtual shared_ptr_type cget_ptr () const;

    public:
        /**
         *  \brief forced copy of the underlying shared_ptr data
         * 
         *  \note In Stroika v2.1, this was broken.
         */
        nonvirtual shared_ptr_type rwget_ptr ();
        template <typename COPIER>
        nonvirtual shared_ptr_type rwget_ptr (COPIER&& copier);

    public:
        /**
         * rwget () returns the real underlying (modifiable) ptr we store. It can be nullptr.
         * 
         * Importantly, it makes sure that there is at most one reference to the 'shared_ptr' value
         * before returning that pointer, so the caller is the only one modifying the object.
         *
         * The no-arg overload uses the builtin copier (overwhelmingly most common), but occasionally its helpful
         * to specify an alternate copier (see CONTAINER::_GetWritableRepAndPatchAssociatedIterator for example).
         */
        nonvirtual element_type* rwget ();
        template <typename COPIER>
        nonvirtual element_type* rwget (COPIER&& copier);

    public:
        /**
         *  cget returns returns the real underlying const ptr we store.
         *
         *  \em Note: cget () will never invoke BreakReferences/Clone.
         * 
         *  To get a non-const pointer, @see rwget ()
         */
        nonvirtual const element_type* cget () const noexcept;

    public:
        /**
         * These operators require that the underlying ptr is non-nil.
         *
         *  \em note - the non-const overloads of operator-> and operator* only work if you use a COPY function
         *              that takes no arguments (otherwise there are no arguments to pass to the clone/copy function).
         *
         *              You can always safely use the copy overload.
         * 
         *  \note This can be confusing, because at the point of call, its unclear if this may invoke BreakReferences or not
         */
        nonvirtual const element_type* operator->() const;
        nonvirtual element_type*       operator->();

    public:
        /**
         * These operators require that the underlying ptr is non-null.
         */
        nonvirtual const element_type& operator* () const;

    public:
        /**
         */
        constexpr bool operator== (nullptr_t) const;

    public:
        /**
         */
        nonvirtual element_copier_type GetElementCopier () const;

    public:
        /**
         *  @see SharingState.
         *
         *  Note that two subsequent calls on an object CAN return different answers, without any calls to 'this' object.
         *  That's because another shared copy can lose a reference. So - if this once returns 'shared', it might later return
         *  solo, without any change to THIS object.
         */
        nonvirtual SharingState GetSharingState () const;

    public:
        /**
         * Returns true if there is exactly one object referenced. Note that if empty () - then not unique().
         */
        nonvirtual bool unique () const;

    public:
        /**
         *  Returns the number of references to the underlying shared pointer.
         *
         *  @see SharingState
         */
        nonvirtual unsigned int use_count () const;

    private:
        using DeclaredInstanceCopierType_ =
            conditional_t<same_as<instance_defined_copier_type, MissingCopierTypeSentinel>, Common::Empty, instance_defined_copier_type>;
        shared_ptr_type                                                          fSharedImpl_;
        qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE DeclaredInstanceCopierType_ fCopier_; // often zero sized

    public:
        /**
         * Assure there are at most N (typically one or 2) references to this object, and if there are more, break references.
         * This method should be applied before destructive operations are applied to the shared object.
         * 
         * Argument copier is typically fCopier_
         * 
         * \note - QUEER - copies generated by BreakReferences_() use the original fCopier_, not the argument copier.
         */
        template <typename COPIER>
        nonvirtual void AssureNOrFewerReferences (COPIER&& copier, unsigned int n = 1u);
        nonvirtual void AssureNOrFewerReferences (unsigned int n = 1u);

    private:
        template <typename COPIER>
        nonvirtual void BreakReferences_ (COPIER&& copier);
    };
    // NOT strictly gauranteed by C++, but we want to be warned if this ever fails, and correct or if we must
    // conditionalize the test
    //static_assert (same_as<SharedByValue<int>::TraitsType, SharedByValueSupport::DefaultTraits_NoInstanceCopier<int>>); // if this fails, next one is meaningless
    static_assert (sizeof (SharedByValue<int>) == sizeof (shared_ptr<int>)); // no space overhead for copier (by default)

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SharedByValue.inl"

#endif /*_Stroika_Foundation_Memory_SharedByValue_h_*/
