/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"

#include "Deflate.h"

#if qHasFeature_ZLib
// SEE http://www.zlib.net/zlib_how.html
#include <zlib.h>

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::Compression;
using namespace Stroika::Foundation::Streams;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

namespace {
    void ThrowIfZLibErr_ (int err)
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
            fInStream_.Close ();
            Assert (fInStream_ == nullptr);
            Ensure (not IsOpenRead ());
        }
        virtual bool IsOpenRead () const override
        {
            return fInStream_ != nullptr;
        }
        virtual SeekOffsetType GetReadOffset () const override
        {
            Require (IsOpenRead ());
            return _fSeekOffset;
        }
        // return number of bytes definitely copied into intoBuffer, else nullopt on EWOULDBLOCK
        template <invocable<bool> PROCESS>
        optional<size_t> PullEnufForDeflate1Byte_ (NoDataAvailableHandling blockFlag, span<byte> intoBuffer, PROCESS processInputZLibFunction)
        {
            Assert (_fNextOutputByte_ == nullopt); // already handled
        Again:
            if (blockFlag == NoDataAvailableHandling::eDontBlock and fZStream_.avail_in == 0 and fInStream_.AvailableToRead () == nullopt) {
                // if non-blocking call, no data pre-available in zstream, and nothing in upstream, NoDataAvailable!
                // note MAY not be enuf in zbuf to read a full byte of output, but OK - will come back here
                return false;
            }
            if (fZStream_.avail_in == 0) {
                Assert (Memory::NEltsOf (fInBuf_) < numeric_limits<uInt>::max ());
                fZStream_.avail_in = static_cast<uInt> (fInStream_.Read (span{fInBuf_}).size ()); // blocking read always OK by the time we get here
                fZStream_.next_in = reinterpret_cast<Bytef*> (begin (fInBuf_));
            }
            bool isAtSrcEOF = fZStream_.avail_in == 0;

            ptrdiff_t outBufSize = intoBuffer.size ();

            // int flush = isAtSrcEOF ? Z_FINISH : Z_NO_FLUSH;

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
            if (_fNextOutputByte_) {
                intoBuffer[0] = *_fNextOutputByte_;
                _fNextOutputByte_.reset ();
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
        DeflateRep_ (const Streams::InputStream::Ptr<byte>& in)
            : BaseRep_{in}
        {
            int level = Z_DEFAULT_COMPRESSION;
            ThrowIfZLibErr_ (::deflateInit (&fZStream_, level));
        }
        virtual ~DeflateRep_ ()
        {
            Verify (::deflateEnd (&fZStream_) == Z_OK);
        }
        virtual optional<size_t> AvailableToRead () override
        {
            return _Available2Read ([this] (bool isEOF) { return DoProcess_ (isEOF); });
        }
        virtual optional<SeekOffsetType> RemainingLength () override
        {
            return nullopt; // generally cannot tell without side-effects on input stream
        }
        virtual optional<span<byte>> Read (span<byte> intoBuffer, NoDataAvailableHandling blockFlag) override
        {
            return _Read (intoBuffer, blockFlag, [this] (bool isEOF) { return DoProcess_ (isEOF); });
        }
        int DoProcess_ (bool isEOF)
        {
            return ::deflate (&fZStream_, isEOF ? Z_FINISH : Z_NO_FLUSH);
        }
    };
    struct InflateRep_ : BaseRep_ {
        InflateRep_ (const Streams::InputStream::Ptr<byte>& in)
            : BaseRep_{in}
        {
            // see http://zlib.net/manual.html  for meaning of params and http://www.lemoda.net/c/zlib-open-read/ for example
            constexpr int windowBits       = 15;
            constexpr int ENABLE_ZLIB_GZIP = 32;
            ThrowIfZLibErr_ (::inflateInit2 (&fZStream_, windowBits | ENABLE_ZLIB_GZIP));
        }
        virtual ~InflateRep_ ()
        {
            Verify (::inflateEnd (&fZStream_) == Z_OK);
        }
        virtual optional<size_t> AvailableToRead () override
        {
            return _Available2Read ([this] (bool isEOF) { return DoProcess_ (isEOF); });
        }
        virtual optional<SeekOffsetType> RemainingLength () override
        {
            return nullopt; // generally cannot tell without side-effects on input stream
        }
        virtual optional<span<byte>> Read (span<byte> intoBuffer, NoDataAvailableHandling blockFlag) override
        {
            return _Read (intoBuffer, blockFlag, [this] (bool isEOF) { return DoProcess_ (isEOF); });
        }
        int DoProcess_ ([[maybe_unused]] bool isEOF)
        {
            return ::inflate (&fZStream_, Z_NO_FLUSH);
        }
    };
}
#endif

#if qHasFeature_ZLib
Compression::Ptr Deflate::Compress::New (const Deflate::Compress::Options& o)
{
    struct MyRep_ : IRep {
        shared_ptr<DeflateRep_>        fDelegate2;
        virtual InputStream::Ptr<byte> Transform (const InputStream::Ptr<byte>& src)
        {
            fDelegate2 = make_shared<DeflateRep_> (src);
            return InputStream::Ptr<byte>{fDelegate2};
        }
        virtual optional<Compression::Stats> GetStats () const
        {
            return nullopt;
        }
    };
    return Compression::Ptr{make_shared<MyRep_> ()};
}
Compression::Ptr Deflate::Decompress::New (const Deflate::Decompress::Options& o)
{
    struct MyRep_ : IRep {
        shared_ptr<InflateRep_>        fDelegate2;
        virtual InputStream::Ptr<byte> Transform (const InputStream::Ptr<byte>& src)
        {
            fDelegate2 = make_shared<InflateRep_> (src);
            return InputStream::Ptr<byte>{fDelegate2};
        }
        virtual optional<Compression::Stats> GetStats () const
        {
            return nullopt;
        }
    };
    return Compression::Ptr{make_shared<MyRep_> ()};
}
#endif
