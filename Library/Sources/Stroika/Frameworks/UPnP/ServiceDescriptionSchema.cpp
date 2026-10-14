/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Debug/Assertions.h"

#include "ServiceDescription.h"

/*
 *  The UPnP service description (SCPD) schema, embedded - with one correction, which is most of what this file is
 *  about. @see DeviceDescriptionSchema.cpp for why each schema is a translation unit of its own, and for why a copy is
 *  carried here at all.
 *
 *  The schema is https://upnp.org/schemas/service-1-0.xsd as it stood 2026-10-13.
 *
 *  THE CORRECTION: as published, the schema restricts stateVariable's sendEvents and multicast attributes to the
 *  enumeration {"1", "0"}. The specification it describes says otherwise, in prose - UPnP Device Architecture 1.0,
 *  service description: "sendEvents attribute defines whether event messages will be generated when the value of this
 *  state variable changes; non-evented state variables have sendEvents="no"; default is sendEvents="yes"". multicast,
 *  added in 1.1, is spelled the same way, and every UPnP implementation writes both as yes/no.
 *
 *  So the published schema rejects every real device, Stroika's own output among them - which makes it useless as a
 *  validator in exactly the way a caller would want to use it. The two enumerations are widened here to accept yes and
 *  no as well as 1 and 0, and nothing else about the schema is touched: the file stays what upnp.org serves, byte for
 *  byte, so it can be diffed against the source at any time, and the one disagreement lives here where the reason for
 *  it can be read. It is also the ONLY disagreement - with these two widened, a full SCPD holds as published
 *  (@see Tests/54's DescriptionsValidateAgainstPublishedSchemas_, which checks both halves of that).
 *
 *  Embedded as BYTES rather than written as a C++ string literal, for the same reason SOAP's is: a schema is someone
 *  else's file, and a literal would be read as cp1252 by the MSVC builds, which are passed no /utf-8, the day one of
 *  them gains a character outside ASCII.
 *
 *      @todo   the xxd rule in this directory's Makefile, and the .embed it generates, go away for #embed once every
 *              compiler Stroika supports has it - the file, and everything said above, stay as they are.
 */
// the embed too, not just the function - @see DeviceDescriptionSchema.cpp
#if qStroika_Foundation_DataExchange_XML_SupportParsing and qStroika_Foundation_DataExchange_XML_SupportSchema and qStroika_Foundation_DataExchange_XML_SupportDOM
namespace {
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wunused-variable\""); // warning suppression for Schemas_service_1_0_xsd_len
    constexpr
#include "service-1-0.xsd.embed"
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wunused-variable\"");
}

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP;

XML::Schema::Ptr UPnP::ServiceDescriptionSchema (const XML::Providers::ISchemaProvider& p)
{
    /*
     *  Compiled afresh each call, and deliberately not kept - @see DeviceDescriptionSchema.cpp, which says why at
     *  length. The correction costs one pass over 11K, which beside compiling a schema is nothing.
     */
    static constexpr string_view kAsPublished_{"<xsd:enumeration value=\"1\"/>"sv};
    static constexpr string_view kWidened_{"<xsd:enumeration value=\"1\"/><xsd:enumeration value=\"yes\"/><xsd:enumeration value=\"no\"/>"sv};
    string corrected{reinterpret_cast<const char*> (Schemas_service_1_0_xsd), std::size (Schemas_service_1_0_xsd)};
    // [[maybe_unused]]: Assert is the only reader, and on a compiler with neither [[assume]] nor MSVC's __assume it
    // compiles to nothing at all (Common/StdCompat.h) - which would leave this set and never read
    [[maybe_unused]] unsigned nWidened{0};
    for (size_t i = corrected.find (kAsPublished_); i != string::npos; i = corrected.find (kAsPublished_, i + kWidened_.length ())) {
        corrected.replace (i, kAsPublished_.length (), kWidened_);
        ++nWidened;
    }
    // sendEvents and multicast, and nothing else in the file is written that way. A published schema that had changed
    // would land here first
    Assert (nWidened == 2);
    return XML::Schema::New (p, Memory::BLOB{as_bytes (span{corrected})}, nullptr);
}
#endif
