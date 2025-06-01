/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/DataExchange/Archive/Zip/Private_minizip_.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"

#include "Writer.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::Archive;
using namespace Stroika::Foundation::DataExchange::Archive::Writer;
using namespace Stroika::Foundation::Execution;

#if qStroika_HasComponent_zlib
using std::byte;

/*
 ********************************************************************************
 ************************ DataExchange::Archive::Writer::Ptr ********************
 ********************************************************************************
 */
Archive::Writer::Ptr Archive::Zip::Writer::New (const Streams::OutputStream::Ptr<byte>& writeTo)
{
    return Archive::Writer::Ptr{shared_ptr<IRep>{}};
}
#endif