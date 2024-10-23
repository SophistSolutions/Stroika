/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Execution/FeatureNotSupportedException.h"

#if qStroika_HasComponent_zlib
#include "Private_/ZLibSupport.h"
#endif

#include "Deflate.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::Compression;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Streams;

#if qStroika_HasComponent_zlib
using Compression::Private_::DeflateRep_;
using Compression::Private_::InflateRep_;
#endif

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

#if !qStroika_HasComponent_zlib
namespace {
    const auto kNotSuppExcept_ = Execution::FeatureNotSupportedException{"Deflate (ZLIB)"sv};
}
#endif

Compression::Ptr Deflate::Compress::New (const Deflate::Compress::Options& o)
{
#if qStroika_HasComponent_zlib
    struct MyRep_ : IRep {
        Deflate::Compress::Options fOptions_;
        shared_ptr<DeflateRep_>    fDelegate2;
        MyRep_ (const Deflate::Compress::Options& o)
            : fOptions_{o}
        {
        }
        virtual InputStream::Ptr<byte> Transform (const InputStream::Ptr<byte>& src)
        {
            fDelegate2 = make_shared<DeflateRep_> (src, fOptions_, false);
            return InputStream::Ptr<byte>{fDelegate2};
        }
        virtual optional<Compression::Stats> GetStats () const
        {
            return nullopt;
        }
    };
    return Compression::Ptr{make_shared<MyRep_> (o)};
#else
    Execution::Throw (kNotSuppExcept_);
#endif
}
Compression::Ptr Deflate::Decompress::New ([[maybe_unused]] const Deflate::Decompress::Options& o)
{
#if qStroika_HasComponent_zlib
    struct MyRep_ : IRep {
        shared_ptr<Private_::InflateRep_> fDelegate2;
        virtual InputStream::Ptr<byte>    Transform (const InputStream::Ptr<byte>& src)
        {
            fDelegate2 = make_shared<Private_::InflateRep_> (src, false);
            return InputStream::Ptr<byte>{fDelegate2};
        }
        virtual optional<Compression::Stats> GetStats () const
        {
            return nullopt;
        }
    };
    return Compression::Ptr{make_shared<MyRep_> ()};
#else
    Execution::Throw (kNotSuppExcept_);
#endif
}
