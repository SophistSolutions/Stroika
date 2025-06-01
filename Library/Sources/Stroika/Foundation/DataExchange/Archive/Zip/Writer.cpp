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
using namespace Stroika::Foundation::Memory;

#if qStroika_HasComponent_zlib
using Memory::BLOB;
using std::byte;

namespace {
    struct MyRep_ : Archive::Writer::IRep {
        Streams::OutputStream::Ptr<byte> fOutputTo_;
        MyRep_ (const Streams::OutputStream::Ptr<byte>& writeTo)
            : fOutputTo_{writeTo}
        {
        }
        virtual void Add (const String& fileName, const BLOB& data) override
        {
        }
    };
}

/*
 ********************************************************************************
 ********************** DataExchange::Archive::Writer::Ptr **********************
 ********************************************************************************
 */
Archive::Writer::Ptr Archive::Zip::Writer::New (const Streams::OutputStream::Ptr<byte>& writeTo)
{
    Require (writeTo.IsSeekable ());
    return Archive::Writer::Ptr{make_shared<MyRep_> (writeTo)};
}
#endif