/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_UniqueID_h_
#define _Stroika_Foundation_Common_UniqueID_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <atomic>
#include <compare>
#include <cstdint>

#include "Stroika/Foundation/Common/Common.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Characters {
    class String;
}

namespace Stroika::Foundation::Common {

    namespace Private_ {
        inline atomic<uint64_t> sUniqueIDLast_{0};
    }

    /**
     *  \brief An opaque ID - each New () one never given before, in this process - of a type of its own for each TAG, so one
     *         kind cannot be passed for another. UniqueID{} names nothing.
     *
     *  For naming what was added - a callback, a timer, a task - so it can be removed, or asked about, later, without comparing
     *  the things themselves (@see Execution::CallbackRegistry for why). One made up (UniqueID{}, or another's) names nothing
     *  added, so New () need not be guarded.
     *
     *  \par Example Usage
     *      \code
     *          class Thing {
     *          public:
     *              using ID = Common::UniqueID<Thing>;
     *              ID   Add (...);         // ID id = ID::New (); ...; return id;
     *              void Remove (ID id);
     *          };
     *      \endcode
     */
    template <typename TAG>
    class UniqueID {
    public:
        constexpr UniqueID () = default;

    public:
        /**
         *  \brief One never given before in this process - by New () of any TAG.
         */
        static UniqueID New ();

    public:
        constexpr bool            operator== (const UniqueID& rhs) const  = default;
        constexpr strong_ordering operator<=> (const UniqueID& rhs) const = default;

    public:
        /**
         *  @see Characters::ToString ()
         */
        nonvirtual Characters::String ToString () const;

    private:
        constexpr explicit UniqueID (uint64_t id);

    private:
        uint64_t fID_{0}; // 0: none
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "UniqueID.inl"

#endif /*_Stroika_Foundation_Common_UniqueID_h_*/
