/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Frameworks/UPnP/DeviceDescription.h"

/*
 *  The UPnP device description schema, embedded.
 *
 *  Its own translation unit so that a program which never validates never carries it: Stroika is a static library, so
 *  an object nothing references is not pulled from the archive at all - no linker flag, no pragma, nothing to get wrong
 *  on one toolchain and not another. That only holds while nothing ELSE in UPnP calls DeviceDescriptionSchema: Options
 *  takes the schema a caller wants rather than a flag that would reach for this one, which is what keeps the reference
 *  out. The service description's schema is a translation unit of its own for the same reason - a device that only ever
 *  reads descriptions carries neither, and one that reads only descriptions carries one.
 *
 *  The schema is https://upnp.org/schemas/device-1-0.xsd as it stood 2026-10-13: what Serialize writes holds against
 *  it (@see Tests/55's DescriptionsValidateAgainstPublishedSchemas_).
 *
 *  KEPT IN THE TREE and shipped, not fetched the way ThirdPartyComponents are - @see WebService/SOAP_EnvelopeSchema.cpp,
 *  which says why at length. upnp.org/schemas/ serves both files openly, and other open-source UPnP projects carry
 *  their own copies - GUPnP ships both as RELAX NG (doc/device-description.rnc, doc/service-description.rnc), and the
 *  .xsd itself sits in public repositories.
 *
 *  Embedded as BYTES rather than written as a C++ string literal, for the same reason SOAP's is: a schema is someone
 *  else's file, and a literal would be read as cp1252 by the MSVC builds, which are passed no /utf-8, the day one of
 *  them gains a character outside ASCII.
 *
 *      @todo   the xxd rule in this directory's Makefile, and the .embed it generates, go away for #embed once every
 *              compiler Stroika supports has it - the file, and everything said above, stay as they are.
 */
// the embed too, not just the function: with no schema support this file has nothing to compile, and an unused array
// of 11K would warn
#if qStroika_Foundation_DataExchange_XML_SupportParsing and qStroika_Foundation_DataExchange_XML_SupportSchema and qStroika_Foundation_DataExchange_XML_SupportDOM
namespace {
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wunused-variable\""); // warning suppression for Schemas_device_1_0_xsd_len
    constexpr
#include "device-1-0.xsd.embed"
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wunused-variable\"");
}

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP;

XML::Schema::Ptr UPnP::DeviceDescriptionSchema (const XML::Options& o)
{
    /*
     *  Compiled afresh each call, and deliberately not kept: a Schema::Ptr belongs to the provider that made it and may
     *  not outlive it (which XML::Providers::LibXML2::Provider's DTOR checks), and nothing holding one in a static can
     *  promise that - destruction order across translation units is not ours to choose, and the provider is the
     *  caller's anyway. A device reading more than the occasional description holds its own, in its own Options, where
     *  the lifetime is plain to see.
     */
    return XML::Schema::New (Memory::BLOB::Attach (Schemas_device_1_0_xsd), nullptr, o);
}
#endif
