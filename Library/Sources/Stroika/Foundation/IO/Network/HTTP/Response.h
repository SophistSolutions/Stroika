/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Response_h_
#define _Stroika_Foundation_IO_Network_HTTP_Response_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Characters/CodePage.h"
#include "../../../Characters/String.h"
#include "../../../Common/Property.h"
#include "../../../Configuration/Common.h"
#include "../../../DataExchange/InternetMediaType.h"
#include "../../../Debug/AssertExternallySynchronizedLock.h"
#include "../../../IO/Network/HTTP/Headers.h"
#include "../../../IO/Network/HTTP/Status.h"
#include "../../../IO/Network/URI.h"

/*
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 *
 * TODO:
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    using Characters::String;
    using DataExchange::InternetMediaType;

    /*
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    class Response : protected Debug::AssertExternallySynchronizedLock {
    public:
        Response (const Response&) = delete;
        Response (Response&& src);
        Response ();

    public:
        ~Response () = default;

    public:
        nonvirtual Response& operator= (const Response&) = delete;

#if qDebug
    public:
        /**
         *  Allow users of the Headers object to have it share a 'assure externally synchronized' context.
         */
        nonvirtual void SetAssertExternallySynchronizedLockContext (const shared_ptr<SharedContext>& sharedContext);
#endif

    public:
        /**
         *  Allow readonly access to the Headers object. This is just checked (assertions) for re-entrancy.
         *  It can be called in any state (during transaction or after).
         * 
         * \note - this returns an INTERNAL POINTER to the Response, so be SURE to remember this with respect to
         *         thread safety, and lifetime (thread safety checked/enforced in debug builds with SetAssertExternallySynchronizedLockContext);
         */
        Common::ReadOnlyProperty<const IO::Network::HTTP::Headers&> headers;

    public:
        /**
         *  Allow reference to the headers (modify access) - but do so in the context where we assure single threading
         *  and that this is only done while the transaction is in progress.
         * 
         * It is legal to call anytime before Flush. Illegal to call after flush. 
         * It is legal to call to replace existing headers values.
         * 
         * \note - this returns an INTERNAL POINTER to the Response, so be SURE to remember this with respect to
         *         thread safety, and lifetime (thread safety checked/enforced in debug builds with SetAssertExternallySynchronizedLockContext);
         */
        Common::ExtendableProperty<IO::Network::HTTP::Headers&> rwHeaders;

    public:
        /**
         *  \brief set the status. use statusAndOverride to also specify a non-default status reason
         * 
         * The Default Status is 200 IO::Network::HTTP::StatusCodes::kOK.
         *
         * @see statusAndOverrideReason
         */
        Common::ExtendableProperty<Status> status;

    public:
        /**
         *  \brief Status value (OK), and status.item2 is an optional string override reason
         * 
         * The Default Status is 200 IO::Network::HTTP::StatusCodes::kOK.
         *
         * The overrideReason - if specified (not empty) will be used associated with the given status in the HTTP response, and otherwise one will
         * be automatically generated based on the status.
         */
        Common::ExtendableProperty<tuple<Status, optional<String>>> statusAndOverrideReason;

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

    private:
        tuple<Status, optional<String>> fStatusAndOverrideReason_;
        IO::Network::HTTP::Headers      fHeaders_;
        Characters::CodePage            fCodePage_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Response.inl"

#endif /*_Stroika_Foundation_IO_Network_HTTP_Response_h_*/
