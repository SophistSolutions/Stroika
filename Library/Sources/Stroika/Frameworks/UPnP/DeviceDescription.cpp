/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <sstream>

#include    "../../Foundation/DataExchange/XML/WriterUtils.h"
#include    "../../Foundation/Streams/iostream/BinaryInputStreamFromIStreamAdapter.h"

#include    "DeviceDescription.h"


using   namespace   Stroika::Foundation;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::UPnP;







/*
********************************************************************************
******************************* DeviceDescription ******************************
********************************************************************************
*/
DeviceDescription::DeviceDescription ()
    : fPresentationURL ()
    , fDeviceType ()
    , fManufactureName ()
    , fFriendlyName ()
    , fManufacturingURL ()
    , fModelDescription ()
    , fModelName ()
    , fModelNumber ()
    , fModelURL ()
    , fSerialNumber ()
    , fUPC ()
{
}









/*
********************************************************************************
********************************* UPnP::Serialize ******************************
********************************************************************************
*/

Memory::BLOB        UPnP::Serialize (const Device& d, const DeviceDescription& dd)
{
    using   namespace DataExchange::XML;

    /*
    // very very rough prelim draft
    *Example based on
    http://upnp.org/specs/arch/UPnP-arch-DeviceArchitecture-v1.0.pdf
    */
    stringstream tmp;
    tmp << "<?xml version=\"1.0\"?>" << endl;
    tmp << "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">" << endl;
    tmp << "	<specVersion>" << endl;
    tmp << "		<major>1</major>" << endl;
    tmp << "		<minor>0</minor>" << endl;
    tmp << "	</specVersion>" << endl;
    tmp << "	<device>" << endl;
    tmp << "		<deviceType>" << QuoteForXML (dd.fDeviceType) << "</deviceType>" << endl;
    tmp << "		<friendlyName>" << QuoteForXML (dd.fFriendlyName) << "</friendlyName>" << endl;
    tmp << "		<manufacturer>" << QuoteForXML (dd.fManufactureName) << "</manufacturer>" << endl;
    if (not dd.fManufacturingURL.empty ()) {
        tmp << "		<manufacturerURL>" << QuoteForXML (dd.fManufacturingURL) << "</manufacturerURL>" << endl;
    }
    tmp << "		<modelDescription>" << QuoteForXML (dd.fModelDescription) << "</modelDescription>" << endl;
    tmp << "		<modelName>" << QuoteForXML (dd.fModelName) << "</modelName>" << endl;
    tmp << "		<modelNumber>" << QuoteForXML (dd.fModelNumber) << "</modelNumber>" << endl;
    tmp << "		<modelURL>" << QuoteForXML (dd.fModelURL) << "</modelURL>" << endl;
    tmp << "		<serialNumber>" << QuoteForXML (dd.fSerialNumber) << "</serialNumber>" << endl;
    tmp << "		<UDN>uuid:" << QuoteForXML (d.fDeviceID) << "</UDN>" << endl;
    if (not dd.fUPC.empty ()) {
        tmp << "		<UPC>" << QuoteForXML (dd.fUPC) << "</UPC>" << endl;
    }

#if 0
    tmp << "		<iconList>" << endl;
    tmp << "			<icon>" << endl;
    tmp << "				<mimetype>image/png</mimetype>" << endl;
    tmp << "				<width>16</width>" << endl;
    tmp << "				<height>16</height>" << endl;
    tmp << "				<depth>2</depth>" << endl;
    tmp << "				<url>myIcon.png</url>" << endl;
    tmp << "			</icon>" << endl;
    // <!-- XML to declare other icons, if any, go here -->
    tmp << "		</iconList>" << endl;
#endif

#if 0
    tmp << "	<serviceList>" << endl;
    tmp << "		<service>" << endl;
    tmp << "			<serviceType>urn:schemas-upnp-org:service:serviceType:v</serviceType>" << endl;
    tmp << "			<serviceId>urn:upnp-org:serviceId:serviceID</serviceId>" << endl;
    tmp << "			<SCPDURL>URL to service description</SCPDURL>" << endl;
    tmp << "			<controlURL>URL for control</controlURL>" << endl;
    tmp << "			<eventSubURL>URL for eventing</eventSubURL>" << endl;
    tmp << "		</service>" << endl;
    // <!-- Declarations for other services defined by a UPnP Forum working committee
    // (if any) go here -->
    // <!-- Declarations for other services added by UPnP vendor (if any) go here -->
    tmp << "	</serviceList>" << endl;
#endif

#if 0
    tmp << "	<deviceList>" << endl;
    // <!-- Description of embedded devices defined by a UPnP Forum working committee
    // (if any) go here -->
    // <!-- Description of embedded devices added by UPnP vendor (if any) go here -->
    tmp << "	</deviceList>" << endl;
#endif

    if (not dd.fPresentationURL.empty ()) {
        tmp << "		<presentationURL>" << QuoteForXML (dd.fPresentationURL) << "</presentationURL>" << endl;
    }
    tmp << "	</device>" << endl;
    tmp << "</root>" << endl;
    Streams::iostream::BinaryInputStreamFromIStreamAdapter tmp2 (tmp);
    return tmp2.ReadAll ();
}



/*
********************************************************************************
******************************* UPnP::DeSerialize ******************************
********************************************************************************
*/
DeviceDescription   UPnP::DeSerialize (const Memory::BLOB& b)
{
    AssertNotImplemented ();
    return DeviceDescription ();
}
