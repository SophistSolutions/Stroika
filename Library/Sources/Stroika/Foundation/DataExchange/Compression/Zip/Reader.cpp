/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    "Reader.h"



//// SEEE http://www.zlib.net/zlib_how.html
/// THIS SHOWS PRETTY SIMPLE EXAMPLE OF HOW TO DO COMPRESS/DECOMPRESS AND WE CAN USE THAT to amke new stream object
/// where inner loop is done each time through with a CHUNK



#if     qHasFeature_ZLib
#include    <zlib.h>


#if     defined (_MSC_VER)
// Use #pragma comment lib instead of explicit entry in the lib entry of the project file
#pragma comment (lib, "zlib.lib")
#endif



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::DataExchange::Compression;
using   namespace	Stroika::Foundation::Streams;






namespace {

    void    ThrowIfZLibErr_ (int err)
    {
        if (err != Z_OK) {
            Execution::Throw (Execution::StringException (L"ZLIB ERR"));    // @todo embelish
        }
    }

    using   Memory::Byte;
    struct MyCompressionStream_ : InputStream<Byte> {
        struct  BaseRep_ : public _IRep  {
            Streams::InputStream<Memory::Byte>  fInStream_;
            z_stream strm;
            BaseRep_ (const Streams::InputStream<Memory::Byte>& in)
                : fInStream_ (in)
                , strm {}
            {
            }
            virtual ~BaseRep_ () = default;
            virtual bool    IsSeekable () const
            {
                // for now - KISS
                return false;   // SHOULD allow seekable IFF src is seekable
            }
            virtual SeekOffsetType      GetReadOffset () const override
            {
                return SeekOffsetType {};
            }
            virtual SeekOffsetType      SeekRead (Whence whence, SignedSeekOffsetType offset) override
            {
                RequireNotReached ();
                return SeekOffsetType {};
            }
        };
        struct  DeflateRep_ : BaseRep_  {
            DeflateRep_ (const Streams::InputStream<Memory::Byte>& in)
                : BaseRep_ (in)
            {
                int level = 1;
                ThrowIfZLibErr_ (::deflateInit (&strm, level));
            }
            virtual ~DeflateRep_ ()
            {
                Verify (::deflateEnd (&strm) == Z_OK);
            }
            virtual size_t  Read (SeekOffsetType* offset, ElementType* intoStart, ElementType* intoEnd) override
            {
                // @TODO - THIS IS WRONG- in that it doesnt take into account if strm.next_in still has data
                //tmphack - do 1 byte at a time
                Require (intoStart < intoEnd);
                Byte b[1];
                size_t n = fInStream_.Read (&b[0], &b[1]);
                if (n == 0) {
                    return 0;
                }
                Assert (n == 1);
                strm.next_in = b;
                strm.avail_in = n;

                strm.avail_out = intoEnd - intoStart;
                strm.next_out = intoStart;
                ThrowIfZLibErr_ (::deflate (&strm, Z_NO_FLUSH));

                Assert (static_cast<ptrdiff_t> (strm.avail_out) < intoEnd - intoStart);
                return strm.avail_out;
            }
        };
        struct  InflateRep_ : BaseRep_  {
            InflateRep_ (const Streams::InputStream<Memory::Byte>& in)
                : BaseRep_ (in)
            {
                ThrowIfZLibErr_ (::inflateInit (&strm));
            }
            virtual ~InflateRep_ ()
            {
                Verify (::inflateEnd (&strm) == Z_OK);
            }
            virtual size_t  Read (SeekOffsetType* offset, ElementType* intoStart, ElementType* intoEnd) override
            {
                // @TODO - THIS IS WRONG- in that it doesnt take into account if strm.next_in still has data
                //tmphack - do 1 byte at a time
                Require (intoStart < intoEnd);
                Byte b[1];
                size_t n = fInStream_.Read (&b[0], &b[1]);
                if (n == 0) {
                    return 0;
                }
                Assert (n == 1);
                strm.next_in = b;
                strm.avail_in = n;

                strm.avail_out = intoEnd - intoStart;
                strm.next_out = intoStart;
                ThrowIfZLibErr_ (::inflate (&strm, Z_NO_FLUSH));

                Assert (static_cast<ptrdiff_t> (strm.avail_out) < intoEnd - intoStart);
                return strm.avail_out;
            }
        };
        enum Compression {eCompression};
        enum DeCompression {eDeCompression};
        MyCompressionStream_ (Compression, const Streams::InputStream<Memory::Byte>& in)
            : InputStream<Byte> (make_shared<DeflateRep_> (in))
        {
        }
        MyCompressionStream_ (DeCompression, const Streams::InputStream<Memory::Byte>& in)
            : InputStream<Byte> (make_shared<InflateRep_> (in))/// tmphack cuz NYI otjer
        {
        }
    };
}
#endif







#if     qHasFeature_ZLib
class   Zip::Reader::Rep_ : public Reader::_IRep {
public:
    Rep_ () = default;
    ~Rep_ () = default;
    virtual InputStream<Byte>   Compress (const InputStream<Byte>& src) const override
    {
        return MyCompressionStream_ (MyCompressionStream_::eCompression, src);
    }
    virtual InputStream<Byte>   Decompress (const InputStream<Byte>& src) const override
    {
        return MyCompressionStream_ (MyCompressionStream_::eDeCompression, src);
    }
};

Zip::Reader::Reader ()
    : DataExchange::Compression::Reader (make_shared<Rep_> ())
{
}
#endif
