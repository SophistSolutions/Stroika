/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Execution/FeatureNotSupportedException.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
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

    constexpr size_t kSmallSoBlockAllocWorksWellNotInlineAnyhow_ = 1;

    struct CompressingByteStreamRep_ final : InputStream::IRep<byte>, Memory::UseBlockAllocationIfAppropriate<CompressingByteStreamRep_> {
    public:
        CompressingByteStreamRep_ ()                                 = delete;
        CompressingByteStreamRep_ (const CompressingByteStreamRep_&) = delete;
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
            // at most twice through loop
            while (true) {
                if (fOutputBufCache_.size () != 0) {
                    return fOutputBufCache_.size ();
                }
                FillOutputBufCache_ (NoDataAvailableHandling::eDontBlock); // pull and process what we can without blocking
                if (fOutputBufCache_.empty ()) {
                    if (auto ob = fInStreamReader_->IsAtEOF (NoDataAvailableHandling::eDontBlock); ob and *ob) {
                        return 0;
                    }
                    return nullopt;
                }
            }
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
             * First try output cache, and then fill if needed. If second time around, and still empty, must be EOF or dont know (cuz not blocking)
             */
            while (true) {
                // See if request can be satisfied from cached output bytes; note we only NEED to return one byte (but can return more)
                if (not fOutputBufCache_.empty ()) {
                    size_t nToCopy = min (intoBuffer.size (), fOutputBufCache_.size ());
                    auto r = Memory::CopySpanData (fOutputBufCache_.subspan (0, nToCopy), intoBuffer); // intoBuffer large enuf cuz we pinned size with nToCopy
                    fOutputBufCache_ = fOutputBufCache_.subspan (nToCopy);                             // skip returned bytes
                    fSeekOffset_ += nToCopy;
                    return r;
                }
                FillOutputBufCache_ (blockFlag);
                if (fOutputBufCache_.empty ()) {
                    // if fill failed, either cuz non-blocking, or EOF
                    return fStage_ == Stage_::eDone ? span<byte>{} : optional<span<byte>>{};
                }
            }
        }

    private:
        unique_ptr<Streams::StreamReader<byte>> fInStreamReader_;                                  // wrapped/buffered provided input stream
        Memory::InlineBuffer<byte, kSmallSoBlockAllocWorksWellNotInlineAnyhow_> fInputBuf_{Memory::eUninitialized, ::ZSTD_CStreamInSize ()}; // used to cache extra input (uncompressed) bytes not yet proceessed
        span<byte> fRawUnprocessedInputBytes_{};                                                   // empty or subspan of fInputBuf_
        Memory::InlineBuffer<byte, kSmallSoBlockAllocWorksWellNotInlineAnyhow_> fOutBuf_{Memory::eUninitialized, ::ZSTD_CStreamOutSize ()}; // used to cache extra output (compressed) bytes not yet returned (NOTE - CStreamOutSize maybe wrong to use here)
        span<byte> fOutputBufCache_{}; // empty or subspan of fOutBuf_
        ZSTD_CCtx* fCtx_{nullptr};
        enum class Stage_ {
            eReadingInput,
            eEndOutput,
            eDone
        };
        Stage_                                                         fStage_{Stage_::eReadingInput};
        SeekOffsetType                                                 fSeekOffset_{};
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;

    private:
        struct CompressResult_ {
            size_t fConsumedInputBytes_{};
            size_t fProducedOutputBytes_{};
            size_t fRemaining;
        };
        static CompressResult_ DoZStd_Compress_ (ZSTD_CCtx* ctx, span<const byte> fromBytes, ZSTD_EndDirective endFlag, span<byte> intoCompressedBytes)
        {
            ZSTD_inBuffer  input     = {fromBytes.data (), fromBytes.size (), 0};
            ZSTD_outBuffer output    = {intoCompressedBytes.data (), intoCompressedBytes.size (), 0};
            size_t const   remaining = ::ZSTD_compressStream2 (ctx, &output, &input, endFlag);
            ThrowIfZStdErr_ (remaining);
            return CompressResult_{.fConsumedInputBytes_ = input.pos, .fProducedOutputBytes_ = output.pos, .fRemaining = remaining};
        }

    private:
        void FillOutputBufCache_ (NoDataAvailableHandling blockFlag)
        {
            Require (fOutputBufCache_.empty ());
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
                                goto EndOutput;
                            }
                            else {
                                Assert (blockFlag == NoDataAvailableHandling::eDontBlock); // else we would have blocked getting at least one byte
                                return;
                            }
                        }
                        else {
                            // Now if we have any input bytes to compress (or at EOF, and may need to write more stuff at end), run it through the library
                            CompressResult_ compressResults = DoZStd_Compress_ (fCtx_, fRawUnprocessedInputBytes_, ZSTD_e_continue, span{fOutBuf_});
                            fRawUnprocessedInputBytes_ = fRawUnprocessedInputBytes_.subspan (compressResults.fConsumedInputBytes_);

                            // if anything produced, adjust cache(s) and return it
                            if (compressResults.fProducedOutputBytes_ > 0) {
                                // cache excess output bytes, and return those that will fit
                                fOutputBufCache_ = span{fOutBuf_}.subspan (0, compressResults.fProducedOutputBytes_); // skip returned bytes
                                return;
                            }
                            else {
                                Assert (compressResults.fConsumedInputBytes_ > 0); // keep going - making progress
                            }
                        }
                    }
                    // There maybe more to pull from the streamreader, so we cannot assume we are done
                    // this line probably wrong!!!
                    AssertNotReached ();
                } break;
                case Stage_::eEndOutput: {
                EndOutput:
                    Assert (fOutputBufCache_.empty ());
                    Assert (fRawUnprocessedInputBytes_.empty ());
                    // then input has already signaled EOF (this cannot change) - and we just do final fetch of remaining output (Final flush and frame end)
                    CompressResult_ compressResults = DoZStd_Compress_ (fCtx_, span<const byte>{}, ZSTD_e_end, span{fOutBuf_});
                    fOutputBufCache_                = span{fOutBuf_}.subspan (0, compressResults.fProducedOutputBytes_);
                    if (compressResults.fRemaining == 0) {
                        fStage_ = Stage_::eDone; // Frame fully flushed and finished
                    }
                    return;
                }
                case Stage_::eDone: {
                    return; // no more data to read
                }
                default:
                    AssertNotReached ();
            }
        }
    };

    struct DecompressingByteStreamRep_ final : InputStream::IRep<byte>, Memory::UseBlockAllocationIfAppropriate<DecompressingByteStreamRep_> {
        unique_ptr<Streams::StreamReader<byte>> fInStreamReader_;                                  // wrapped/buffered provided input stream
        Memory::InlineBuffer<byte, kSmallSoBlockAllocWorksWellNotInlineAnyhow_> fInputBuf_{Memory::eUninitialized, ::ZSTD_DStreamInSize ()}; // used to cache extra input (compressed) bytes not yet proceessed
        span<byte> fRawUnprocessedInputBytes_{};                                                   // empty or subspan of fInputBuf_
        Memory::InlineBuffer<byte, kSmallSoBlockAllocWorksWellNotInlineAnyhow_> fOutBuf_{Memory::eUninitialized, ::ZSTD_DStreamOutSize ()}; // used to cache extra output (uncompressed) bytes not yet returned (NOTE - CStreamOutSize maybe wrong to use here)
        span<byte>                                                     fOutputBufCache_{}; // empty or subspan of fOutBuf_
        ZSTD_DCtx*                                                     fCtx_{nullptr};
        SeekOffsetType                                                 fSeekOffset_{};
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;

    public:
        DecompressingByteStreamRep_ ()                                   = delete;
        DecompressingByteStreamRep_ (const DecompressingByteStreamRep_&) = delete;
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
            // at most twice through loop
            while (true) {
                if (fOutputBufCache_.size () != 0) {
                    return fOutputBufCache_.size ();
                }
                FillOutputBufCache_ (NoDataAvailableHandling::eDontBlock); // pull and process what we can without blocking
                if (fOutputBufCache_.empty ()) {
                    if (auto ob = fInStreamReader_->IsAtEOF (NoDataAvailableHandling::eDontBlock); ob and *ob) {
                        return 0;
                    }
                    return nullopt;
                }
            }
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
             * First try output cache, and then fill if needed. If second time around, and still empty, must be EOF or dont know (cuz not blocking)
             */
            while (true) {
                // See if request can be satisfied from cached output bytes; note we only NEED to return one byte (but can return more)
                if (not fOutputBufCache_.empty ()) {
                    size_t nToCopy = min (intoBuffer.size (), fOutputBufCache_.size ());
                    auto r = Memory::CopySpanData (fOutputBufCache_.subspan (0, nToCopy), intoBuffer); // intoBuffer large enuf cuz we pinned size with nToCopy
                    fOutputBufCache_ = fOutputBufCache_.subspan (nToCopy);                             // skip returned bytes
                    fSeekOffset_ += nToCopy;
                    return r;
                }
                FillOutputBufCache_ (blockFlag);
                if (fOutputBufCache_.empty ()) {
                    // if fill failed, either cuz non-blocking, or EOF
                    return blockFlag == NoDataAvailableHandling::eBlockIfNoDataAvailable ? span<byte>{} : optional<span<byte>>{};
                }
            }
        }

    private:
        struct DecompressResult_ {
            size_t fConsumedInputBytes_{};
            size_t fProducedOutputBytes_{};
            size_t fRemaining;
        };
        static DecompressResult_ DoZStd_Decompress_ (ZSTD_DCtx* ctx, span<const byte> fromBytes, span<byte> intoDecompressedBytes)
        {
            ZSTD_inBuffer  input     = {fromBytes.data (), fromBytes.size (), 0};
            ZSTD_outBuffer output    = {intoDecompressedBytes.data (), intoDecompressedBytes.size (), 0};
            size_t const   remaining = ::ZSTD_decompressStream (ctx, &output, &input);
            ThrowIfZStdErr_ (remaining);
            return DecompressResult_{.fConsumedInputBytes_ = input.pos, .fProducedOutputBytes_ = output.pos, .fRemaining = remaining};
        }
        void FillOutputBufCache_ (NoDataAvailableHandling blockFlag)
        {
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
                    return; // all we can do... either at EOF, or no more non-blocking data available
                }
                else {
                    // Now if we have any input bytes to decompress
                    DecompressResult_ decompressResults = DoZStd_Decompress_ (fCtx_, fRawUnprocessedInputBytes_, span{fOutBuf_});
                    fRawUnprocessedInputBytes_          = fRawUnprocessedInputBytes_.subspan (decompressResults.fConsumedInputBytes_);

                    // if anything produced, adjust cache(s) and return it
                    if (decompressResults.fProducedOutputBytes_ > 0) {
                        fOutputBufCache_ = span{fOutBuf_}.subspan (0, decompressResults.fProducedOutputBytes_);
                        return;
                    }
                    else {
                        Assert (decompressResults.fConsumedInputBytes_ > 0); // keep going - making progress - buffered internally in fCtx_
                    }
                }
            }
        }
    };
}
#endif

Compression::Ptr ZStd::Compress::New (const ZStd::Compress::Options& o)
{
#if qStroika_HasComponent_zstd
    struct MyRep_ final : IRep, public Memory::UseBlockAllocationIfAppropriate<MyRep_> {
        ZStd::Compress::Options               fOptions_;
        shared_ptr<CompressingByteStreamRep_> fDelegate2;
        MyRep_ (const ZStd::Compress::Options& o)
            : fOptions_{o}
        {
        }
        virtual InputStream::Ptr<byte> Transform (const InputStream::Ptr<byte>& src)
        {
            fDelegate2 = Memory::MakeSharedPtr<CompressingByteStreamRep_> (src, fOptions_);
            return InputStream::Ptr<byte>{fDelegate2};
        }
        virtual optional<Compression::Stats> GetStats () const
        {
            return nullopt;
        }
    };
    return Compression::Ptr{Memory::MakeSharedPtr<MyRep_> (o)};
#else
    Execution::Throw (kNotSuppExcept_);
#endif
}
Compression::Ptr ZStd::Decompress::New ([[maybe_unused]] const ZStd::Decompress::Options& o)
{
#if qStroika_HasComponent_zstd
    struct MyRep_ final : IRep, public Memory::UseBlockAllocationIfAppropriate<MyRep_> {
        shared_ptr<DecompressingByteStreamRep_> fDelegate2;
        virtual InputStream::Ptr<byte>          Transform (const InputStream::Ptr<byte>& src)
        {
            fDelegate2 = Memory::MakeSharedPtr<DecompressingByteStreamRep_> (src);
            return InputStream::Ptr<byte>{fDelegate2};
        }
        virtual optional<Compression::Stats> GetStats () const
        {
            return nullopt;
        }
    };
    return Compression::Ptr{Memory::MakeSharedPtr<MyRep_> ()};
#else
    Execution::Throw (kNotSuppExcept_);
#endif
}
