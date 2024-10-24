/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_IfNoneMatch_h_
#define _Stroika_Foundation_IO_Network_HTTP_IfNoneMatch_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Sequence.h"

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

        /**
         */
        nonvirtual strong_ordering operator<=> (const IfNoneMatch& rhs) const = default;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "IfNoneMatch.inl"

#endif /*_Stroika_Foundation_IO_Network_HTTP_IfNoneMatch_h_*/
