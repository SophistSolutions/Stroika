/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_IfNoneMatch_h_
#define _Stroika_Foundation_IO_Network_HTTP_IfNoneMatch_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Characters/String.h"
#include "../../../Containers/Sequence.h"

#include "ETag.h"

/**
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    using Characters::String;

    /**
     *  If-None-Match is typically a list of just one ETag (@see https://tools.ietf.org/html/rfc7232#section-3.2)
     */
    struct IfNoneMatch {

        IfNoneMatch (const Containers::Sequence<ETag>& etags);

        /**
         *  If ill-format or missing, return nullopt, no exception.
         */
        static optional<IfNoneMatch> Parse (const String& wireFormat);

        Containers::Sequence<ETag> fETags;

        // true iff fETags is empty
        bool IsAsterisk () const;

        /**
         *  This As<> encodes the ETag as specified in HTTP SPEC (URL)
         *
         *  T can be among these:
         *      o   String
         */
        template <typename T>
        T As () const;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

#if __cpp_impl_three_way_comparison >= 201907
        /**
         */
        nonvirtual strong_ordering operator<=> (const IfNoneMatch& rhs) const = default;
#endif
    };

#if __cpp_impl_three_way_comparison < 201907
    bool operator== (const IfNoneMatch& lhs, const IfNoneMatch& rhs);
    bool operator!= (const IfNoneMatch& lhs, const IfNoneMatch& rhs);
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "IfNoneMatch.inl"

#endif /*_Stroika_Foundation_IO_Network_HTTP_IfNoneMatch_h_*/
