/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_SharedByValue_h_
#define _Stroika_Foundation_Memory_SharedByValue_h_ 1

#include "../StroikaPreComp.h"

#include <memory>

#include "../Configuration/Common.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   Get rid of SharedByValue_CopySharedPtrDefaultSynchronization, and related stuff (deprecated unused one
 *              in 2.1d6). Instead just directly to copy/load/store (using operators) - simplify code.
 *
 *      @todo   FLESH OUT HIGHLY EXPERIEMNTAL AND INCOMPLETE SHARED_IMPL_COPIER (HOPEFULLY WILL BE PART OF
 *              ENVELOPE THREAD SAFETY FIX). AND DOCUMENT!!!!
 *
 *      @todo   REDO operator== etc using non-member functions
 *              (see See coding conventions document about operator usage: Compare () and operator<, operator>, etc comments)
 *
 *      @todo   DOCUMENT (and debug if needed) the new experiemental variadic template
 *              COPY code.
 *
 *      @todo   Consider if BOTH overloads for get() should take COPY forward args - even if not needed?
 *              Especially reasonable todo now that we have cget () - which always takes zero arguments.
 *
 */

namespace Stroika::Foundation::Memory {

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
     *  \brief  SharedByValue_CopySharedPtrDefault is the default template parameter for copying SharedByValue
     *
     *  THIS IS HIGHLY EXPERIMENTAL AS OF v2.0a22 (2014-03-23) but intended to provide a useful basis for threadsafe
     *  copy-by-value (COW) envelope thread safety.
     */
    template <typename T, typename SHARED_IMLP>
    struct SharedByValue_CopySharedPtrExternallySynchronized {
        static SHARED_IMLP Load (const SHARED_IMLP& copyFrom);
        static void        Store (SHARED_IMLP* storeTo, const SHARED_IMLP& o);
    };

    /**
     */
    template <typename T, typename SHARED_IMLP>
    using SharedByValue_CopySharedPtrDefaultSynchronization = SharedByValue_CopySharedPtrExternallySynchronized<T, SHARED_IMLP>;

    /**
     *  \brief  SharedByValue_Traits is a utilitity struct to provide parameterized support
     *          for SharedByValue<>
     *
     *  This class should allow SHARED_IMLP to be std::shared_ptr (or another sharedptr implementation).
     */
    template <typename T, typename SHARED_IMLP = shared_ptr<T>, typename COPIER = SharedByValue_CopyByDefault<T, SHARED_IMLP>, typename SHARED_IMPL_COPIER = SharedByValue_CopySharedPtrDefaultSynchronization<T, SHARED_IMLP>>
    struct SharedByValue_Traits {
        using element_type            = T;
        using element_copier_type     = COPIER;
        using shared_impl_copier_type = SHARED_IMPL_COPIER;
        using shared_ptr_type         = SHARED_IMLP;
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
     *  \brief  SharedByValue is a utility class to implement Copy-On-Write (aka COW)
     *
     *  SharedByValue is a utility class to implement Copy-On-Write (aka Copy on Write, or COW).
     *
     *  This utility class should not be used lightly. Its somewhat tricky to use properly. Its meant
     *  to facilitiate implementing the copy-on-write semantics which are often handy in providing
     *  high-performance data structures.
     *
     *  This class should allow SHARED_IMLP to be std::shared_ptr (or another sharedptr implementation).
     *
     *  This class template was originally called CopyOnWrite.
     *
     *  \par Example Usage
     *      \code
     *          SharedByValue<vector<byte>> b{BLOB::Hex ("abcd1245").Repeat (100).As<vector<byte>> ()};
     *          SharedByValue<vector<byte>> c = b;  // copied by reference until 'c' or 'b' changed values
     *          VerifyTestResult (c == b);
     *      \endcode
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T, typename TRAITS = SharedByValue_Traits<T>>
    class SharedByValue {
    public:
        using element_type            = typename TRAITS::element_type;
        using element_copier_type     = typename TRAITS::element_copier_type;
        using shared_ptr_type         = typename TRAITS::shared_ptr_type;
        using shared_impl_copier_type = typename TRAITS::shared_impl_copier_type;

    public:
        static_assert (is_same_v<T, typename TRAITS::element_type>);

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
        SharedByValue () noexcept;
        SharedByValue (nullptr_t n) noexcept;
        SharedByValue (const SharedByValue& from) noexcept;
        SharedByValue (SharedByValue&& from) noexcept;
        explicit SharedByValue (const element_type& from, const element_copier_type& copier = element_copier_type ()) noexcept;
        explicit SharedByValue (const shared_ptr_type& from, const element_copier_type& copier = element_copier_type ()) noexcept;
        explicit SharedByValue (shared_ptr_type&& from, const element_copier_type&& copier = element_copier_type ()) noexcept;
        explicit SharedByValue (element_type* from, const element_copier_type& copier = element_copier_type ());

    public:
        nonvirtual SharedByValue& operator= (const SharedByValue& src);
        nonvirtual SharedByValue& operator= (SharedByValue&& src) noexcept;
        nonvirtual SharedByValue& operator= (const shared_ptr_type& from);
        nonvirtual SharedByValue& operator= (shared_ptr_type&& from);

    public:
        /**
         * get () returns the real underlying ptr we store. It can be nullptr. This should
         * rarely be used - use operator-> in preference. This is only for dealing with cases where
         * the ptr could legitimately be nil.
         *
         *  Note that the COPY_ARGS in the non-const overload of get () MUST match the parameters passed
         *  to the element_copier_type::Copy () function specified in the SharedByValue traits object.
         *
         *  This defaults to no parameters.
         */
        nonvirtual const element_type* get () const noexcept;
        template <typename... COPY_ARGS>
        nonvirtual element_type* get (COPY_ARGS&&... copyArgs);

    public:
        /**
         *  cget is equivalent to get () on a const pointer. The reason to have/use this method
         *  is just to avoid accidentally getting a reference to the non-const overload of get ().
         *
         *  \em Note: cget () will never invoke BreakReferences/Clone.
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
         */
        nonvirtual const element_type* operator-> () const;
        nonvirtual element_type* operator-> ();

    public:
        /**
         * These operators require that the underlying ptr is non-nil.
         *
         *  \em note - the non-const overloads of operator-> and operator* only work if you use a COPY function
         *              that takes no arguments (otherwise there are no arguments to pass to the clone/copy function).
         *
         *              You can always safely use the copy overload.
         */
        nonvirtual const element_type& operator* () const;
        nonvirtual element_type& operator* ();

    public:
        /**
         */
        nonvirtual bool operator== (const SharedByValue& rhs) const;

    public:
        /**
         */
        nonvirtual bool operator!= (const SharedByValue& rhs) const;

    public:
        /**
         */
        nonvirtual element_copier_type GetCopier () const;

    public:
        /**
         *  @see SharedByValue_State.
         *
         *  Note that two subsequent calls on an object CAN return different answers, without any calls to 'this' object.
         *  Thats because another shared copy can lose a reference. So - if this once returns 'shared', it might later return
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
        [[NO_UNIQUE_ADDRESS_ATTR]] element_copier_type fCopier_; // often zero sized
        shared_ptr_type                                fSharedImpl_;

    public:
        /**
         * Assure there is a single reference to this object, and if there are more, break references.
         * This method should be applied before destructive operations are applied to the shared object.
         */
        template <typename... COPY_ARGS>
        nonvirtual void Assure1Reference (COPY_ARGS&&... copyArgs);

    private:
        template <typename... COPY_ARGS>
        nonvirtual void BreakReferences_ (COPY_ARGS&&... copyArgs);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SharedByValue.inl"

#endif /*_Stroika_Foundation_Memory_SharedByValue_h_*/
