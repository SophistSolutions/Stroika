/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Execution/FeatureNotSupportedException.h"

#include "Stroika/Foundation/DataExchange/Compression/Common.h"

#if qHasFeature_ZLib
// SEE http://www.zlib.net/zlib_how.html
#include <zlib.h>
#endif

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::Compression;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Streams;

#if qHasFeature_ZLib
namespace Stroika::Foundation::DataExchange::Compression::Private_ {
    inline void ThrowIfZLibErr_ (int err)
    {
        // VERY ROUGH DRAFT - probably need a more specific exception object type
        if (err != Z_OK) [[unlikely]] {
            switch (err) {
                case Z_VERSION_ERROR: {
                    static const Execution::RuntimeErrorException kException_{"ZLIB Z_VERSION_ERROR"sv};
                    Execution::Throw (kException_);
                }
                case Z_DATA_ERROR: {
                    static const Execution::RuntimeErrorException kException_{"ZLIB Z_DATA_ERROR"sv};
                    Execution::Throw (kException_);
                }
                case Z_STREAM_ERROR: {
                    static const Execution::RuntimeErrorException kException_{"ZLIB Z_STREAM_ERROR"sv};
                    Execution::Throw (kException_);
                }
                case Z_ERRNO:
                    Execution::Throw (Execution::RuntimeErrorException{Characters::Format ("ZLIB Z_ERRNO (errno={})"_f, errno)});
                default:
                    Execution::Throw (Execution::RuntimeErrorException{Characters::Format ("ZLIB ERR {}"_f, err)});
            }
        }
    }

    struct BaseRep_ : public InputStream::IRep<byte> {
    private:
        static constexpr size_t CHUNK_ = 16384;

    public:
        Streams::InputStream::Ptr<byte> fInStream_; // consider wrapping in StreamReader for efficiency sake - maybe unhelpful due to CHUNK logic below
        z_stream       fZStream_{};
        byte           fInBuf_[CHUNK_]; // uninitialized cuz written before read
        SeekOffsetType _fSeekOffset{};
        optional<byte> _fNextOutputByte_; // 'cached' next output byte - if not nullopt - magic needed to make AvailableToRead
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;

        BaseRep_ (const Streams::InputStream::Ptr<byte>& in)
            : fInStream_{in}
        {
        }
        virtual ~BaseRep_ () = default;
        virtual bool IsSeekable () const override
        {
            return false; // SHOULD allow seekable IFF src is seekable, but tricky because of internal state in compress/decompress library - not sure how to update/manage
        }
        virtual void CloseRead () override
        {
            Require (IsOpenRead ());
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            fInStream_.Close ();
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
            Assert (_fNextOutputByte_ == nullopt); // already handled
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        Again:
            if (blockFlag == NoDataAvailableHandling::eDontBlock and fZStream_.avail_in == 0 and fInStream_.AvailableToRead () == nullopt) {
                // if non-blocking call, no data pre-available in zstream, and nothing in upstream, NoDataAvailable!
                // note MAY not be enuf in zbuf to read a full byte of output, but OK - will come back here
                return nullopt;
            }
            if (fZStream_.avail_in == 0) {
                Assert (Memory::NEltsOf (fInBuf_) < numeric_limits<uInt>::max ());
                fZStream_.avail_in = static_cast<uInt> (fInStream_.Read (span{fInBuf_}).size ()); // blocking read always OK by the time we get here
                fZStream_.next_in = reinterpret_cast<Bytef*> (begin (fInBuf_));
            }
            bool isAtSrcEOF = fZStream_.avail_in == 0;

            ptrdiff_t outBufSize = intoBuffer.size ();

            fZStream_.avail_out = static_cast<uInt> (outBufSize);
            fZStream_.next_out  = reinterpret_cast<Bytef*> (intoBuffer.data ());
            int ret;
            switch (ret = processInputZLibFunction (isAtSrcEOF)) {
                case Z_OK:
                    break;
                case Z_STREAM_END:
                    break;
                default:
                    ThrowIfZLibErr_ (ret);
            }
            ptrdiff_t pulledOut = outBufSize - fZStream_.avail_out;
            Assert (pulledOut <= outBufSize);
            if (pulledOut == 0 and not isAtSrcEOF) {
                goto Again;
            }
            return pulledOut;
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
    struct DeflateRep_ : BaseRep_ {
        Compress::Options fOptions_;
        DeflateRep_ (const Streams::InputStream::Ptr<byte>& in, Compress::Options o, bool useGZip)
            : BaseRep_{in}
            , fOptions_{o}
        {
            Require (not o.fCompressionLevel.has_value () or (0 <= o.fCompressionLevel and o.fCompressionLevel <= 1));
            int level = o.fCompressionLevel ? Z_BEST_SPEED + Math::Round<int> (*o.fCompressionLevel * (Z_BEST_COMPRESSION - Z_BEST_SPEED))
                                            : Z_DEFAULT_COMPRESSION;
            Assert (level == Z_DEFAULT_COMPRESSION or (Z_BEST_SPEED <= level and level <= Z_BEST_COMPRESSION));
            if (useGZip) {
                constexpr int kWindowBits = 15 + 16; // 16 => gzip - https://zlib.net/manual.html#Advanced
                constexpr int kMemLevel   = 8;       // default
                ThrowIfZLibErr_ (::deflateInit2 (&fZStream_, level, Z_DEFLATED, kWindowBits, kMemLevel, Z_DEFAULT_STRATEGY));
            }
            else {
                ThrowIfZLibErr_ (::deflateInit (&fZStream_, level));
            }
        }
        virtual ~DeflateRep_ ()
        {
            Verify (::deflateEnd (&fZStream_) == Z_OK);
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
            return ::deflate (&fZStream_, isEOF ? Z_FINISH : Z_NO_FLUSH);
        }
    };
    struct InflateRep_ : BaseRep_ {
        InflateRep_ (const Streams::InputStream::Ptr<byte>& in, bool gzip)
            : BaseRep_{in}
        {
            // see http://zlib.net/manual.html  for meaning of params and http://www.lemoda.net/c/zlib-open-read/ for example
            constexpr int windowBits       = 15;
            constexpr int ENABLE_ZLIB_GZIP = 32;
            ThrowIfZLibErr_ (::inflateInit2 (&fZStream_, windowBits | (gzip ? ENABLE_ZLIB_GZIP : 0)));
        }
        virtual ~InflateRep_ ()
        {
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            Verify (::inflateEnd (&fZStream_) == Z_OK);
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
            return ::inflate (&fZStream_, Z_NO_FLUSH);
        }
    };
}
#endif
