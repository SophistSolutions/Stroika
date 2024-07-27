/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Execution/FeatureNotSupportedException.h"

#if qHasFeature_ZLib
#include "Private_/ZLibSupport.h"
#endif

#include "GZip.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::Compression;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Streams;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

#if !qHasFeature_ZLib
namespace {
    const auto kNotSuppExcept_ = Execution::FeatureNotSupportedException{"GZip (ZLIB)"sv};
}
#endif
#if qHasFeature_ZLib
using Compression::Private_::DeflateRep_;
using Compression::Private_::InflateRep_;
#endif

Compression::Ptr GZip::Compress::New (const GZip::Compress::Options& o)
{
#if qHasFeature_ZLib
    struct MyRep_ : IRep {
        GZip::Compress::Options fOptions_;
        shared_ptr<DeflateRep_> fDelegate2;
        MyRep_ (const GZip::Compress::Options& o)
            : fOptions_{o}
        {
        }
        virtual InputStream::Ptr<byte> Transform (const InputStream::Ptr<byte>& src)
        {
            fDelegate2 = make_shared<Private_::DeflateRep_> (src, fOptions_, true);
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
Compression::Ptr GZip::Decompress::New ([[maybe_unused]] const GZip::Decompress::Options& o)
{
#if qHasFeature_ZLib
    struct MyRep_ : IRep {
        shared_ptr<InflateRep_>        fDelegate2;
        virtual InputStream::Ptr<byte> Transform (const InputStream::Ptr<byte>& src)
        {
            fDelegate2 = make_shared<InflateRep_> (src, true);
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
