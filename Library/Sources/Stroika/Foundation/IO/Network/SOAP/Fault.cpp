/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../DataExchange/StructuredStreamEvents/ObjectReader.h"
#include "../../../DataExchange/XML/SAXReader.h"

#include "Fault.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::StructuredStreamEvents;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::SOAP;

/*
 ********************************************************************************
 ********************** Network::SOAP::Deserialize_Fault ************************
 ********************************************************************************
 */
Optional<Fault> SOAP::Deserialize_Fault (const Streams::InputStream<Byte>::Ptr& from)
{
    using namespace ObjectReader;
    static const Registry kSOAPTypeMapper_ = []() -> Registry {
        //      using   namespace DataExchange::StructuredStreamEvents;
        Registry mapper;
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Winvalid-offsetof\""); // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddCommonType<String> ();
        mapper.AddClass<Fault> (initializer_list<StructFieldInfo>{
            {Name{L"faultcode"}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (Fault, faultcode)},
            {Name{L"faultstring"}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (Fault, faultstring)},
        });
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Winvalid-offsetof\"");
        return mapper;
    }();
    Fault result;
    try {
        IConsumerDelegateToContext ctx{kSOAPTypeMapper_, make_shared<ReadDownToReader> (kSOAPTypeMapper_.MakeContextReader (&result), Name (L"Fault"))};
#if qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing && USE_NOISY_TRACE_IN_THIS_MODULE_
        ctx.fContext.fTraceThisReader = true;
#endif
        XML::SAXParse (from, ctx);

        return result;
    }
    catch (...) {
        // not a bug, but a feature...
        return Optional<Fault>{};
    }
}

Optional<Fault> SOAP::Deserialize_Fault (const Memory::BLOB& from)
{
    return Deserialize_Fault (from.As<Streams::InputStream<Byte>::Ptr> ());
}
