/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../../Foundation/Characters/Format.h"
#include "../../../Foundation/Characters/StringBuilder.h"
#include "../../../Foundation/Characters/ToString.h"
#include "../../../Foundation/Containers/Mapping.h"
#include "../../../Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "../../../Foundation/DataExchange/Variant/JSON/Reader.h"
#include "../../../Foundation/DataExchange/Variant/JSON/Writer.h"

#include "Specification.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebService::OpenAPI;

namespace {
    const String kServersElt_ = "servers"sv;
    const String kURLElt_     = "url"sv;
}

namespace {
    VariantValue rd_ (const Streams::InputStream::Ptr<byte>& b, const optional<DataExchange::InternetMediaType>& mediaType)
    {
        if (mediaType) { // only check if specifed, else assume its right
            if (not InternetMediaTypeRegistry::Get ().IsA (kMediaType, *mediaType)) {
                Execution::Throw (Execution::RuntimeErrorException{"Unrecognized media type"sv});
            }
        }
        return DataExchange::Variant::JSON::Reader{}.Read (b);
    }
}

/*
 ********************************************************************************
 *********************************** Specification ******************************
 ********************************************************************************
 */
Specification::Specification (const Streams::InputStream::Ptr<byte>& b, const optional<DataExchange::InternetMediaType>& mediaType)
    : Specification{rd_ (b, mediaType)}
{
}

Memory::BLOB Specification::As (const DataExchange::InternetMediaType& mediaType)
{
    if (mediaType == kMediaType) {
        return DataExchange::Variant::JSON::Writer{}.WriteAsBLOB (fValue_);
    }
    else {
        Execution::Throw (Execution::RuntimeErrorException{"Type not supported"sv});
    }
}

Sequence<URI> Specification::GetServers () const
{
    // @todo store in better internal rep assuring structure already has all these fields... - so the below cannot fail (as is it can)
    return fValue_.As<Mapping<String, VariantValue>> ().LookupValue (kServersElt_, {}).As<Sequence<VariantValue>> ().Map<Sequence<URI>> ([] (auto vv) {
        return URI{vv.template As<Mapping<String, VariantValue>> ().LookupValue (kURLElt_).template As<String> ()};
    });
}

void Specification::SetServers (const Sequence<URI>& s)
{
    Mapping<String, VariantValue> vv = fValue_.As<Mapping<String, VariantValue>> ();
    vv.Add (kServersElt_, VariantValue{s.Map<Sequence<VariantValue>> ([] (URI u) {
                return VariantValue{Mapping<String, VariantValue>{{kURLElt_, u.As<String> ()}}};
            })});
    fValue_ = VariantValue{vv};
}
