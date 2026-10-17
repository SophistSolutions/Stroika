/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "SOAP.h"

/*
 *  The SOAP 1.1 envelope schema, embedded.
 *
 *  Its own translation unit so that a program which never validates never carries it: Stroika is a static library, so
 *  an object nothing references is not pulled from the archive at all - no linker flag, no pragma, nothing to get wrong
 *  on one toolchain and not another. That only holds while nothing ELSE in SOAP calls EnvelopeSchema: Options takes the
 *  schema a caller wants rather than a flag that would reach for this one, which is what keeps the reference out.
 *
 *  The schema is http://schemas.xmlsoap.org/soap/envelope/ VERBATIM - Schemas/soap-envelope-1.1.xsd byte for byte,
 *  notice included, which its licence (the W3C Software License) requires travel with any copy. It imports and includes
 *  nothing, so it compiles with no Resource::Resolver.
 *
 *  KEPT IN THE TREE rather than fetched at build time the way ThirdPartyComponents are (LGP 2026-10-13). That machinery
 *  is for versioned source that then gets built, and everything it fetches carries a configure feature flag so it can be
 *  swapped, disabled, or pointed at a system copy - none of which a frozen 2001 standards document of 6K has any use
 *  for: there is no later version to want, and never will be. Tests/34 keeps three third-party .xsd files the same way.
 *  The line is source-we-compile versus data-we-embed.
 *
 *  Embedded as BYTES rather than written as a C++ string literal because that notice carries a copyright sign: MSVC is
 *  passed no /utf-8, so a literal of it would be read as cp1252 and the notice mangled on Windows alone.
 *
 *      @todo   the xxd rule in this directory's Makefile, and the .embed it generates, go away for #embed once every
 *              compiler Stroika supports has it - the file, and everything said above, stay as they are.
 */
// the embed too, not just the function: with no schema support this file has nothing to compile, and an unused array of 6K
// would warn
#if qStroika_Foundation_DataExchange_XML_SupportParsing and qStroika_Foundation_DataExchange_XML_SupportSchema and qStroika_Foundation_DataExchange_XML_SupportDOM
namespace {
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wunused-variable\""); // warning suppression for Schemas_soap_envelope_1_1_xsd_len
    constexpr
#include "soap-envelope-1.1.xsd.embed"
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wunused-variable\"");
}

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebService;

XML::Schema::Ptr SOAP::EnvelopeSchema (const XML::Options& o)
{
    /*
     *  Compiled afresh each call, and deliberately not kept: a Schema::Ptr belongs to the provider that made it and may
     *  not outlive it (which XML::Providers::LibXML2::Provider's DTOR checks), and nothing holding one in a static can
     *  promise that - destruction order across translation units is not ours to choose, and the provider is the
     *  caller's anyway. A caller sending more than the occasional message holds its own, in its own Options, where the
     *  lifetime is plain to see.
     */
    return XML::Schema::New (Memory::BLOB::Attach (Schemas_soap_envelope_1_1_xsd), nullptr, o);
}
#endif
