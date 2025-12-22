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
    inline void ThrowIfZStdErr_ (size_t rc) // CHECK_ZSTD
    {
        if (ZSTD_isError (rc)) {
            Execution::Throw (Execution::RuntimeErrorException{"ZStd error: {}"_f(String::FromNarrowSDKString (::ZSTD_getErrorName (rc)))});
        }
    }

    struct CompressingByteStreamRep_ final : InputStream::IRep<byte>, Memory::UseBlockAllocationIfAppropriate<CompressingByteStreamRep_> {
    private:
        unique_ptr<Streams::StreamReader<byte>> fInStreamReader_;                               // wrapped/buffered provided input stream
        Memory::InlineBuffer<byte> fInputBuf_{Memory::eUninitialized, ::ZSTD_CStreamInSize ()}; // used to cache extra input (uncompressed) bytes not yet proceessed
        span<byte> fRawUnprocessedInputBytes_{};                                                // empty or subspan of fInputBuf_
        Memory::InlineBuffer<byte> fOutBuf_{Memory::eUninitialized, ::ZSTD_CStreamOutSize ()}; // used to cache extra output (compressed) bytes not yet returned (NOTE - CStreamOutSize maybe wrong to use here)
        span<byte> fOutputBufCache_{}; // empty or subspan of fOutBuf_
        ZSTD_CCtx* fCtx_{nullptr};
        enum class Stage_ {
            eReadingInput,
            eEndOutput,
            eDone
        } fStage_{Stage_::eReadingInput};
        SeekOffsetType                                                 fSeekOffset_{};
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;

    private:
        struct CompressResult_ {
            size_t fConsumedInputBytes_{};
            size_t fProducedOutputBytes_{};
            size_t fRemaining;
        };
        static CompressResult_ LowLevelCompress_ (ZSTD_CCtx* ctx, span<const byte> fromBytes, ZSTD_EndDirective endFlag, span<byte> intoCompressedBytes)
        {
            ZSTD_inBuffer  input     = {fromBytes.data (), fromBytes.size (), 0};
            ZSTD_outBuffer output    = {intoCompressedBytes.data (), intoCompressedBytes.size (), 0};
            size_t const   remaining = ::ZSTD_compressStream2 (ctx, &output, &input, endFlag);
            ThrowIfZStdErr_ (remaining);
            return CompressResult_{.fConsumedInputBytes_ = input.pos, .fProducedOutputBytes_ = output.pos, .fRemaining = remaining};
        }

    public:
        CompressingByteStreamRep_ (const Streams::InputStream::Ptr<byte>& in, Compress::Options o)
            : fInStreamReader_{make_unique<Streams::StreamReader<byte>> (in)}
        {
            Require (not o.fCompressionLevel.has_value () or (0 <= o.fCompressionLevel and o.fCompressionLevel <= 1));

            Execution::ThrowIfNull (fCtx_ = ::ZSTD_createCCtx ());
            if (o.fCompressionLevel.has_value ()) {
                // The library supports regular compression levels from 1 up to ZSTD_maxCLevel()
                ThrowIfZStdErr_ (::ZSTD_CCtx_setParameter (fCtx_, ZSTD_c_compressionLevel,
                                                           static_cast<int> ((::ZSTD_maxCLevel () - 1 + 1) * (*o.fCompressionLevel)) + 1));
            }
            else {
                ThrowIfZStdErr_ (::ZSTD_CCtx_setParameter (fCtx_, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT));
            }
            // from example - not sure if helpful
            ThrowIfZStdErr_ (::ZSTD_CCtx_setParameter (fCtx_, ZSTD_c_checksumFlag, 1));
        }
        virtual ~CompressingByteStreamRep_ ()
        {
            if (fCtx_ != nullptr) {
                ::ZSTD_freeCCtx (fCtx_);
                fCtx_ = nullptr;
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
            if (fOutputBufCache_.size () != 0) {
                return fOutputBufCache_.size ();
            }

            // tricky - cuz we don't want to block

            // Combine existing fInputBuf_ cached data with a bit more we try to read, so we pass as big a chunk as possible to ZStd lib
            // Read argument windows into fInputBuf_, just after any bytes already read
            if (optional<span<byte>> n =
                    fInStreamReader_->Read (span{fInputBuf_}.subspan (fRawUnprocessedInputBytes_.size ()), NoDataAvailableHandling::eDontBlock)) {
                fRawUnprocessedInputBytes_ = span{fInputBuf_}.first (fRawUnprocessedInputBytes_.size () + n->size ());
            }

            // if we have any data in fRawUnprocessedInputBytes_, translate it and copy it into fOutputBufCache_.
            // return sizes of know available data.
            ZSTD_inBuffer input = {fRawUnprocessedInputBytes_.data (), fRawUnprocessedInputBytes_.size (), 0};
            Assert (fOutputBufCache_.size ()); // so overwrite whole buffer
            ZSTD_outBuffer output = {fOutBuf_.data (), fOutBuf_.size (), 0};
            // @todo consider checking EOF of input? and passing different endflag to compress?
            size_t const remaining = ::ZSTD_compressStream2 (fCtx_, &output, &input, ZSTD_e_continue);
            ThrowIfZStdErr_ (remaining);

            // if anything produced cache it
            if (output.pos > 0) {
                // remove consumed input bytes
                fRawUnprocessedInputBytes_ = fRawUnprocessedInputBytes_.subspan (input.pos);
                // extend the fOutputBufCache_
                Assert (fOutputBufCache_.size () == 0);
                fOutputBufCache_ = span{fOutBuf_}.first (output.pos);
                // but dont update seek offset, cuz we didn't actually read anything
            }
            if (fOutputBufCache_.size () != 0) {
                return fOutputBufCache_.size ();
            }
            return nullopt;
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
                Assert (fStage_ != Stage_::eDone);
                size_t nToCopy = min (intoBuffer.size (), fOutputBufCache_.size ());
                auto r = Memory::CopySpanData (fOutputBufCache_.subspan (0, nToCopy), intoBuffer); // intoBuffer large enuf cuz we pinned size with nToCopy
                fOutputBufCache_ = fOutputBufCache_.subspan (nToCopy);                             // skip returned bytes
                fSeekOffset_ += nToCopy;
                return r;
            }

            // first read from the input stream, and accumulate until EOF on input stream (using ZSTD_e_continue or ZSTD_e_flush) - (Process chunks)
            switch (fStage_) {
                // TODO
                // Latency vs. Ratio: If you need data to be available immediately
                //  (e.g., for real-time network packets), use ZSTD_e_flush instead of ZSTD_e_continue,
                //  though this may slightly reduce your compression ratio.
                case Stage_::eReadingInput: {
                    while (true) {
                        // Combine existing fInputBuf_ cached data with a bit more we try to read, so we pass as big a chunk as possible to ZStd lib
                        // Read argument windows into fInputBuf_, just after any bytes already read
                        if (fRawUnprocessedInputBytes_.size () < fInputBuf_.size ()) {
                            if (optional<span<byte>> n = fInStreamReader_->Read (span{fInputBuf_}.subspan (fRawUnprocessedInputBytes_.size ()), blockFlag);
                                n and not n->empty ()) {
                                fRawUnprocessedInputBytes_ = span{fInputBuf_}.first (fRawUnprocessedInputBytes_.size () + n->size ());
                            }
                        }

                        Assert (fOutputBufCache_.empty ());
                        if (fRawUnprocessedInputBytes_.empty ()) {
                            // If nothing available to compress, either input at EOF, or must return nullopt and wait for more
                            if (optional<bool> isAtEOF = fInStreamReader_->IsAtEOF (blockFlag); isAtEOF and *isAtEOF) {
                                fStage_ = Stage_::eEndOutput; // if instream reader definitely at EOF, then fRawUnprocessedInputBytes_ contains all that is left
                                goto xxx;
                            }
                            else {
                                Assert (blockFlag == NoDataAvailableHandling::eDontBlock); // else we would have blocked getting at least one byte
                                return nullopt;
                            }
                        }
                        else {
                            // Now if we have any input bytes to compress (or at EOF, and may need to write more stuff at end), run it through the library
                            CompressResult_ compressResults = LowLevelCompress_ (fCtx_, fRawUnprocessedInputBytes_, ZSTD_e_continue, span{fOutBuf_});
                            fRawUnprocessedInputBytes_ = fRawUnprocessedInputBytes_.subspan (compressResults.fConsumedInputBytes_);

                            // if anything produced, adjust cache(s) and return it
                            if (compressResults.fProducedOutputBytes_ > 0) {
                                // cache excess output bytes, and return those that will fit
                                size_t nToCopy = min (intoBuffer.size (), compressResults.fProducedOutputBytes_);
                                auto   r       = Memory::CopySpanData (span{fOutBuf_}.subspan (0, nToCopy), intoBuffer);
                                fOutputBufCache_ = span{fOutBuf_}.subspan (nToCopy, compressResults.fProducedOutputBytes_ - nToCopy); // skip returned bytes
                                fSeekOffset_ += nToCopy;
                                return r;
                            }
                            else {
                                Assert (compressResults.fConsumedInputBytes_ > 0); // keep going - making progress
                            }
                        }
                    }
                    // There maybe more to pull from the streamreader, so we cannot assume we are done
                    // this line probably wrong!!!
                    AssertNotReached ();
                    return nullopt;
                } break;
                case Stage_::eEndOutput: {
                xxx:
                    Assert (fRawUnprocessedInputBytes_.empty ());
                    // then input has already signaled EOF (this cannot change) - and we just do final fetch of remaining output (Final flush and frame end)
                    CompressResult_ compressResults = LowLevelCompress_ (fCtx_, span<const byte>{}, ZSTD_e_end, intoBuffer);
                    auto            r               = intoBuffer.subspan (0, compressResults.fProducedOutputBytes_);
                    if (compressResults.fRemaining == 0) {
                        fStage_ = Stage_::eDone; // Frame fully flushed and finished
                    }
                    return r;
                }
                case Stage_::eDone: {
                    return span<byte>{}; // no more data to read
                }
                default:
                    AssertNotReached ();
            }
        }
    };

    struct DecompressingByteStreamRep_ final : InputStream::IRep<byte>, Memory::UseBlockAllocationIfAppropriate<DecompressingByteStreamRep_> {
        unique_ptr<Streams::StreamReader<byte>> fInStreamReader_;                               // wrapped/buffered provided input stream
        Memory::InlineBuffer<byte> fInputBuf_{Memory::eUninitialized, ::ZSTD_DStreamInSize ()}; // used to cache extra input (compressed) bytes not yet proceessed
        span<byte> fRawUnprocessedInputBytes_{};                                                /// empty or subspan of fInputBuf_
        Memory::InlineBuffer<byte> fOutBuf_{Memory::eUninitialized, ::ZSTD_DStreamOutSize ()}; // used to cache extra output (uncompressed) bytes not yet returned (NOTE - CStreamOutSize maybe wrong to use here)
        span<byte>                                                     fOutputBufCache_{}; /// empty or subspan of fOutBuf_
        ZSTD_DCtx*                                                     fCtx_{nullptr};
        SeekOffsetType                                                 fSeekOffset_{};
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;

    private:
        struct DecompressResult_ {
            size_t fConsumedInputBytes_{};
            size_t fProducedOutputBytes_{};
            size_t fRemaining;
        };
        static DecompressResult_ LowLevelDecompress_ (ZSTD_DCtx* ctx, span<const byte> fromBytes, span<byte> intoDecompressedBytes)
        {
            ZSTD_inBuffer  input     = {fromBytes.data (), fromBytes.size (), 0};
            ZSTD_outBuffer output    = {intoDecompressedBytes.data (), intoDecompressedBytes.size (), 0};
            size_t const   remaining = ::ZSTD_decompressStream (ctx, &output, &input);
            ThrowIfZStdErr_ (remaining);
            return DecompressResult_{.fConsumedInputBytes_ = input.pos, .fProducedOutputBytes_ = output.pos, .fRemaining = remaining};
        }

    public:
        DecompressingByteStreamRep_ (const Streams::InputStream::Ptr<byte>& in)
            : fInStreamReader_{make_unique<Streams::StreamReader<byte>> (in)}
        {
            Execution::ThrowIfNull (fCtx_ = ::ZSTD_createDCtx ());
        }
        virtual ~DecompressingByteStreamRep_ ()
        {
            if (fCtx_ != nullptr) {
                ::ZSTD_freeDCtx (fCtx_);
                fCtx_ = nullptr;
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
            if (fOutputBufCache_.size () != 0) {
                return fOutputBufCache_.size ();
            }

            // else tricky - cuz we don't want to block

            // Combine existing fInputBuf_ cached data with a bit more we try to read, so we pass as big a chunk as possible to ZStd lib
            // Read argument windows into fInputBuf_, just after any bytes already read
            if (optional<span<byte>> n =
                    fInStreamReader_->Read (span{fInputBuf_}.subspan (fRawUnprocessedInputBytes_.size ()), NoDataAvailableHandling::eDontBlock)) {
                fRawUnprocessedInputBytes_ = span{fInputBuf_}.first (fRawUnprocessedInputBytes_.size () + n->size ());
            }
            DbgTrace ("fRawUnprocessedInputBytes_ = {}"_f, fRawUnprocessedInputBytes_);

            // if we have any data in fRawUnprocessedInputBytes_, translate it and copy it into fOutputBufCache_.
            // return sizes of know available data.
            ZSTD_inBuffer  input     = {fRawUnprocessedInputBytes_.data (), fRawUnprocessedInputBytes_.size (), 0};
            ZSTD_outBuffer output    = {fOutBuf_.data (), fOutBuf_.size () - fOutputBufCache_.size (), 0};
            size_t const   remaining = ::ZSTD_decompressStream (fCtx_, &output, &input);
            ThrowIfZStdErr_ (remaining);

            // if anything produced cache it
            if (output.pos > 0) {
                // remove consumed input bytes
                fRawUnprocessedInputBytes_ = fRawUnprocessedInputBytes_.subspan (input.pos);
                // extend the fOutputBufCache_
                fOutputBufCache_ = span{fOutBuf_}.first (fOutputBufCache_.size () + output.pos);
                // but dont update seek offset, cuz we didn't actually read anything
            }
            if (fOutputBufCache_.size () != 0) {
                return fOutputBufCache_.size ();
            }
            return nullopt;
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
                size_t nToCopy = min (intoBuffer.size (), fOutputBufCache_.size ());
                auto r = Memory::CopySpanData (fOutputBufCache_.subspan (0, nToCopy), intoBuffer); // intoBuffer large enuf cuz we pinned size with nToCopy
                fOutputBufCache_ = fOutputBufCache_.subspan (nToCopy);                             // skip returned bytes
                fSeekOffset_ += nToCopy;
                return r;
            }

#if 1
            while (true) {
                // Combine existing fInputBuf_ cached data with a bit more we try to read, so we pass as big a chunk as possible to ZStd lib
                // Read argument windows into fInputBuf_, just after any bytes already read
                if (fRawUnprocessedInputBytes_.size () < fInputBuf_.size ()) {
                    if (optional<span<byte>> n = fInStreamReader_->Read (span{fInputBuf_}.subspan (fRawUnprocessedInputBytes_.size ()), blockFlag);
                        n and not n->empty ()) {
                        fRawUnprocessedInputBytes_ = span{fInputBuf_}.first (fRawUnprocessedInputBytes_.size () + n->size ());
                    }
                }

                Assert (fOutputBufCache_.empty ());
                if (fRawUnprocessedInputBytes_.empty ()) {
                    // If nothing available to compress, either input at EOF, or must return nullopt and wait for more

                    if (optional<bool> isAtEOF = fInStreamReader_->IsAtEOF (blockFlag)) {
                        if (*isAtEOF) {
                            // DONE
                            Assert (fOutputBufCache_.empty ());
                            return span<byte>{};
                        }
                        else {
                            Assert (blockFlag == NoDataAvailableHandling::eDontBlock);
                            return nullopt; // non-blocking API - need more data
                        }
                    }
                }
                else {
                    // Now if we have any input bytes to compress (or at EOF, and may need to write more stuff at end), run it through the library
                    DecompressResult_ decompressResults = LowLevelDecompress_ (fCtx_, fRawUnprocessedInputBytes_, span{fOutBuf_});
                    fRawUnprocessedInputBytes_          = fRawUnprocessedInputBytes_.subspan (decompressResults.fConsumedInputBytes_);

                    // if anything produced, adjust cache(s) and return it
                    if (decompressResults.fProducedOutputBytes_ > 0) {
                        // cache excess output bytes, and return those that will fit
                        size_t nToCopy = min (intoBuffer.size (), decompressResults.fProducedOutputBytes_);
                        auto   r       = Memory::CopySpanData (span{fOutBuf_}.subspan (0, nToCopy), intoBuffer);
                        fOutputBufCache_ = span{fOutBuf_}.subspan (nToCopy, decompressResults.fProducedOutputBytes_ - nToCopy); // skip returned bytes
                        fSeekOffset_ += nToCopy;
                        return r;
                    }
                    else {
                        Assert (decompressResults.fConsumedInputBytes_ > 0); // keep going - making progress
                    }
                }
            }

#else
            // Combine existing fInputBuf_ cached data with a bit more we try to read, so we pass as big a chunk as possible to ZStd lib
            // Read argument windows into fInputBuf_, just after any bytes already read
            if (fRawUnprocessedInputBytes_.size () < fInputBuf_.size ()) {
                if (optional<span<byte>> n = fInStreamReader_->Read (span{fInputBuf_}.subspan (fRawUnprocessedInputBytes_.size ()), blockFlag)) {
                    fRawUnprocessedInputBytes_ = span{fInputBuf_}.first (fRawUnprocessedInputBytes_.size () + n->size ());
                }
            }

            Assert (fOutputBufCache_.empty ());
            // Now if we have any input bytes to decompress, run it through the library
            if (not fRawUnprocessedInputBytes_.empty ()) {
                {
                    ZSTD_inBuffer  input     = {fRawUnprocessedInputBytes_.data (), fRawUnprocessedInputBytes_.size (), 0};
                    ZSTD_outBuffer output    = {intoBuffer.data (), intoBuffer.size (), 0};
                    size_t const   remaining = ::ZSTD_decompressStream (fCtx_, &output, &input);
                    ThrowIfZStdErr_ (remaining);

                    // if anything produced, adjust cache(s) and return it
                    if (output.pos > 0) {
                        fRawUnprocessedInputBytes_ = fRawUnprocessedInputBytes_.subspan (input.pos);
                        fSeekOffset_ += output.pos;
                        // Note if remaining > 0, there are more bytes to be flushed out later, retained in the context
                        return intoBuffer.subspan (0, output.pos);
                    }
                    Assert (output.pos == 0);
                }

                // If we have input data, and no output data, maybe output buffer too small: try using a larger output buffer
                if (fOutBuf_.size () > intoBuffer.size ()) {
                    ZSTD_inBuffer  input     = {fRawUnprocessedInputBytes_.data (), fRawUnprocessedInputBytes_.size (), 0};
                    ZSTD_outBuffer output    = {fOutBuf_.data (), fOutBuf_.size (), 0};
                    size_t const   remaining = ::ZSTD_decompressStream (fCtx_, &output, &input);
                    ThrowIfZStdErr_ (remaining);
                    // if anything produced this time, return what we can, and stash the rest.
                    if (output.pos > 0) {
                        fRawUnprocessedInputBytes_ = fRawUnprocessedInputBytes_.subspan (input.pos);
                        fSeekOffset_ += output.pos;
                        // cache excess output bytes, and return those that will fit
                        size_t nToCopy   = min (intoBuffer.size (), output.pos);
                        auto   r         = Memory::CopySpanData (span{fOutBuf_}.subspan (0, nToCopy), intoBuffer);
                        fOutputBufCache_ = span{fOutBuf_}.subspan (nToCopy); // skip returned bytes
                        return r;
                    }
                }
            }

            // at this point, we have no data to return, and no more input data to process. This could be because we are at EOF,
            // or because we did non-blocking reads, and we just didn't get any data yet.

            Assert (fOutputBufCache_.empty ()); // fRawUnprocessedInputBytes_.empty () MIGHT have data, but not enuf to decompress
            if (optional<bool> isAtEOF = fInStreamReader_->IsAtEOF (blockFlag); isAtEOF and *isAtEOF) {
                // @todo IF there is fRawUnprocessedInputBytes_, this i think is bogus data and we should throw!
                return span<byte>{}; // no more data to read
            }
#endif

            // There maybe more to pull from the streamreader, so we cannot assume we are done
            Assert (blockFlag == NoDataAvailableHandling::eDontBlock); // else we should have read more data to return a good answer - only return nullopt if would block
            return nullopt;
        }
    };
}
#endif

Compression::Ptr ZStd::Compress::New (const ZStd::Compress::Options& o)
{
#if qStroika_HasComponent_zstd
    struct MyRep_ : IRep, public Memory::UseBlockAllocationIfAppropriate<MyRep_> {
        ZStd::Compress::Options               fOptions_;
        shared_ptr<CompressingByteStreamRep_> fDelegate2;
        MyRep_ (const ZStd::Compress::Options& o)
            : fOptions_{o}
        {
        }
        virtual InputStream::Ptr<byte> Transform (const InputStream::Ptr<byte>& src)
        {
            fDelegate2 = make_shared<CompressingByteStreamRep_> (src, fOptions_);
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
        shared_ptr<DecompressingByteStreamRep_> fDelegate2;
        virtual InputStream::Ptr<byte>          Transform (const InputStream::Ptr<byte>& src)
        {
            fDelegate2 = make_shared<DecompressingByteStreamRep_> (src);
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
