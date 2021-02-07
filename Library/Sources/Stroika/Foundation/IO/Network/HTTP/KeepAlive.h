/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_KeepAlive_h_
#define _Stroika_Foundation_IO_Network_HTTP_KeepAlive_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Characters/String.h"

/**
 */

namespace Stroika::Foundation::IO::Network::HTTP {

	using Characters::String;

    /**
     *  https://tools.ietf.org/html/rfc2068#section-19.7.1.1
     *  https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Keep-Alive
     */
	struct KeepAlive {
        optional<unsigned int>              fMessages;
        optional<Time::DurationSecondsType> fTimeoutAt;

        /**
         */
        static KeepAlive Parse (const String& headerValue);

        static optional<KeepAlive> Merge (const optional<KeepAlive>& lhs, const optional<KeepAlive>& rhs);

        /**
         * As 'header' value
         */
        nonvirtual String AsValue () const;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "KeepAlive.inl"

#endif /*_Stroika_Foundation_IO_Network_HTTP_KeepAlive_h_*/
