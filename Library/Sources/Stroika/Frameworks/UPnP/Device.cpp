/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../../Foundation/IO/Network/NetworkInterfaces.h"

#include    "Device.h"



using   namespace   Stroika::Foundation;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::UPnP;







/*
********************************************************************************
******************************* DeviceDescription ******************************
********************************************************************************
*/
String  UPnP::MungePrimaryMacAddrIntoBaseDeviceID (String baseDeviceID)
{
    Require (baseDeviceID.length () == 36); // also require layout normal as in 315CAAE0-1335-57BF-A178-24C9EE756627 but check later
    String  result = baseDeviceID;
    String  macAddr = IO::Network::GetPrimaryNetworkDeviceMacAddress ();
    macAddr = macAddr.ReplaceAll (L"-", L"");
    if (macAddr.length () > 12) {
        macAddr = macAddr.SubString (0, 12);
    }
    while (macAddr.length () < 12) {
        macAddr += L"0";
    }
    result = baseDeviceID.SubString (0, 36 - 12) + macAddr;
    Ensure (result.length () == 36);
    return result.ToLowerCase ();       // just normalize
}
