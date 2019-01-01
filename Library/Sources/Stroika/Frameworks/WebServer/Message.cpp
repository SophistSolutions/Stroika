/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../../Foundation/Characters/StringBuilder.h"
#include "../../Foundation/Characters/ToString.h"

#include "Message.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebServer;

/*
 ********************************************************************************
 ************************* WebServer::Message ***********************************
 ********************************************************************************
 */
String Message::ToString () const
{
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    StringBuilder                                       sb;
    sb += L"{";
    sb += L"PeerAddress: " + Characters::ToString (fPeerAddress_) + L", ";
    sb += L"Request: " + Characters::ToString (fRequest_) + L", ";
    sb += L"Response: " + Characters::ToString (fResponse_);
    sb += L"}";
    return sb.str ();
}
