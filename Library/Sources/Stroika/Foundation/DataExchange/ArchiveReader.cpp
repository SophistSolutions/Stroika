/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"
#include    "../Streams/iostream/InputStreamFromStdIStream.h"

#include    "ArchiveReader.h"


#if     qHasFeature_LZMA
extern "C" {
#include    <lzma/7z.h>
#include    <lzma/7zCrc.h>
}
#endif






#if     qHasFeature_LZMA && defined (_MSC_VER)
// Use #pragma comment lib instead of explicit entry in the lib entry of the project file
#pragma comment (lib, "lzma.lib")
#endif




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;


using   Streams::iostream::InputStreamFromStdIStream;

#if 0

/*
 ********************************************************************************
 ****************************** DataExchange::ArchiveReader ****************************
 ********************************************************************************
 */
VariantValue    Reader::Read (istream& in)
{
    return Read (InputStreamFromStdIStream<Memory::Byte> (in));
}

VariantValue    Reader::Read (wistream& in)
{
    return Read (InputStreamFromStdIStream<Characters::Character> (in));
}
#endif


#if qHasFeature_LZMA


// example code/guidance

namespace {
    struct InitOnce_ {
        InitOnce_ ()
        {
            ::CrcGenerateTable ();
        }
    } sInitOnce_;
}

class ArchiveReader_7z::Rep_ : public ArchiveReader::_IRep {
private:
    // could do smarter/block allocation or arena allocation, but KISS for now
    static    void* Alloc_ (void* p, size_t size)
    {
        Require (size > 0);
        return new Byte [size];
    }
    static    void Free_ (void* p, void* address)
    {
        delete[] address;
    }


private:
    CSzArEx fDB_ {};
    CLookToRead lookStream {};
    ISzAlloc allocImp {  };
    ISzAlloc allocTempImp {};
    struct MyISeekInStream : ISeekInStream {
        MyISeekInStream (const Streams::InputStream<Memory::Byte>& in)
            : fInStream_ (in)
        {
            Read = Stream_Read_;
            Seek = Stream_Seek_;
        }
        Streams::InputStream<Memory::Byte> fInStream_;
        static SRes Stream_Read_(void* pp, void* buf, size_t* size)
        {
            MyISeekInStream* pThis = (MyISeekInStream*)pp;
            size_t sz = pThis->fInStream_.Read (reinterpret_cast<Byte*> (buf), reinterpret_cast<Byte*> (buf) + *size);
            Assert (sz <= *size);
            *size = sz;
            return SZ_OK;   // not sure on EOF/underflow?SZ_ERROR_READ
        }

        static SRes Stream_Seek_ (void* pp, Int64* pos, ESzSeek origin)
        {
            MyISeekInStream* pThis = (MyISeekInStream*)pp;
            switch (origin) {
                case SZ_SEEK_SET:
                    *pos = pThis->fInStream_.Seek (*pos);
                    break;
                case SZ_SEEK_CUR:
                    *pos = pThis->fInStream_.Seek (Streams::Whence::eFromCurrent, *pos);
                    break;
                case SZ_SEEK_END:
                    *pos = pThis->fInStream_.Seek (Streams::Whence::eFromEnd, *pos);
                    break;
                default:
                    AssertNotReached ();
                    return ERROR_INVALID_PARAMETER;
            }
            return SZ_OK;
        }
    };
    MyISeekInStream inSeekStream;
    Streams::InputStream<Memory::Byte>  fInStream_;

public:
    Rep_ (const Streams::InputStream<Memory::Byte>& in)
        : fInStream_ (in)
        , inSeekStream (in)
    {
        allocImp = ISzAlloc { Alloc_, Free_ };
        allocTempImp  = ISzAlloc {Alloc_, Free_};

        SzArEx_Init (&fDB_);

        LookToRead_CreateVTable(&lookStream, false);
        lookStream.realStream = &inSeekStream;


        SRes  ret {};
        if ((ret = SzArEx_Open (&fDB_, &lookStream.s, &allocImp, &allocTempImp)) != SZ_OK) {
            // throw
        }

    }
    ~Rep_ ()
    {
        SzArEx_Free (&fDB_, &allocImp);
    }
    virtual Set<String>     GetContainedFiles () const override
    {
        Set<String> result;
        for (unsigned int i = 0; i < fDB_.NumFiles; i++) {
            if (not SzArEx_IsDir (&fDB_, i)) {
                size_t file_name_length = SzArEx_GetFileNameUtf16 (&fDB_, i, NULL);
                if (file_name_length < 1) {
                    break;
                }
                std::vector<char16_t> file_name(file_name_length);
                size_t z = SzArEx_GetFileNameUtf16(&fDB_, i, &file_name[0]);
                result.Add (String (&file_name[0]));
            }
        }
        return result;
    }
};


ArchiveReader_7z::ArchiveReader_7z (const Streams::InputStream<Memory::Byte>& in)
    : ArchiveReader (make_shared<Rep_> (in))
{
}
#endif
