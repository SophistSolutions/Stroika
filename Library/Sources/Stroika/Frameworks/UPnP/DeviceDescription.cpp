/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <sstream>

#include    "../../Foundation/DataExchange/XML/WriterUtils.h"
#include    "../../Foundation/Streams/iostream/InputStreamAdapter.h"

#include    "DeviceDescription.h"


using   namespace   Stroika::Foundation;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::UPnP;







/*
********************************************************************************
*************************** DeviceDescription::Icon ****************************
********************************************************************************
*/
DeviceDescription::Icon::Icon ()
    : fMimeType ()
    , fHorizontalPixels (16)
    , fVerticalPixels (16)
    , fColorDepth (8)
    , fURL ()
{
}





/*
********************************************************************************
************************ DeviceDescription::Service ****************************
********************************************************************************
*/
DeviceDescription::Service::Service ()
    : fServiceType ()
    , fServiceID ()
    , fSCPDURL ()
    , fControlURL ()
    , fEventSubURL ()
{
}





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
    , fIcons ()
    , fServices ()
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
    if (dd.fManufacturingURL.IsPresent ()) {
        tmp << "		<manufacturerURL>" << QuoteForXML (dd.fManufacturingURL->GetFullURL ()) << "</manufacturerURL>" << endl;
    }
    tmp << "		<modelDescription>" << QuoteForXML (dd.fModelDescription) << "</modelDescription>" << endl;
    tmp << "		<modelName>" << QuoteForXML (dd.fModelName) << "</modelName>" << endl;
    tmp << "		<modelNumber>" << QuoteForXML (dd.fModelNumber) << "</modelNumber>" << endl;
    if (dd.fModelURL.IsPresent ()) {
        tmp << "		<modelURL>" << QuoteForXML (dd.fModelURL->GetFullURL ()) << "</modelURL>" << endl;
    }
    tmp << "		<serialNumber>" << QuoteForXML (dd.fSerialNumber) << "</serialNumber>" << endl;
    tmp << "		<UDN>uuid:" << QuoteForXML (d.fDeviceID) << "</UDN>" << endl;
    if (not dd.fUPC.empty ()) {
        tmp << "		<UPC>" << QuoteForXML (dd.fUPC) << "</UPC>" << endl;
    }
    if (not dd.fIcons.empty ()) {
        tmp << "		<iconList>" << endl;
        for (DeviceDescription::Icon i : dd.fIcons) {
            tmp << "			<icon>" << endl;
            tmp << "				<mimetype>" << QuoteForXML (i.fMimeType.As<String> ()) << "</mimetype>" << endl;
            tmp << "				<width>" << i.fHorizontalPixels << "</width>" << endl;
            tmp << "				<height>" << i.fVerticalPixels << "</height>" << endl;
            tmp << "				<depth>" << i.fColorDepth << "</depth>" << endl;
            tmp << "				<url>" << QuoteForXML (i.fURL.GetFullURL ()) << "</url>" << endl;
            tmp << "			</icon>" << endl;
        }
        tmp << "		</iconList>" << endl;
    }
    if (not dd.fServices.empty ()) {
        tmp << "		<serviceList>" << endl;
        for (DeviceDescription::Service i : dd.fServices) {
            tmp << "			<service>" << endl;
            tmp << "				<serviceType>" << QuoteForXML (i.fServiceType) << "</serviceType>" << endl;
            tmp << "				<serviceId>" << QuoteForXML (i.fServiceID) << "</serviceId>" << endl;
            tmp << "				<SCPDURL>" << QuoteForXML (i.fSCPDURL.GetFullURL ()) << "</SCPDURL>" << endl;
            tmp << "				<controlURL>" << QuoteForXML (i.fControlURL.GetFullURL ()) << "</controlURL>" << endl;
            tmp << "				<eventSubURL>" << QuoteForXML (i.fEventSubURL.GetFullURL ()) << "</eventSubURL>" << endl;
            tmp << "			</service>" << endl;
        }
        tmp << "		</serviceList>" << endl;
    }

#if 0
    tmp << "	<deviceList>" << endl;
    // <!-- Description of embedded devices defined by a UPnP Forum working committee
    // (if any) go here -->
    // <!-- Description of embedded devices added by UPnP vendor (if any) go here -->
    tmp << "	</deviceList>" << endl;
#endif

    if (dd.fPresentationURL.IsPresent ()) {
        tmp << "		<presentationURL>" << QuoteForXML (dd.fPresentationURL->GetFullURL ()) << "</presentationURL>" << endl;
    }
    tmp << "	</device>" << endl;
    tmp << "</root>" << endl;
    Streams::iostream::InputStreamAdapter<Memory::Byte> tmp2 (tmp);
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
