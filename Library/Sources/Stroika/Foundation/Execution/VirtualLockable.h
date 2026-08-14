/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_VirtualLockable_h_
#define _Stroika_Foundation_Execution_VirtualLockable_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <mutex>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/StdCompat.h"

/*
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::Execution {

    /**
     *  This class follows the Mutex concept and indirects to argument mutex impl.
     * 
     *  \par Example Usage
     *      \code
     *          // in class declaration where you sometimes want real locking, and somtimes just assert fake locking
     *          mutable Execution::VirtualLockable fMaybeLock_; // either Debug::AssertExternallySyncrhonized or std::recursive_mutex
     * 
     *          // in constructor, based on config parameter passed in
     *          , fMaybeLock_{options.fInternallySyncrhonized == eInternallySynchronized ? VirtualLockable::Make<recursive_mutex> ()
     *                                                                                  : VirtualLockable::Make<Debug::AssertExternallySynchronizedChecker> ()}
     *          // then use as regular mutex
     *          scoped_lock critSec{fMaybeLock_};
     *      \endcode
     * 
     *  \note Satisfies Concepts:
     *      o   movable<VirtualLockable>;
     *      o   not copyable<VirtualLockable>);
     *      o   Common::StdCompat::Lockable<VirtualLockable>;
     */
    class VirtualLockable {
    public:
        /**
         * Because we need a templated type for construction, and cannot template constructor called with explicit template arguments
         */
        template <Common::StdCompat::Lockable REAL_MUTEX>
        static VirtualLockable Make ();

    public:
        /**
         */
        VirtualLockable ()                               = delete;
        VirtualLockable (const VirtualLockable& src)     = delete;
        VirtualLockable (VirtualLockable&& src) noexcept = default;

    private:
        struct IRep_;
        VirtualLockable (unique_ptr<IRep_>&& rep);

    public:
        VirtualLockable& operator= (VirtualLockable&& rhs) noexcept = default;

    public:
        nonvirtual void lock ();
        nonvirtual bool try_lock ();
        nonvirtual void unlock ();

    private:
        struct IRep_ {
            virtual ~IRep_ ()        = default;
            virtual void lock ()     = 0;
            virtual bool try_lock () = 0;
            virtual void unlock ()   = 0;
        };
        unique_ptr<IRep_> fRep_;
    };
    static_assert (movable<VirtualLockable>); // see Satisfies Concepts
    static_assert (not copyable<VirtualLockable>);
    static_assert (Common::StdCompat::Lockable<VirtualLockable>);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "VirtualLockable.inl"

#endif /*_Stroika_Foundation_Execution_VirtualLockable_h_*/
