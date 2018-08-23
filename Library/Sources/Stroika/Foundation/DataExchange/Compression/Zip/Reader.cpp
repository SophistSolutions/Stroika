/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/Format.h"

#include "Reader.h"

//// SEEE http://www.zlib.net/zlib_how.html
/// THIS SHOWS PRETTY SIMPLE EXAMPLE OF HOW TO DO COMPRESS/DECOMPRESS AND WE CAN USE THAT to amke new stream object
/// where inner loop is done each time through with a CHUNK

#if qHasFeature_ZLib
#include <zlib.h>

#if defined(_MSC_VER)
// Use #pragma comment lib instead of explicit entry in the lib entry of the project file
#pragma comment(lib, "zlib.lib")
#endif

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::Compression;
using namespace Stroika::Foundation::Streams;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

namespace {

    void ThrowIfZLibErr_ (int err)
    {
        // VERY ROUGH DRAFT - probably need a more specific exception object type
        if (err != Z_OK) {
            switch (err) {
                case Z_VERSION_ERROR:
                    Execution::Throw (Execution::StringException (L"ZLIB Z_VERSION_ERROR"));
                case Z_DATA_ERROR:
                    Execution::Throw (Execution::StringException (L"ZLIB Z_DATA_ERROR"));
                case Z_ERRNO:
                    Execution::Throw (Execution::StringException (Characters::Format (L"ZLIB Z_ERRNO (errno=%d", errno)));
                default:
                    Execution::Throw (Execution::StringException (Characters::Format (L"ZLIB ERR %d", err)));
            }
        }
    }

    using Memory::Byte;
    struct MyCompressionStream_ : InputStream<Byte>::Ptr {
        struct BaseRep_ : public InputStream<Byte>::_IRep {
        private:
            static constexpr size_t CHUNK_ = 16384;

        public:
            Streams::InputStream<Memory::Byte>::Ptr fInStream_;
            z_stream                                fZStream_;
            Byte                                    fInBuf_[CHUNK_];
            SeekOffsetType                          _fSeekOffset{};
            BaseRep_ (const Streams::InputStream<Memory::Byte>::Ptr& in)
                : fInStream_ (in)
                , fZStream_{}
            {
            }
            virtual ~BaseRep_ () = default;
            virtual bool IsSeekable () const override
            {
                // for now - KISS
                return false; // SHOULD allow seekable IFF src is seekable
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
            virtual SeekOffsetType SeekRead (Whence /*whence*/, SignedSeekOffsetType /*offset*/) override
            {
                RequireNotReached ();
                Require (IsOpenRead ());
                return SeekOffsetType{};
            }
            nonvirtual bool _AssureInputAvailableReturnTrueIfAtEOF ()
            {
                Require (IsOpenRead ());
                if (fZStream_.avail_in == 0) {
                    Assert (NEltsOf (fInBuf_) < numeric_limits<uInt>::max ());
                    fZStream_.avail_in = static_cast<uInt> (fInStream_.Read (begin (fInBuf_), end (fInBuf_)));
                    fZStream_.next_in  = reinterpret_cast<Bytef*> (begin (fInBuf_));
                }
                return fZStream_.avail_in == 0;
            }
        };
        struct DeflateRep_ : BaseRep_ {
            DeflateRep_ (const Streams::InputStream<Memory::Byte>::Ptr& in)
                : BaseRep_ (in)
            {
                int level = Z_DEFAULT_COMPRESSION;
                ThrowIfZLibErr_ (::deflateInit (&fZStream_, level));
            }
            virtual ~DeflateRep_ ()
            {
                Verify (::deflateEnd (&fZStream_) == Z_OK);
            }
            virtual size_t Read (ElementType* intoStart, ElementType* intoEnd) override
            {
                Require (intoStart < intoEnd); // API rule for streams
                Require (IsOpenRead ());
            Again:
                bool isAtSrcEOF = _AssureInputAvailableReturnTrueIfAtEOF ();

                Require (intoStart < intoEnd);
                ptrdiff_t outBufSize = intoEnd - intoStart;

                int flush = isAtSrcEOF ? Z_FINISH : Z_NO_FLUSH;

                fZStream_.avail_out = static_cast<uInt> (outBufSize);
                fZStream_.next_out  = reinterpret_cast<Bytef*> (intoStart);
                int ret;
                switch (ret = ::deflate (&fZStream_, flush)) {
                    case Z_OK:
                        break;
                    case Z_STREAM_END:
                        break;
                    default:
                        ThrowIfZLibErr_ (ret);
                }

                ptrdiff_t pulledOut = outBufSize - fZStream_.avail_out;
                Assert (pulledOut <= outBufSize);
                if (pulledOut == 0 and not isAtSrcEOF and flush == Z_NO_FLUSH) {
                    goto Again;
                }
                _fSeekOffset += pulledOut;
                return pulledOut;
            }
            virtual optional<size_t> ReadNonBlocking ([[maybe_unused]] ElementType* intoStart, [[maybe_unused]] ElementType* intoEnd) override
            {
                Require ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1);
                Require (IsOpenRead ());
// https://stroika.atlassian.net/browse/STK-567 EXPERIMENTAL DRAFT API - INCOMPLETE IMPL
#if 0
                if (intoStart == nullptr) {
                    // Just check if any data available

                }
                else {
                    Assert (intoStart < intoEnd);
                Again:
                    if (fZStream_.avail_in == 0) {
                        Assert (NEltsOf (fInBuf_) < numeric_limits<uInt>::max ());
                        if (auto o = fInStream_.ReadNonBlocking (begin (fInBuf_), end (fInBuf_))) {
                            fZStream_.avail_in = static_cast<uInt> (*o);
                            fZStream_.next_in = begin (fInBuf_);
                        }
                        else {
                            return {};
                        }
                    }
                    ptrdiff_t outBufSize = intoEnd - intoStart;
                    fZStream_.avail_out = static_cast<uInt> (outBufSize);
                    fZStream_.next_out = intoStart;
                    bool isAtSrcEOF = fZStream_.avail_in == 0;
                    int flush = isAtSrcEOF ? Z_FINISH : Z_NO_FLUSH;
                    int ret;
                    switch (ret = ::deflate (&fZStream_, flush)) {
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
                    _fSeekOffset += pulledOut;
                    return pulledOut;
                }
#else
                // while nothing available to output, try getting more from input stream until we cannot (either cuz {} or EOF)
                // and then report accordingly
                //
                // INCOMPLETE - tricky... most common cases easy, but edges hard...
                //&&&& issues are:
                //>> wrong meaning for available_out - we have no output buffer
                //  >> need output buffer for this API, since could pass in no argument buffer (OK to be one byte in that case)
                while (fZStream_.avail_out == 0) {
                    if (fZStream_.avail_in == 0) {
                        if (optional<size_t> tmpAvail = fInStream_.ReadNonBlocking (begin (fInBuf_), end (fInBuf_))) {
                        }
                        else {
                            return {};
                        }
                    }
                }
#endif
                WeakAssert (false);
                // @todo - FIX TO REALLY CHECK
                return {};
            }
        };
        struct InflateRep_ : BaseRep_ {
            InflateRep_ (const Streams::InputStream<Memory::Byte>::Ptr& in)
                : BaseRep_ (in)
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
            virtual size_t Read (ElementType* intoStart, ElementType* intoEnd) override
            {
                Require (intoStart < intoEnd); // API rule for streams
                Require (IsOpenRead ());
            Again:
                bool      isAtSrcEOF = _AssureInputAvailableReturnTrueIfAtEOF ();
                ptrdiff_t outBufSize = intoEnd - intoStart;

                fZStream_.avail_out = static_cast<uInt> (outBufSize);
                fZStream_.next_out  = reinterpret_cast<Bytef*> (intoStart);
                int ret;
                switch (ret = ::inflate (&fZStream_, Z_NO_FLUSH)) {
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
                _fSeekOffset += pulledOut;
                return pulledOut;
            }
            virtual optional<size_t> ReadNonBlocking ([[maybe_unused]] ElementType* intoStart, [[maybe_unused]] ElementType* intoEnd) override
            {
                // https://stroika.atlassian.net/browse/STK-567 EXPERIMENTAL DRAFT API - incomplete IMPL
                Require ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1);
                Require (IsOpenRead ());
                WeakAssert (false);
                // @todo - FIX TO REALLY CHECK
                return {};
            }
        };
        enum Compression { eCompression };
        enum DeCompression { eDeCompression };
        MyCompressionStream_ (Compression, const Streams::InputStream<Memory::Byte>::Ptr& in)
            : InputStream<Byte>::Ptr (make_shared<DeflateRep_> (in))
        {
        }
        MyCompressionStream_ (DeCompression, const Streams::InputStream<Memory::Byte>::Ptr& in)
            : InputStream<Byte>::Ptr (make_shared<InflateRep_> (in))
        {
        }
    };
}
#endif

#if qHasFeature_ZLib
class Zip::Reader::Rep_ : public Reader::_IRep {
public:
    virtual InputStream<Byte>::Ptr Compress (const InputStream<Byte>::Ptr& src) const override
    {
        return MyCompressionStream_ (MyCompressionStream_::eCompression, src);
    }
    virtual InputStream<Byte>::Ptr Decompress (const InputStream<Byte>::Ptr& src) const override
    {
        return MyCompressionStream_ (MyCompressionStream_::eDeCompression, src);
    }
};

Zip::Reader::Reader ()
    : DataExchange::Compression::Reader (make_shared<Rep_> ())
{
}
#endif
