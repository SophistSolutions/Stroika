/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/Format.h"
#include "../../../Execution/Finally.h"

#include "Reader.h"

#if qHasFeature_LZMA
extern "C" {
#include <lzma/7z.h>
#include <lzma/7zCrc.h>
}
#endif

#if qHasFeature_LZMA && defined(_MSC_VER)
// Use #pragma comment lib instead of explicit entry in the lib entry of the project file
#pragma comment(lib, "lzma.lib")
#endif

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::Archive;

using Memory::SmallStackBuffer;
using Memory::SmallStackBufferCommon;

using std::byte;

#if qHasFeature_LZMA
namespace {
    struct InitOnce_ {
        InitOnce_ ()
        {
            ::CrcGenerateTable ();
        }
    } sInitOnce_;
}

class _7z::Reader::Rep_ : public Reader::_IRep {
private:
    // could do smarter/block allocation or arena allocation, but KISS for now
    static void* Alloc_ (void* /*p*/, size_t size)
    {
        Require (size > 0);
        return new byte[size];
    }
    static void Free_ (void* /*p*/, void* address)
    {
        delete[] reinterpret_cast<byte*> (address);
    }

private:
    mutable ISzAlloc fAllocImp_{};
    mutable ISzAlloc fAllocTempImp_{};
    CSzArEx          fDB_{};
    struct MyISeekInStream : ISeekInStream {
        Streams::InputStream<byte>::Ptr fInStream_;
        MyISeekInStream (const Streams::InputStream<byte>::Ptr& in)
            : fInStream_ (in)
        {
            Read = Stream_Read_;
            Seek = Stream_Seek_;
        }
        static SRes Stream_Read_ (void* pp, void* buf, size_t* size)
        {
            MyISeekInStream* pThis = (MyISeekInStream*)pp;
            size_t           sz    = pThis->fInStream_.Read (reinterpret_cast<byte*> (buf), reinterpret_cast<byte*> (buf) + *size);
            Assert (sz <= *size);
            *size = sz;
            return SZ_OK; // not sure on EOF/underflow?SZ_ERROR_READ
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
                    return SZ_ERROR_UNSUPPORTED;
            }
            return SZ_OK;
        }
    };
    MyISeekInStream     fInSeekStream_;
    mutable CLookToRead fLookStream_{};

public:
    Rep_ (const Streams::InputStream<byte>::Ptr& in)
        : fInSeekStream_ (in)
    {
        fAllocImp_     = ISzAlloc{Alloc_, Free_};
        fAllocTempImp_ = ISzAlloc{Alloc_, Free_};

        ::SzArEx_Init (&fDB_);

        ::LookToRead_CreateVTable (&fLookStream_, false);
        fLookStream_.realStream = &fInSeekStream_;

        SRes ret{};
        if ((ret = ::SzArEx_Open (&fDB_, &fLookStream_.s, &fAllocImp_, &fAllocTempImp_)) != SZ_OK) {
            // throw
            throw "bad";
        }
    }
    ~Rep_ ()
    {
        ::SzArEx_Free (&fDB_, &fAllocImp_);
    }
    virtual Set<String> GetContainedFiles () const override
    {
        Set<String> result;
        for (unsigned int i = 0; i < fDB_.NumFiles; i++) {
            if (not SzArEx_IsDir (&fDB_, i)) {
                size_t nameLen = ::SzArEx_GetFileNameUtf16 (&fDB_, i, nullptr);
                if (nameLen < 1) {
                    break;
                }
                SmallStackBuffer<char16_t> fileName (SmallStackBufferCommon::eUninitialized, nameLen);
                [[maybe_unused]] size_t    z = ::SzArEx_GetFileNameUtf16 (&fDB_, i, reinterpret_cast<UInt16*> (&fileName[0]));
                result.Add (String (&fileName[0]));
            }
        }
        return result;
    }
    virtual Memory::BLOB GetData (const String& fileName) const override
    {
        UInt32 idx = GetIdx_ (fileName);
        if (idx == -1) {
            throw "bad"; //filenotfound
        }

        byte*  outBuffer     = 0;          // it must be 0 before first call for each new archive
        UInt32 blockIndex    = 0xFFFFFFFF; // can have any value if outBuffer = 0
        size_t outBufferSize = 0;          // can have any value if outBuffer = 0

        size_t offset{};
        size_t outSizeProcessed{};

        [[maybe_unused]] auto&& cleanup = Execution::Finally ([&outBuffer, this]() { IAlloc_Free (&fAllocImp_, outBuffer); });

        SRes ret;
        if ((ret = ::SzArEx_Extract (&fDB_, &fLookStream_.s, idx, &blockIndex, reinterpret_cast<uint8_t**> (&outBuffer), &outBufferSize, &offset, &outSizeProcessed, &fAllocImp_, &fAllocTempImp_)) != SZ_OK) {
            throw "bad";
        }
        return Memory::BLOB (outBuffer + offset, outBuffer + offset + outSizeProcessed);
    }
    UInt32 GetIdx_ (const String& fn) const
    {
        // could create map to lookup once and maintain
        for (UInt32 i = 0; i < fDB_.NumFiles; i++) {
            if (not SzArEx_IsDir (&fDB_, i)) {
                size_t nameLen = SzArEx_GetFileNameUtf16 (&fDB_, i, nullptr);
                if (nameLen < 1) {
                    break;
                }
                SmallStackBuffer<char16_t> fileName (SmallStackBufferCommon::eUninitialized, nameLen);
                [[maybe_unused]] size_t    z = ::SzArEx_GetFileNameUtf16 (&fDB_, i, reinterpret_cast<UInt16*> (&fileName[0]));
                if (String (&fileName[0]) == fn) {
                    return i;
                }
            }
        }
        return static_cast<UInt32> (-1);
    }
};

_7z::Reader::Reader (const Streams::InputStream<byte>::Ptr& in)
    : DataExchange::Archive::Reader (make_shared<Rep_> (in))
{
}
#endif
