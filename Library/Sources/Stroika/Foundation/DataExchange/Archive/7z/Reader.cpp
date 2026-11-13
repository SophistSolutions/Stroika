/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedChecker.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

#include "Reader.h"

#if qStroika_HasComponent_LZMA
extern "C" {
#include <lzma/7z.h>
#include <lzma/7zCrc.h>
}
#endif

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::Archive;
using namespace Stroika::Foundation::Streams;

using Memory::StackBuffer;
using std::byte;

#if qStroika_HasComponent_LZMA
namespace {
    struct InitOnce_ {
        InitOnce_ ()
        {
            ::CrcGenerateTable ();
        }
    } sInitOnce_;
}

namespace {
    void ThrowIfSzErr_ (SRes err)
    {
        switch (err) {
            case SZ_OK:
                return;
            case SZ_ERROR_MEM:
                Execution::Throw (bad_alloc{});
            case SZ_ERROR_NO_ARCHIVE:
            case SZ_ERROR_ARCHIVE:
            case SZ_ERROR_INPUT_EOF:
            case SZ_ERROR_DATA:
            case SZ_ERROR_CRC:
                Execution::Throw (BadFormatException{"Not a 7z archive, or a damaged one (LZMA SDK error {})"_f(err)});
            case SZ_ERROR_UNSUPPORTED:
                Execution::Throw (Execution::Exception<runtime_error>{"7z archive needs a feature this reader lacks (LZMA SDK error {})"_f(err)});
            default:
                Execution::Throw (Execution::Exception<runtime_error>{"7z: LZMA SDK error {}"_f(err)});
        }
    }
}

namespace {

    struct Rep_ final : Reader::IRep {
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
        // fDB_'s index of each file (not directory), by name
        Mapping<String, UInt32> fFileIndexes_;
        // GetData's WriteContext: it reads through fLookStream_, moving it and fInSeekStream_, and replaces fBlock_; GetContainedFiles
        // only reads fFileIndexes_
        mutable Debug::AssertExternallySynchronizedChecker fThisAssertExternallySynchronized_;
        struct MyISeekInStream : ISeekInStream {
            Streams::InputStream::Ptr<byte> fInStream_;
            MyISeekInStream (const Streams::InputStream::Ptr<byte>& in)
                : fInStream_{in}
            {
                Read = Stream_Read_;
                Seek = Stream_Seek_;
            }
            static SRes Stream_Read_ (void* pp, void* buf, size_t* size)
            {
                MyISeekInStream* pThis = (MyISeekInStream*)pp;
                size_t           sz    = pThis->fInStream_.ReadBlocking (span{reinterpret_cast<byte*> (buf), *size}).size ();
                Assert (sz <= *size);
                *size = sz;
                return SZ_OK; // *size 0 at the end of the stream, as ISeekInStream specifies
            }
            static SRes Stream_Seek_ (void* pp, Int64* pos, ESzSeek origin)
            {
                MyISeekInStream* pThis = (MyISeekInStream*)pp;
                switch (origin) {
                    case SZ_SEEK_SET:
                        *pos = pThis->fInStream_.Seek (*pos);
                        break;
                    case SZ_SEEK_CUR:
                        *pos = pThis->fInStream_.Seek (eFromCurrent, *pos);
                        break;
                    case SZ_SEEK_END:
                        *pos = pThis->fInStream_.Seek (eFromEnd, *pos);
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
        /*
         *  The block (7z's 'folder') GetData last decompressed, kept for the next call: a solid archive packs many files into one
         *  block, so reading each would otherwise decompress the whole block again. These are SzArEx_Extract's in/out arguments,
         *  which replaces the block when a file is in another. Holds memory the size of the block until then.
         */
        mutable Byte*  fBlock_{nullptr}; // from fAllocImp_; nullptr: none held
        mutable UInt32 fBlockIndex_{};   // any value while fBlock_ is nullptr
        mutable size_t fBlockSize_{};    // ditto

    public:
        Rep_ (const Streams::InputStream::Ptr<byte>& in)
            : fInSeekStream_{in}
        {
            fAllocImp_     = ISzAlloc{Alloc_, Free_};
            fAllocTempImp_ = ISzAlloc{Alloc_, Free_};

            ::SzArEx_Init (&fDB_);

            ::LookToRead_CreateVTable (&fLookStream_, false);
            fLookStream_.realStream = &fInSeekStream_;

            ThrowIfSzErr_ (::SzArEx_Open (&fDB_, &fLookStream_.s, &fAllocImp_, &fAllocTempImp_)); // which frees fDB_ itself on failure
            try {
                for (UInt32 i = 0; i < fDB_.NumFiles; ++i) {
                    if (not SzArEx_IsDir (&fDB_, i)) {
                        size_t nameLen = ::SzArEx_GetFileNameUtf16 (&fDB_, i, nullptr); // with its NUL; 0 when the archive stores no names
                        if (nameLen < 1) {
                            break;
                        }
                        StackBuffer<char16_t> fileName{Memory::eUninitialized, nameLen};
                        (void)::SzArEx_GetFileNameUtf16 (&fDB_, i, reinterpret_cast<UInt16*> (fileName.data ()));
                        fFileIndexes_.Add (String{span{fileName.data (), nameLen - 1}}, i, AddReplaceMode::eAddIfMissing); // of two the same, the first
                    }
                }
            }
            catch (...) {
                ::SzArEx_Free (&fDB_, &fAllocImp_); // no ~Rep_ for a constructor that throws
                throw;
            }
        }
        ~Rep_ ()
        {
            IAlloc_Free (&fAllocImp_, fBlock_);
            ::SzArEx_Free (&fDB_, &fAllocImp_);
        }
        virtual Set<String> GetContainedFiles () const override
        {
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fThisAssertExternallySynchronized_};
            return Set<String>{fFileIndexes_.Keys ()};
        }
        virtual Streams::InputStream::Ptr<byte> GetSource () const override
        {
            return fInSeekStream_.fInStream_;
        }
        virtual Memory::BLOB GetData (const String& fileName) const override
        {
            Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized_};
            optional<UInt32>                                         idx = fFileIndexes_.Lookup (fileName);
            if (not idx) [[unlikely]] {
                Execution::Throw (Execution::Exception<runtime_error>{"File '{}' not found"_f(fileName)});
            }
            size_t offset{};
            size_t outSizeProcessed{};
            if (SRes err = ::SzArEx_Extract (&fDB_, &fLookStream_.s, *idx, &fBlockIndex_, &fBlock_, &fBlockSize_, &offset,
                                             &outSizeProcessed, &fAllocImp_, &fAllocTempImp_);
                err != SZ_OK) [[unlikely]] {
                // a failed decode leaves fBlock_ partly filled, though marked as fBlockIndex_ - drop it, or the next call would use it
                IAlloc_Free (&fAllocImp_, fBlock_);
                fBlock_ = nullptr;
                ThrowIfSzErr_ (err);
            }
            return Memory::BLOB{fBlock_ + offset, fBlock_ + offset + outSizeProcessed};
        }
    };
}

/*
 ********************************************************************************
 ***************** DataExchange::Archive::_7z::Reader::New **********************
 ********************************************************************************
 */
Archive::Reader::Ptr Archive::_7z::Reader::New (const Streams::InputStream::Ptr<byte>& readFrom)
{
    return Archive::Reader::Ptr{Memory::MakeSharedPtr<Rep_> (readFrom)};
}

#endif
