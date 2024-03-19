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
#include "../../../Foundation/Streams/MemoryStream.h"

#include "Specification.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebService::OpenAPI;

Specification::Specification (const Streams::InputStream::Ptr<byte>& b, const optional<DataExchange::InternetMediaType>& mediaType)
    : Specification{DataExchange::Variant::JSON::Reader{}.Read (b)}
{
}

Memory::BLOB Specification::As (const DataExchange::InternetMediaType& mediaType)
{
    if (mediaType == kMediaType) {
        Streams::MemoryStream::Ptr<byte> b = Streams::MemoryStream::New<byte> ();
        DataExchange::Variant::JSON::Writer{}.Write (fValue_);
        return b.As<Memory::BLOB> ();
    }
    else {
        Execution::Throw (Execution::RuntimeErrorException{"Type not supported"sv});
    }
}