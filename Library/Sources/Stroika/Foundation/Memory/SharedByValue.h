/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_SharedByValue_h_
#define _Stroika_Foundation_Memory_SharedByValue_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <memory>

#include "Stroika/Foundation/Configuration/Common.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 * 
 *  TODO:
 *      @todo Probably should use Debug::AssertExternallySyncrhonized in SharedByValue
 *
 */

namespace Stroika::Foundation::Memory {

    // @todo redo as C++ concept in C++20
    template <typename T, typename SHARED_IMLP, typename COPIER>
    constexpr bool SharedByValue_IsCopier ()
    {
        // @todo should match API function<SHARED_IMLP(const T&)>
        return true;
    }

    /**
     *  \brief  SharedByValue_CopyByDefault is the default template parameter for copying SharedByValue
     *
     * SharedByValue_CopyByDefault is the a simple copying mechanism used by SharedByValue<>.
     * It simply hardwires use of new T() - the default T(T&) constructor to copy elements of type T.
     */
    template <typename T, typename SHARED_IMLP = shared_ptr<T>>
    struct SharedByValue_CopyByDefault {
        nonvirtual SHARED_IMLP operator() (const T& t) const;
    };

    /**
     *  \brief  SharedByValue_Traits is a utility struct to provide parameterized support
     *          for SharedByValue<>
     *
     *  This class should allow SHARED_IMLP to be std::shared_ptr (or another shared_ptr implementation).
     */
    template <typename T, typename SHARED_IMLP = shared_ptr<T>, typename COPIER = SharedByValue_CopyByDefault<T, SHARED_IMLP>>
    struct SharedByValue_Traits {
        using element_type = T;

        /**
         *  Note that the COPIER can ASSERT externally synchronized, and doesnt need to synchronize itself.
         */
        using element_copier_type = COPIER;
        using shared_ptr_type     = SHARED_IMLP;
    };

    /**
     *   This state is meant purely for code that may manage their internal behavior
     *   based on details of reference counting - not for semantic reasons, but to enhance performance.
     */
    enum class SharedByValue_State {
        eNull,
        eSolo,
        eShared,
    };

    /**
     *  \brief  SharedByValue is a utility class to implement Copy-On-Write (aka COW) - sort of halfway between unique_ptr and shared_ptr
     *
     *  SharedByValue is a utility class to implement Copy-On-Write (aka Copy on Write, or COW).
     *
     *  This utility class should not be used lightly. Its somewhat tricky to use properly. Its meant
     *  to facilitate implementing the copy-on-write semantics which are often handy in providing
     *  high-performance data structures.
     *
     *  This class should allow SHARED_IMLP to be std::shared_ptr (or another shared_ptr implementation).
     *
     *  This class template was originally called CopyOnWrite.
     * 
     *  \note Though there IS a fCopier, this is only the default copier, and calls to rwget() can always provide
     *        an alternative copier.
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
     *          matter/cost anything ([[no_unique_address]]). If you want to have some data in copier, and have that specific to
     *          the instance (I can think of no use case for this) - very tricky unless embedded.
     * 
     *          PRO NOT EMBED: Simpler todo access functions (default parameter instead of overload passing fCopier).
     *          For now - go with more flexible approach since not much more complex to implement.
     * 
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
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
    template <typename T, typename TRAITS = SharedByValue_Traits<T>>
    class SharedByValue {
    public:
        using element_type        = typename TRAITS::element_type;
        using element_copier_type = typename TRAITS::element_copier_type;
        using shared_ptr_type     = typename TRAITS::shared_ptr_type;

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
         *  with a copier (defaults to SharedByValue_CopyByDefault). If passed a bare pointer, that
         *  pointer will be wrapped in a shared_ptr (so it better not be already), and the SharedByValue()
         *  will take ownership of the lifetime of that pointer.
         *
         *  You can also copy a straight 'element_type' value into a SharedByValue.
         */
        SharedByValue () noexcept = default;
        SharedByValue (nullptr_t n) noexcept;
        SharedByValue (SharedByValue&& from) noexcept      = default;
        SharedByValue (const SharedByValue& from) noexcept = default;
        explicit SharedByValue (const element_type& from, const element_copier_type& copier = element_copier_type{}) noexcept;
        explicit SharedByValue (const shared_ptr_type& from, const element_copier_type& copier = element_copier_type{}) noexcept;
        explicit SharedByValue (shared_ptr_type&& from, const element_copier_type&& copier = element_copier_type{}) noexcept;
        explicit SharedByValue (element_type* from, const element_copier_type& copier = element_copier_type{});

    public:
        nonvirtual SharedByValue& operator= (SharedByValue&& src) noexcept      = default;
        nonvirtual SharedByValue& operator= (const SharedByValue& src) noexcept = default;
        nonvirtual SharedByValue& operator= (shared_ptr_type&& from) noexcept;
        nonvirtual SharedByValue& operator= (const shared_ptr_type& from) noexcept;

    public:
        /**
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
         * These operators require that the underlying ptr is non-nil.
         */
        nonvirtual const element_type& operator* () const;

    public:
        /**
         */
        constexpr bool operator== (nullptr_t) const;

    public:
        /**
         */
        nonvirtual element_copier_type GetDefaultCopier () const;

    public:
        /**
         *  @see SharedByValue_State.
         *
         *  Note that two subsequent calls on an object CAN return different answers, without any calls to 'this' object.
         *  That's because another shared copy can lose a reference. So - if this once returns 'shared', it might later return
         *  solo, without any change to THIS object.
         */
        nonvirtual SharedByValue_State GetSharingState () const;

    public:
        /**
         * Returns true if there is exactly one object referenced. Note that if empty () - then not unique().
         */
        nonvirtual bool unique () const;

    public:
        /**
         *  Returns the number of references to the underlying shared pointer.
         *
         *  @see SharedByValue_State
         */
        nonvirtual unsigned int use_count () const;

    private:
        [[no_unique_address]] element_copier_type fCopier_; // often zero sized
        shared_ptr_type                           fSharedImpl_;

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

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SharedByValue.inl"

#endif /*_Stroika_Foundation_Memory_SharedByValue_h_*/
