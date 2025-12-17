/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Execution/FeatureNotSupportedException.h"

#if qStroika_HasComponent_zstd
#include <zstd.h>
#endif

#include "ZStd.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::Compression;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Streams;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

#if !qStroika_HasComponent_zstd
namespace {
    const auto kNotSuppExcept_ = Execution::FeatureNotSupportedException{"ZStd"sv};
}
#endif

#if qStroika_HasComponent_zstd
namespace {

    // EARLY DRAFT BASED ON https://github.com/facebook/zstd/blob/dev/examples/streaming_compression.c

    inline void ThrowIfZStdErr_ (size_t rc) // CHECK_ZSTD
    {
        if (ZSTD_isError (rc)) {
            Execution::Throw (Execution::RuntimeErrorException{"ZStd error: {}"_f(String::FromNarrowSDKString (::ZSTD_getErrorName (rc)))});
        }
    }

    struct BaseRep_ : public InputStream::IRep<byte> {
    private:
        static constexpr size_t CHUNK_ = 16384;

    public:
        Streams::InputStream::Ptr<byte> fInStream_; // consider wrapping in StreamReader for efficiency sake - maybe unhelpful due to CHUNK logic below
        ZSTD_CCtx*     fCCtx_{nullptr};
        byte           fInBuf_[CHUNK_]; // uninitialized cuz written before read
        SeekOffsetType _fSeekOffset{};
        optional<byte> _fNextOutputByte_; // 'cached' next output byte - if not nullopt - magic needed to make AvailableToRead
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;

        BaseRep_ (const Streams::InputStream::Ptr<byte>& in)
            : fInStream_{in}
        {
            Execution::ThrowIfNull (fCCtx_ = ::ZSTD_createCCtx ());
        }
        virtual ~BaseRep_ ()
        {
            if (fCCtx_ != nullptr) {
                ::ZSTD_freeCCtx (fCCtx_);
                fCCtx_ = nullptr;
            }
        }
        virtual bool IsSeekable () const override
        {
            return false; // SHOULD allow seekable IFF src is seekable, but tricky because of internal state in compress/decompress library - not sure how to update/manage
        }
        virtual void CloseRead () override
        {
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            if (fInStream_ != nullptr) {
                fInStream_.Close ();
            }
            Assert (fInStream_ == nullptr);
            Ensure (not IsOpenRead ());
        }
        virtual bool IsOpenRead () const override
        {
            AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
            return fInStream_ != nullptr;
        }
        virtual SeekOffsetType GetReadOffset () const override
        {
            Require (IsOpenRead ());
            AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
            return _fSeekOffset;
        }
        // return number of bytes definitely copied into intoBuffer, else nullopt on EWOULDBLOCK
        template <invocable<bool> PROCESS>
        optional<size_t> PullEnufForDeflate1Byte_ (NoDataAvailableHandling blockFlag, span<byte> intoBuffer, PROCESS processInputZLibFunction)
        {
#if 1
            AssertNotImplemented ();
            return nullopt;
#else
            Assert (_fNextOutputByte_ == nullopt); // already handled
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        Again:
            if (blockFlag == NoDataAvailableHandling::eDontBlock and fCCtx_.avail_in == 0 and fInStream_.AvailableToRead () == nullopt) {
                // if non-blocking call, no data pre-available in zstream, and nothing in upstream, NoDataAvailable!
                // note MAY not be enuf in zbuf to read a full byte of output, but OK - will come back here
                return nullopt;
            }
            if (fCCtx_.avail_in == 0) {
                Assert (std::size (fInBuf_) < numeric_limits<uInt>::max ());
                fCCtx_.avail_in = static_cast<uInt> (fInStream_.ReadBlocking (span{fInBuf_}).size ()); // blocking read always OK by the time we get here
                fCCtx_.next_in = reinterpret_cast<Bytef*> (begin (fInBuf_));
            }
            bool isAtSrcEOF = fCCtx_.avail_in == 0;

            ptrdiff_t outBufSize = intoBuffer.size ();

            fCCtx_.avail_out = static_cast<uInt> (outBufSize);
            fCCtx_.next_out  = reinterpret_cast<Bytef*> (intoBuffer.data ());
            int ret;
            switch (ret = processInputZLibFunction (isAtSrcEOF)) {
                case Z_OK:
                    break;
                case Z_STREAM_END:
                    break;
                default:
                    ThrowIfZLibErr_ (ret);
            }
            ptrdiff_t pulledOut = outBufSize - fCCtx_.avail_out;
            Assert (pulledOut <= outBufSize);
            if (pulledOut == 0 and not isAtSrcEOF) {
                goto Again;
            }
            return pulledOut;
#endif
        }
        template <invocable<bool> PROCESS>
        optional<size_t> _Available2Read (PROCESS processInputZLibFunction)
        {
            Require (IsOpenRead ());
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            if (_fNextOutputByte_) {
                return 1;
            }
            byte tmp;
            auto pulledButMustCache = PullEnufForDeflate1Byte_ (NoDataAvailableHandling::eDontBlock, span{&tmp, 1}, processInputZLibFunction);
            if (pulledButMustCache) {
                if (*pulledButMustCache == 0) {
                    return 0;
                }
                else {
                    Assert (*pulledButMustCache == 1);
                    _fNextOutputByte_ = tmp;
                    return 1;
                }
            }
            else {
                return nullopt;
            }
        }
        template <invocable<bool> PROCESS>
        optional<span<byte>> _Read (span<byte> intoBuffer, NoDataAvailableHandling blockFlag, PROCESS processInputZLibFunction)
        {
            Require (not intoBuffer.empty ()); // API rule for streams
            Require (IsOpenRead ());
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            if (_fNextOutputByte_) {
                intoBuffer[0] = *_fNextOutputByte_;
                _fNextOutputByte_.reset ();
                // OK to just return now, but see if we can be more efficient, and grab a bit more
                if (intoBuffer.size () > 1) {
                    if (auto o = PullEnufForDeflate1Byte_ (NoDataAvailableHandling::eDontBlock, intoBuffer.subspan (1), processInputZLibFunction)) {
                        size_t pulledOut = *o + 1;
                        _fSeekOffset += pulledOut;
                        return intoBuffer.subspan (0, pulledOut);
                    }
                }
                _fSeekOffset++;
                return intoBuffer.subspan (0, 1);
            }
            if (auto o = PullEnufForDeflate1Byte_ (blockFlag, intoBuffer, processInputZLibFunction)) {
                size_t pulledOut = *o;
                _fSeekOffset += pulledOut;
                return intoBuffer.subspan (0, pulledOut);
            }
            else {
                return nullopt;
            }
        }
    };
    struct CompressRep_ final : BaseRep_, Memory::UseBlockAllocationIfAppropriate<CompressRep_> {
        Compress::Options fOptions_;
        CompressRep_ (const Streams::InputStream::Ptr<byte>& in, Compress::Options o)
            : BaseRep_{in}
        {
            Require (not o.fCompressionLevel.has_value () or (0 <= o.fCompressionLevel and o.fCompressionLevel <= 1));
            if (o.fCompressionLevel.has_value ()) {
                ThrowIfZStdErr_ (::ZSTD_CCtx_setParameter (
                    fCCtx_, ZSTD_c_compressionLevel, static_cast<int> ((::ZSTD_maxCLevel () - ::ZSTD_minCLevel ()) * (*o.fCompressionLevel)) + ZSTD_minCLevel ()));
            }
            else {
                ThrowIfZStdErr_ (::ZSTD_CCtx_setParameter (fCCtx_, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT));
            }
            // from example - not sure if helpful
            ThrowIfZStdErr_ (::ZSTD_CCtx_setParameter (fCCtx_, ZSTD_c_checksumFlag, 1));
        }
        virtual optional<size_t> AvailableToRead () override
        {
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            return _Available2Read ([this] (bool isEOF) { return DoProcess_ (isEOF); });
        }
        virtual optional<SeekOffsetType> RemainingLength () override
        {
            return nullopt; // generally cannot tell without side-effects on input stream
        }
        virtual optional<span<byte>> Read (span<byte> intoBuffer, NoDataAvailableHandling blockFlag) override
        {
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            return _Read (intoBuffer, blockFlag, [this] (bool isEOF) { return DoProcess_ (isEOF); });
        }
        int DoProcess_ (bool isEOF)
        {
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
#if 0
            return ::deflate (&fCCtx_, isEOF ? Z_FINISH : Z_NO_FLUSH);
#endif
            return 0;
        }
    };
    struct DecompressRep_ final : BaseRep_, Memory::UseBlockAllocationIfAppropriate<DecompressRep_> {
        DecompressRep_ (const Streams::InputStream::Ptr<byte>& in)
            : BaseRep_{in}
        {
#if 0
            // see http://zlib.net/manual.html  for meaning of params and http://www.lemoda.net/c/zlib-open-read/ for example
            constexpr int windowBits       = 15;
            constexpr int ENABLE_ZLIB_GZIP = 32;
            ThrowIfZLibErr_ (::inflateInit2 (&fCCtx_, windowBits | (gzip ? ENABLE_ZLIB_GZIP : 0)));
#endif
        }
        virtual ~DecompressRep_ ()
        {
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
#if 0
            Verify (::inflateEnd (&fCCtx_) == Z_OK);
#endif
        }
        virtual optional<size_t> AvailableToRead () override
        {
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            return _Available2Read ([this] (bool isEOF) { return DoProcess_ (isEOF); });
        }
        virtual optional<SeekOffsetType> RemainingLength () override
        {
            return nullopt; // generally cannot tell without side-effects on input stream
        }
        virtual optional<span<byte>> Read (span<byte> intoBuffer, NoDataAvailableHandling blockFlag) override
        {
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            return _Read (intoBuffer, blockFlag, [this] (bool isEOF) { return DoProcess_ (isEOF); });
        }
        int DoProcess_ ([[maybe_unused]] bool isEOF)
        {
#if 0
            return ::inflate (&fCCtx_, Z_NO_FLUSH);
#endif
            return 0;
        }
    };
}
#endif

Compression::Ptr ZStd::Compress::New (const ZStd::Compress::Options& o)
{
#if qStroika_HasComponent_zstd
    struct MyRep_ : IRep, public Memory::UseBlockAllocationIfAppropriate<MyRep_> {
        ZStd::Compress::Options  fOptions_;
        shared_ptr<CompressRep_> fDelegate2;
        MyRep_ (const ZStd::Compress::Options& o)
            : fOptions_{o}
        {
        }
        virtual InputStream::Ptr<byte> Transform (const InputStream::Ptr<byte>& src)
        {
            fDelegate2 = make_shared<CompressRep_> (src, fOptions_);
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
Compression::Ptr ZStd::Decompress::New ([[maybe_unused]] const ZStd::Decompress::Options& o)
{
#if qStroika_HasComponent_zstd
    struct MyRep_ : IRep, public Memory::UseBlockAllocationIfAppropriate<MyRep_> {
        shared_ptr<DecompressRep_>     fDelegate2;
        virtual InputStream::Ptr<byte> Transform (const InputStream::Ptr<byte>& src)
        {
            fDelegate2 = make_shared<DecompressRep_> (src);
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
