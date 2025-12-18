/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Execution/FeatureNotSupportedException.h"
#include "Stroika/Foundation/Streams/StreamReader.h"

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
        // static constexpr size_t CHUNK_ = 16384;

    public:
        Streams::InputStream::Ptr<byte> fInStream_; // consider wrapping in StreamReader for efficiency sake - maybe unhelpful due to CHUNK logic below
        ZSTD_CCtx* fCCtx_{nullptr};
        // byte           fInBuf_[CHUNK_]; // uninitialized cuz written before read
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
        //         template <invocable<bool> PROCESS>
        //         optional<size_t> PullEnufForDeflate1Byte_ (NoDataAvailableHandling blockFlag, span<byte> intoBuffer, PROCESS processInputCompressionLibFunction)
        //         {
        // #if 1
        //             AssertNotImplemented ();
        //             return nullopt;
        // #else
        //             Assert (_fNextOutputByte_ == nullopt); // already handled
        //             AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        //         Again:
        //             if (blockFlag == NoDataAvailableHandling::eDontBlock and fCCtx_.avail_in == 0 and fInStream_.AvailableToRead () == nullopt) {
        //                 // if non-blocking call, no data pre-available in zstream, and nothing in upstream, NoDataAvailable!
        //                 // note MAY not be enuf in zbuf to read a full byte of output, but OK - will come back here
        //                 return nullopt;
        //             }
        //             if (fCCtx_.avail_in == 0) {
        //                 Assert (std::size (fInBuf_) < numeric_limits<uInt>::max ());
        //                 fCCtx_.avail_in = static_cast<uInt> (fInStream_.ReadBlocking (span{fInBuf_}).size ()); // blocking read always OK by the time we get here
        //                 fCCtx_.next_in = reinterpret_cast<Bytef*> (begin (fInBuf_));
        //             }
        //             bool isAtSrcEOF = fCCtx_.avail_in == 0;

        //             ptrdiff_t outBufSize = intoBuffer.size ();

        //             fCCtx_.avail_out = static_cast<uInt> (outBufSize);
        //             fCCtx_.next_out  = reinterpret_cast<Bytef*> (intoBuffer.data ());
        //             int ret;
        //             switch (ret = processInputCompressionLibFunction (isAtSrcEOF)) {
        //                 case Z_OK:
        //                     break;
        //                 case Z_STREAM_END:
        //                     break;
        //                 default:
        //                     ThrowIfZLibErr_ (ret);
        //             }
        //             ptrdiff_t pulledOut = outBufSize - fCCtx_.avail_out;
        //             Assert (pulledOut <= outBufSize);
        //             if (pulledOut == 0 and not isAtSrcEOF) {
        //                 goto Again;
        //             }
        //             return pulledOut;
        // #endif
        //         }
        // template <invocable<bool> PROCESS>
        // optional<size_t> _Available2Read (PROCESS processInputCompressionLibFunction)
        // {
        //     Require (IsOpenRead ());
        //     AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        //     if (_fNextOutputByte_) {
        //         return 1;
        //     }
        //     byte tmp;
        //     auto pulledButMustCache = PullEnufForDeflate1Byte_ (NoDataAvailableHandling::eDontBlock, span{&tmp, 1}, processInputCompressionLibFunction);
        //     if (pulledButMustCache) {
        //         if (*pulledButMustCache == 0) {
        //             return 0;
        //         }
        //         else {
        //             Assert (*pulledButMustCache == 1);
        //             _fNextOutputByte_ = tmp;
        //             return 1;
        //         }
        //     }
        //     else {
        //         return nullopt;
        //     }
        // }
        // template <invocable<bool> PROCESS>
        // optional<span<byte>> _Read (span<byte> intoBuffer, NoDataAvailableHandling blockFlag, PROCESS processInputCompressionLibFunction)
        // {
        //     Require (not intoBuffer.empty ()); // API rule for streams
        //     Require (IsOpenRead ());
        //     AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        //     if (_fNextOutputByte_) {
        //         intoBuffer[0] = *_fNextOutputByte_;
        //         _fNextOutputByte_.reset ();
        //         // OK to just return now, but see if we can be more efficient, and grab a bit more
        //         if (intoBuffer.size () > 1) {
        //             if (auto o = PullEnufForDeflate1Byte_ (NoDataAvailableHandling::eDontBlock, intoBuffer.subspan (1), processInputCompressionLibFunction)) {
        //                 size_t pulledOut = *o + 1;
        //                 _fSeekOffset += pulledOut;
        //                 return intoBuffer.subspan (0, pulledOut);
        //             }
        //         }
        //         _fSeekOffset++;
        //         return intoBuffer.subspan (0, 1);
        //     }
        //     if (auto o = PullEnufForDeflate1Byte_ (blockFlag, intoBuffer, processInputCompressionLibFunction)) {
        //         size_t pulledOut = *o;
        //         _fSeekOffset += pulledOut;
        //         return intoBuffer.subspan (0, pulledOut);
        //     }
        //     else {
        //         return nullopt;
        //     }
        // }
    };
    struct CompressRep_ final : InputStream::IRep<byte>, Memory::UseBlockAllocationIfAppropriate<CompressRep_> {
        unique_ptr<Streams::StreamReader<byte>> fInStreamReader_;                               // wrapped/buffered provided input stream
        Memory::InlineBuffer<byte> fInputBuf_{Memory::eUninitialized, ::ZSTD_CStreamInSize ()}; // used to cache extra input (uncompressed) bytes not yet proceessed
        span<byte> fInputBufCache_{};                                                           /// empty or subspan of fInputBuf_
        Memory::InlineBuffer<byte> fOutBuf_{Memory::eUninitialized, ::ZSTD_CStreamOutSize ()}; // used to cache extra output (compressed) bytes not yet returned (NOTE - CStreamOutSize maybe wrong to use here)
        span<byte>                                                     fOutputBufCache_{}; /// empty or subspan of fOutBuf_
        ZSTD_CCtx*                                                     fCCtx_{nullptr};
        SeekOffsetType                                                 fSeekOffset_{};
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;

        CompressRep_ (const Streams::InputStream::Ptr<byte>& in, Compress::Options o)
            : fInStreamReader_{make_unique<Streams::StreamReader<byte>> (in)}
        {
            Require (not o.fCompressionLevel.has_value () or (0 <= o.fCompressionLevel and o.fCompressionLevel <= 1));

            Execution::ThrowIfNull (fCCtx_ = ::ZSTD_createCCtx ());
            if (o.fCompressionLevel.has_value ()) {
                ThrowIfZStdErr_ (::ZSTD_CCtx_setParameter (
                    fCCtx_, ZSTD_c_compressionLevel,
                    static_cast<int> ((::ZSTD_maxCLevel () - ::ZSTD_minCLevel ()) * (*o.fCompressionLevel)) + ::ZSTD_minCLevel ()));
            }
            else {
                ThrowIfZStdErr_ (::ZSTD_CCtx_setParameter (fCCtx_, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT));
            }
            // from example - not sure if helpful
            ThrowIfZStdErr_ (::ZSTD_CCtx_setParameter (fCCtx_, ZSTD_c_checksumFlag, 1));
        }
        virtual ~CompressRep_ ()
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
            fInStreamReader_ = nullptr;
            Ensure (not IsOpenRead ());
        }
        virtual bool IsOpenRead () const override
        {
            AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
            return fInStreamReader_ != nullptr;
        }
        virtual SeekOffsetType GetReadOffset () const override
        {
            Require (IsOpenRead ());
            AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
            return fSeekOffset_;
        }
        virtual optional<size_t> AvailableToRead () override
        {
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
#if 1
            AssertNotImplemented ();
            return nullopt;
#else
            return _Available2Read ([this] (bool isEOF) { return DoProcess_ (isEOF); });
#endif
        }
        virtual optional<SeekOffsetType> RemainingLength () override
        {
            return nullopt; // generally cannot tell without side-effects on input stream
        }
        virtual optional<span<byte>> Read (span<byte> intoBuffer, NoDataAvailableHandling blockFlag) override
        {
            Require (not intoBuffer.empty ()); // API rule for streams

            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};

            /*
             * must read enough bytes from inputStream (possible some unconverted bytes left over to re-use), to satisify the intoBuffer
             * read request. This might produce more compressed output than fits in intoBuffer - in which case we must cache the balance
             */

            // See if request can be satisfied from cached output bytes; note we only NEED to return one byte (but can return more)
            if (not fOutputBufCache_.empty ()) {
                size_t nToCopy   = min (intoBuffer.size (), fOutputBufCache_.size ());
                auto   r         = Memory::CopySpanData (fOutputBufCache_.subspan (0, nToCopy), intoBuffer);
                fOutputBufCache_ = fOutputBufCache_.subspan (nToCopy); // skip returned bytes
                fSeekOffset_ += nToCopy;
                return r;
            }

            // Combine existing fInputBuf_ cached data with a bit more we try to read, so we pass as big a chunk as possible to ZStd lib
            // Read argument windows into fInputBuf_, just after any bytes already read
            if (optional<span<byte>> n = fInStreamReader_->Read (span{fInputBuf_}.subspan (fInputBufCache_.size ()), blockFlag)) {
                fInputBufCache_ = fInputBufCache_.first (fInputBufCache_.size () + n->size ());
            }

            // Now if we have any input bytes to compress, run it through the library
            if (not fInputBufCache_.empty ()) {
                ZSTD_inBuffer  input     = {fInputBufCache_.data (), fInputBufCache_.size (), 0};
                ZSTD_outBuffer output    = {intoBuffer.data (), intoBuffer.size (), 0};
                size_t const   remaining = ZSTD_compressStream2 (fCCtx_, &output, &input, ZSTD_e_continue);
                ThrowIfZStdErr_ (remaining);

                // if anything produced, adjust cache(s) and return it
                if (output.pos > 0) {
                    fInputBufCache_ = fInputBufCache_.subspan (input.pos);
                    fSeekOffset_ += output.pos;
                    return intoBuffer.subspan (0, output.pos);
                }

                // NEED to check if failed cuz output buffer too small, and try larger output buffer (our cache)

                // and must handle the DONE case - where input is exhausted
            }

            // EOF or EWOULDBLOCK
            if (blockFlag == NoDataAvailableHandling::eDontBlock) {
                return nullopt;
            }

            // see how much uncompressed input we already have cached before pulling more from the input stream

            // for first cuz - KISS - and don't combine input bytes sources

            /// BELOW CODE WRONG - JUST INCOMPLETE - DRAFT

            if (not fInputBufCache_.empty ()) {
                ZSTD_inBuffer  input     = {fInputBufCache_.data (), fInputBufCache_.size_bytes (), 0};
                ZSTD_outBuffer output    = {intoBuffer.data (), intoBuffer.size (), 0};
                size_t const   remaining = ::ZSTD_compressStream2 (fCCtx_, &output, &input, ZSTD_e_continue);
                ThrowIfZStdErr_ (remaining);

                // if anything produced, adjust cache(s) and return it (fancier impl might combine this with read but for now KISS)
                if (output.pos > 0) {
                    fInputBufCache_ = fInputBufCache_.subspan (input.pos);
                    fSeekOffset_ += output.pos;
                    return intoBuffer.subspan (0, output.pos);
                }
            }

            // Now retry above logic (so maybe just do this first)

            // adjust fInputBufCache_ to include any newly read input bytes

            // WRITE TO fOutBuf_

            int const               lastChunk = false; //???
            ZSTD_EndDirective const mode      = lastChunk ? ZSTD_e_end : ZSTD_e_continue;

            // WRONG - INPUT IS FROM IN STREAM (and memory copy of said)&&&&

            ZSTD_inBuffer             input = {intoBuffer.data (), intoBuffer.size_bytes (), 0};
            Memory::StackBuffer<byte> outBuf{::ZSTD_CStreamOutSize ()};
            int                       finished;
            do {
                /* Compress into the output buffer and write all of the output to
             * the file so we can reuse the buffer next iteration.
             */
                ZSTD_outBuffer output    = {outBuf.data (), outBuf.size (), 0};
                size_t const   remaining = ZSTD_compressStream2 (fCCtx_, &output, &input, mode);
                ThrowIfZStdErr_ (remaining);
                // fwrite_orDie (buffOut, output.pos, fout);

                // COPY INTOBUFFER

                /* If we're on the last chunk we're finished when zstd returns 0,
             * which means its consumed all the input AND finished the frame.
             * Otherwise, we're finished when we've consumed all the input.
             */
                finished = lastChunk ? (remaining == 0) : (input.pos == input.size);
            } while (!finished);
            Assert (input.pos == input.size);

            AssertNotImplemented ();
            return nullopt;
        }

        //         // return number of bytes definitely copied into intoBuffer, else nullopt on EWOULDBLOCK
        //         template <invocable<bool> PROCESS>
        //         optional<size_t> PullEnufForDeflate1Byte_ (NoDataAvailableHandling blockFlag, span<byte> intoBuffer, PROCESS processInputCompressionLibFunction)
        //         {
        // #if 1
        //             AssertNotImplemented ();
        //             return nullopt;
        // #else
        //             Assert (_fNextOutputByte_ == nullopt); // already handled
        //             AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        //         Again:
        //             if (blockFlag == NoDataAvailableHandling::eDontBlock and fCCtx_.avail_in == 0 and fInStream_.AvailableToRead () == nullopt) {
        //                 // if non-blocking call, no data pre-available in zstream, and nothing in upstream, NoDataAvailable!
        //                 // note MAY not be enuf in zbuf to read a full byte of output, but OK - will come back here
        //                 return nullopt;
        //             }
        //             if (fCCtx_.avail_in == 0) {
        //                 Assert (std::size (fInBuf_) < numeric_limits<uInt>::max ());
        //                 fCCtx_.avail_in = static_cast<uInt> (fInStream_.ReadBlocking (span{fInBuf_}).size ()); // blocking read always OK by the time we get here
        //                 fCCtx_.next_in = reinterpret_cast<Bytef*> (begin (fInBuf_));
        //             }
        //             bool isAtSrcEOF = fCCtx_.avail_in == 0;

        //             ptrdiff_t outBufSize = intoBuffer.size ();

        //             fCCtx_.avail_out = static_cast<uInt> (outBufSize);
        //             fCCtx_.next_out  = reinterpret_cast<Bytef*> (intoBuffer.data ());
        //             int ret;
        //             switch (ret = processInputCompressionLibFunction (isAtSrcEOF)) {
        //                 case Z_OK:
        //                     break;
        //                 case Z_STREAM_END:
        //                     break;
        //                 default:
        //                     ThrowIfZLibErr_ (ret);
        //             }
        //             ptrdiff_t pulledOut = outBufSize - fCCtx_.avail_out;
        //             Assert (pulledOut <= outBufSize);
        //             if (pulledOut == 0 and not isAtSrcEOF) {
        //                 goto Again;
        //             }
        //             return pulledOut;
        // #endif
        //         }
        // template <invocable<bool> PROCESS>
        // optional<size_t> _Available2Read (PROCESS processInputCompressionLibFunction)
        // {
        //     Require (IsOpenRead ());
        //     AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        //     if (_fNextOutputByte_) {
        //         return 1;
        //     }
        //     byte tmp;
        //     auto pulledButMustCache = PullEnufForDeflate1Byte_ (NoDataAvailableHandling::eDontBlock, span{&tmp, 1}, processInputCompressionLibFunction);
        //     if (pulledButMustCache) {
        //         if (*pulledButMustCache == 0) {
        //             return 0;
        //         }
        //         else {
        //             Assert (*pulledButMustCache == 1);
        //             _fNextOutputByte_ = tmp;
        //             return 1;
        //         }
        //     }
        //     else {
        //         return nullopt;
        //     }
        // }
        // template <invocable<bool> PROCESS>
        // optional<span<byte>> _Read (span<byte> intoBuffer, NoDataAvailableHandling blockFlag, PROCESS processInputCompressionLibFunction)
        // {
        //     Require (not intoBuffer.empty ()); // API rule for streams
        //     Require (IsOpenRead ());
        //     AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        //     if (_fNextOutputByte_) {
        //         intoBuffer[0] = *_fNextOutputByte_;
        //         _fNextOutputByte_.reset ();
        //         // OK to just return now, but see if we can be more efficient, and grab a bit more
        //         if (intoBuffer.size () > 1) {
        //             if (auto o = PullEnufForDeflate1Byte_ (NoDataAvailableHandling::eDontBlock, intoBuffer.subspan (1), processInputCompressionLibFunction)) {
        //                 size_t pulledOut = *o + 1;
        //                 fSeekOffset_ += pulledOut;
        //                 return intoBuffer.subspan (0, pulledOut);
        //             }
        //         }
        //         fSeekOffset_++;
        //         return intoBuffer.subspan (0, 1);
        //     }
        //     if (auto o = PullEnufForDeflate1Byte_ (blockFlag, intoBuffer, processInputCompressionLibFunction)) {
        //         size_t pulledOut = *o;
        //         fSeekOffset_ += pulledOut;
        //         return intoBuffer.subspan (0, pulledOut);
        //     }
        //     else {
        //         return nullopt;
        //     }
        // }

        //         int DoProcess_ (bool isEOF)
        //         {
        //             AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        // #if 0
        //             return ::deflate (&fCCtx_, isEOF ? Z_FINISH : Z_NO_FLUSH);
        // #endif
        //             return 0;
        //         }
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
            // return _Available2Read ([this] (bool isEOF) { return DoProcess_ (isEOF); });
            return nullopt;
        }
        virtual optional<SeekOffsetType> RemainingLength () override
        {
            return nullopt; // generally cannot tell without side-effects on input stream
        }
        virtual optional<span<byte>> Read (span<byte> intoBuffer, NoDataAvailableHandling blockFlag) override
        {
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            // return _Read (intoBuffer, blockFlag, [this] (bool isEOF) { return DoProcess_ (isEOF); });
            return nullopt;
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
