/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Response_h_
#define _Stroika_Foundation_IO_Network_HTTP_Response_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/DataExchange/InternetMediaType.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include "Stroika/Foundation/IO/Network/HTTP/Status.h"
#include "Stroika/Foundation/IO/Network/URI.h"

/*
 *  \note Code-Status:  <a href="Code-Status.md#Late-Alpha">Late-Alpha</a>
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    using Characters::String;
    using DataExchange::InternetMediaType;

    /*
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    class Response {
    public:
        Response (const Response&) = delete;
        Response (Response&& src);
        Response (const optional<Headers>& initialHeaders = {});

    public:
        ~Response () = default;

    public:
        nonvirtual Response& operator= (const Response&) = delete;

#if qStroika_Foundation_Debug_AssertExternallySynchronizedMutex_Enabled
    public:
        /**
         *  Allow users of the Headers object to have it share a 'assure externally synchronized' context.
         *
         *  \see AssertExternallySynchronizedMutex::SetAssertExternallySynchronizedMutexContext
         */
        nonvirtual void SetAssertExternallySynchronizedMutexContext (const shared_ptr<Debug::AssertExternallySynchronizedMutex::SharedContext>& sharedContext);
#endif

    public:
        /**
         *  Allow readonly access to the Headers object. This is just checked (assertions) for re-entrancy.
         *  It can be called in any state (during transaction or after).
         * 
         * \note - this returns an INTERNAL POINTER to the Response, so be SURE to remember this with respect to
         *         thread safety, and lifetime (thread safety checked/enforced in debug builds with SetAssertExternallySynchronizedMutexContext);
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
         *         thread safety, and lifetime (thread safety checked/enforced in debug builds with SetAssertExternallySynchronizedMutexContext);
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
         *  Access the HTTP request body Content-Type, if any given
         *  A short-hand, equivalent to fetching headers().contentType(), provided because this is such a commonly accessed part of Request.
         */
        Common::ExtendableProperty<optional<InternetMediaType>> contentType;

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

    protected:
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex _fThisAssertExternallySynchronized;

    private:
        tuple<Status, optional<String>> fStatusAndOverrideReason_;
        IO::Network::HTTP::Headers      fHeaders_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Response.inl"

#endif /*_Stroika_Foundation_IO_Network_HTTP_Response_h_*/
