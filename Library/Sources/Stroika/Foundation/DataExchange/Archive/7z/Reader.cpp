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

    // the LZMA SDK's allocator - for its main and temporary allocations alike. Could do smarter/block allocation or arena
    // allocation, but KISS for now
    void* Alloc_ (ISzAllocPtr, size_t size)
    {
        Require (size > 0);
        return new byte[size];
    }
    void Free_ (ISzAllocPtr, void* address)
    {
        delete[] reinterpret_cast<byte*> (address);
    }
    constexpr ISzAlloc kAlloc_{Alloc_, Free_};

    struct Rep_ final : Reader::IRep {
    private:
        CSzArEx fDB_{};
        // fDB_'s index of each file (not directory), by name
        Mapping<String, UInt32> fFileIndexes_;
        // GetData's WriteContext: it reads through fLookStream_, moving it and fInSeekStream_, and replaces fBlock_; GetContainedFiles
        // only reads fFileIndexes_
        mutable Debug::AssertExternallySynchronizedChecker fThisAssertExternallySynchronized_;
        /*
         *  The LZMA SDK's view of fInStream_. The SDK is C, so an exception must not unwind through it - it would free nothing on the
         *  way, and MSVC assumes extern "C" code never throws: a callback keeps fInStream_'s exception and returns SZ_ERROR_READ, and
         *  RethrowStreamFailure () rethrows it once the SDK has returned.
         */
        struct MyISeekInStream : ISeekInStream {
            Streams::InputStream::Ptr<byte> fInStream_;
            exception_ptr                   fFailure_;
            MyISeekInStream (const Streams::InputStream::Ptr<byte>& in)
                : fInStream_{in}
            {
                Read = Stream_Read_;
                Seek = Stream_Seek_;
            }
            void RethrowStreamFailure ()
            {
                if (exception_ptr e = exchange (fFailure_, nullptr)) [[unlikely]] {
                    Execution::ReThrow (e);
                }
            }
            // the SDK passes its interfaces const; this is the very (non-const) object it was given
            static MyISeekInStream* This_ (ISeekInStreamPtr pp)
            {
                return const_cast<MyISeekInStream*> (static_cast<const MyISeekInStream*> (pp));
            }
            static SRes Stream_Read_ (ISeekInStreamPtr pp, void* buf, size_t* size)
            {
                MyISeekInStream* pThis = This_ (pp);
                size_t           want  = exchange (*size, 0); // 0 if it fails: the SDK's CLookToRead2 takes *size even then
                return pThis->Guarded_ ([&] () {
                    *size = pThis->fInStream_.ReadBlocking (span{reinterpret_cast<byte*> (buf), want}).size ();
                    Assert (*size <= want); // 0 at the end of the stream, as ISeekInStream specifies
                });
            }
            static SRes Stream_Seek_ (ISeekInStreamPtr pp, Int64* pos, ESzSeek origin)
            {
                MyISeekInStream* pThis = This_ (pp);
                Whence           whence;
                switch (origin) {
                    case SZ_SEEK_SET:
                        whence = eFromStart;
                        break;
                    case SZ_SEEK_CUR:
                        whence = eFromCurrent;
                        break;
                    case SZ_SEEK_END:
                        whence = eFromEnd;
                        break;
                    default:
                        AssertNotReached ();
                        return SZ_ERROR_UNSUPPORTED;
                }
                return pThis->Guarded_ ([&] () { *pos = pThis->fInStream_.Seek (whence, *pos); });
            }

        private:
            /*
             *  f (), or SZ_ERROR_READ if it throws - its exception kept for RethrowStreamFailure (). Once one has, every call fails,
             *  so the SDK goes no further on what it read.
             */
            SRes Guarded_ (const invocable auto& f) noexcept
            {
                if (fFailure_ == nullptr) {
                    try {
                        f ();
                        return SZ_OK;
                    }
                    catch (...) {
                        fFailure_ = current_exception ();
                    }
                }
                return SZ_ERROR_READ;
            }
        };
        mutable MyISeekInStream fInSeekStream_;
        mutable CLookToRead2    fLookStream_{};
        mutable Byte            fLookBuffer_[1 << 14]; // fLookStream_'s
        /*
         *  The block (7z's 'folder') GetData last decompressed, kept for the next call: a solid archive packs many files into one
         *  block, so reading each would otherwise decompress the whole block again. These are SzArEx_Extract's in/out arguments,
         *  which replaces the block when a file is in another. Holds memory the size of the block until then.
         */
        mutable Byte*  fBlock_{nullptr}; // from kAlloc_; nullptr: none held
        mutable UInt32 fBlockIndex_{};   // any value while fBlock_ is nullptr
        mutable size_t fBlockSize_{};    // ditto

    public:
        Rep_ (const Streams::InputStream::Ptr<byte>& in)
            : fInSeekStream_{in}
        {
            ::SzArEx_Init (&fDB_);

            ::LookToRead2_CreateVTable (&fLookStream_, false);
            fLookStream_.buf     = fLookBuffer_;
            fLookStream_.bufSize = std::size (fLookBuffer_);
            LookToRead2_INIT (&fLookStream_);
            fLookStream_.realStream = &fInSeekStream_;

            try {
                ThrowIfFailed_ (::SzArEx_Open (&fDB_, &fLookStream_.vt, &kAlloc_, &kAlloc_));
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
                // no ~Rep_ for a constructor that throws; harmless after a failed SzArEx_Open, which frees fDB_ itself
                ::SzArEx_Free (&fDB_, &kAlloc_);
                throw;
            }
        }
        ~Rep_ ()
        {
            IAlloc_Free (&kAlloc_, fBlock_);
            ::SzArEx_Free (&fDB_, &kAlloc_);
        }

    private:
        // after a call into the SDK: what a stream callback threw, else what err says
        void ThrowIfFailed_ (SRes err) const
        {
            fInSeekStream_.RethrowStreamFailure ();
            ThrowIfSzErr_ (err);
        }

    public:
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
            if (SRes err = ::SzArEx_Extract (&fDB_, &fLookStream_.vt, *idx, &fBlockIndex_, &fBlock_, &fBlockSize_, &offset,
                                             &outSizeProcessed, &kAlloc_, &kAlloc_);
                err != SZ_OK) [[unlikely]] {
                // a failed decode leaves fBlock_ partly filled, though marked as fBlockIndex_ - drop it, or the next call would use it
                IAlloc_Free (&kAlloc_, fBlock_);
                fBlock_ = nullptr;
                ThrowIfFailed_ (err);
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
