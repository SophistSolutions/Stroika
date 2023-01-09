/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
optional<Fault> SOAP::Deserialize_Fault (const Streams::InputStream<byte>::Ptr& from)
{
    using namespace ObjectReader;
    static const Registry kSOAPTypeMapper_ = [] () -> Registry {
        //      using   namespace DataExchange::StructuredStreamEvents;
        Registry mapper;
        mapper.AddCommonType<String> ();
        mapper.AddCommonReader_Class<Fault> (initializer_list<StructFieldInfo>{
            {Name{"faultcode"sv}, StructFieldMetaInfo{&Fault::faultcode}},
            {Name{"faultstring"sv}, StructFieldMetaInfo{&Fault::faultstring}},
        });
        return mapper;
    }();
    Fault result;
    try {
        IConsumerDelegateToContext ctx{kSOAPTypeMapper_, make_shared<ReadDownToReader> (kSOAPTypeMapper_.MakeContextReader (&result), Name{"Fault"sv})};
#if qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing && USE_NOISY_TRACE_IN_THIS_MODULE_
        ctx.fContext.fTraceThisReader = true;
#endif
        XML::SAXParse (from, ctx);

        return result;
    }
    catch (...) {
        // not a bug, but a feature...
        return nullopt;
    }
}

optional<Fault> SOAP::Deserialize_Fault (const Memory::BLOB& from)
{
    return Deserialize_Fault (from.As<Streams::InputStream<byte>::Ptr> ());
}
