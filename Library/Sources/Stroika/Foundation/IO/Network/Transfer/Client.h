/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Client_h_
#define _Stroika_Foundation_IO_Network_Transfer_Client_h_ 1

#include "../../../StroikaPreComp.h"

#include "Connection.h"

/**
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *
 * TODO:
 *      @todo   FIXUP Docs and code so on thrown response, and open stream in the response is replaced by
 *              a FULLY REALIZED STREAM (MEMORY STREAM)
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-443 - cleanup exception handling and docs a little.
 *
 *      @todo   Probably should redo Request so it can optionally use a BLOB or
 *              Stream (like Response). DO NOW the header / class changes - making
 *              CTOR/accessors, so easiser to change funcitonality later!!!
 *
 *      @todo   Add thread safety (locks/semaphores)
 *
 *                  Decide on and DOCUMENT threading policy. For example - do we need
 *                  locks internally in the connection object or DEFINE that its
 *                  the callers resposabiltiy. PROBABLY best to do in the Connection object itself?
 *
 *                  ADD CRITICAL SECTIONS!!! - or DOCUMENT CALLERS REPSONABILTY
 *
 *      @todo   Add (optionally callable) Connect() method. Send etc connect on-demand as needed
 *              but sometimes its useful to pre-create connections (to reduce latnecy).
 *
 *      @todo   Add factory for 'CreateConnection'  - so you can do 'dependnecy injection' or other
 *              way to configure http client support library (winhttp versus libcurl or other).
 *
 *      @todo   Redo Response to fully/properly support incremental read through streams. Must do
 *              CTOR on response object taking a stream, and then reasonably (tbd) how to handle
 *              calls to getResponseBLOB? (probably assert or except?)
 *
 *      @todo   Progress Callbacks?
 *
 *      @todo   Add Client side certs
 *
 *      @todo   Refine server-side-cert checking mechanism (review LIBCURL to see what makes sense)
 *              PROBABLY just a callback mechanism - with a few default callabcks you can plugin (like reject bad certs)
 *              MAYBE just add FLAG saying whether to VALIDATE_HOST? Maybe callback API"?
 *
 *                        //  This COULD be expanded to include either a BOOL to CHECK remote SSL info, or a callback (which can throw)
 *                        //  if invalid SSL INFO from server side. Thats probably best. callback whcih throws, and set it to such a callback by default!
 *                        //      -- LGP 2012-06-26
 *
 */

namespace Stroika::Foundation::IO::Network::Transfer {

    [[deprecated ("Since v2.1d27, use Connection::New")]] inline Connection::Ptr CreateConnection (const Connection::Options& options = {})
    {
        return Connection::New (options);
    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Client.inl"

#endif /*_Stroika_Foundation_IO_Network_Transfer_Client_h_*/
