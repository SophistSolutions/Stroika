/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    "../../../../Foundation/Characters/Format.h"
#include    "../../../../Foundation/Execution/Sleep.h"
#include    "../../../../Foundation/Execution/Thread.h"
#include    "../../../../Foundation/IO/Network/Socket.h"
#include    "../../../../Foundation/Streams/BasicBinaryOutputStream.h"
#include    "../../../../Foundation/Streams/TextOutputStreamBinaryAdapter.h"

#include    "../Common.h"
#include    "BasicServer.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::UPnP;
using   namespace   Stroika::Frameworks::UPnP::SSDP;
using   namespace   Stroika::Frameworks::UPnP::SSDP::Server;






/*
********************************************************************************
******************************* BasicServer::Rep_ ******************************
********************************************************************************
*/
class   BasicServer::Rep_ {
public:
    Rep_ (const Device& d, const FrequencyInfo& fi)
    {
        fNotifierThread_ = Thread ([d, fi]() {
            PeriodicNotifier l;
            l.Run (d, PeriodicNotifier::FrequencyInfo ());
        });
        fNotifierThread_.Start ();
        fSearchResponderThread_ = Thread ([d, fi]() {
            SearchResponder sr;
            sr.Run (d);
        });
        fSearchResponderThread_.Start ();
    }
    ~Rep_ ()
    {
        fNotifierThread_.AbortAndWaitForDone ();
        fSearchResponderThread_.AbortAndWaitForDone ();
    }
    Execution::Thread     fNotifierThread_;
    Execution::Thread     fSearchResponderThread_;
};



/*
********************************************************************************
********************************** BasicServer *********************************
********************************************************************************
*/
BasicServer::BasicServer (const Device& d, const FrequencyInfo& fi)
    : fRep_ (new Rep_ (d, fi))
{
}
