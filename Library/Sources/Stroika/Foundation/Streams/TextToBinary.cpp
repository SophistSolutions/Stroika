/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/CodeCvt.h"
#include "Stroika/Foundation/Characters/TextConvert.h"
#include "Stroika/Foundation/Containers/Support/ReserveTweaks.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Execution/Common.h"
#include "Stroika/Foundation/Execution/OperationNotSupportedException.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Memory/InlineBuffer.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"
#include "Stroika/Foundation/Streams/BufferedInputStream.h"
#include "Stroika/Foundation/Streams/IterableToInputStream.h"

#include "TextToBinary.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Streams;
using namespace Stroika::Foundation::Streams::TextToBinary;

using Characters::String;
using Debug::AssertExternallySynchronizedMutex;
using Memory::InlineBuffer;
using Memory::StackBuffer;

namespace {
    class Rep_ final : public InputStream::IRep<byte>, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
    public:
        Rep_ (const InputStream::Ptr<Character>& src)
            : fSrc_{src}
        {
        }

    protected:
        virtual bool IsSeekable () const override
        {
            // @todo https://stroika.atlassian.net/browse/STK-1028 - make optionally seekable
            return false; // avoid seekability at first cuz makes impl much more costly/complex
        }
        virtual void CloseRead () override
        {
            if (fSrc_ != nullptr) {
                fSrc_.Close ();
            }
            Ensure (not IsOpenRead ());
            Ensure (fSrc_ == nullptr);
        }
        virtual bool IsOpenRead () const override
        {
            return fSrc_ != nullptr;
        }
        virtual optional<size_t> AvailableToRead () override
        {
            // this is easy, because an upstream character always translates into at least one byte, so just check if any upstream characters
            if (fSrcBufferedSpan_.size () > 0) {
                return fSrcBufferedSpan_.size ();
            }
            return fSrc_.AvailableToRead ();
        }
        virtual optional<SeekOffsetType> RemainingLength () override
        {
            Require (IsOpenRead ());
            return nullopt; // possible, but not easy...
        }
        virtual optional<span<byte>> Read (span<byte> intoBuffer, NoDataAvailableHandling blockFlag) override
        {
            // NB: CURRENTLY HARDWIRE CONVERT TO UTF8- but later allow params to specify convert-to
            Require (IsOpenRead ());
            Require (not intoBuffer.empty ());
            // first see if any partially translated bytes to return
        Again:
            if (not fSrcBufferedSpan_.empty ()) [[unlikely]] {
                auto copiedIntoSpan = Memory::CopyBytes (fSrcBufferedSpan_.subspan (0, min (fSrcBufferedSpan_.size (), intoBuffer.size ())), intoBuffer);
                Assert (copiedIntoSpan.size () >= 1);
                fSrcBufferedSpan_ = fSrcBufferedSpan_.subspan (copiedIntoSpan.size ()); // skip copied bytes
                _fOffset += copiedIntoSpan.size ();
                return intoBuffer.subspan (0, copiedIntoSpan.size ());
            }
            // more likely - KISS for now - read one character from upstream, patch fSrcBufferedSpan_, and try again
            Assert (fSrcBufferedSpan_.empty ());
            Character readBuf[1];
            if (auto o = fSrc_.GetRepRWRef ().Read (span{readBuf}, blockFlag)) {
                if (size_t nChars = o->size ()) {
                    Assert (nChars == 1); // for now cuz we have small buffer - could enlarge
                    fSrcBufferedSpan_ = Memory::SpanBytesCast<span<byte>> (Characters::UTFConvert::kThe.ConvertSpan (
                        span{readBuf, nChars}, Memory::SpanBytesCast<span<char8_t>> (span{fSrcBufferedRawBytes_})));
                    Assert (not fSrcBufferedSpan_.empty ()); // cuz at least one character
                    goto Again;                              // cuz it has data it can pull
                }
                else {
                    // if we got here (Read worked, but returned zero characters), nothing in our buf, and nothing upstream - EOF
                    return span<byte>{};
                }
            }
            else {
                Assert (blockFlag == eDontBlock);
                return nullopt; // nothing pre-read, and nothing available upstream, so nothing yet
            }
        }
        virtual SeekOffsetType GetReadOffset () const override
        {
            AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
            Require (IsOpenRead ());
            return _fOffset;
        }

    protected:
        InputStream::Ptr<Character>                  fSrc_;
        byte                                         fSrcBufferedRawBytes_[4]; // not used directly, but always through fSrcBufferedSpan_
        span<byte>                                   fSrcBufferedSpan_;
        SeekOffsetType                               _fOffset{0};
        qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
    };
}

/*
 ********************************************************************************
 *********************** Streams::TextToBinary::Reader::New *********************
 ********************************************************************************
 */
auto TextToBinary::Reader::New (const InputStream::Ptr<Character>& srcStream, optional<SeekableFlag> seekable) -> InputStream::Ptr<byte>
{
    auto result = InputStream::Ptr<byte>{Memory::MakeSharedPtr<Rep_> (srcStream)};
    // @todo - this could be more efficient by working embedding some of this logic into Rep_
    if (seekable == SeekableFlag::eSeekable) {
        result = BufferedInputStream::New (result, SeekableFlag::eSeekable);
        Ensure (result.IsSeekable ());
    }
    return result;
}

auto TextToBinary::Reader::New (const Traversal::Iterable<Character>& srcText) -> InputStream::Ptr<byte>
{
    // @todo - Could make this more efficient (by combining into one object) - but for now KISS
    auto result = New (IterableToInputStream::New<Character> (srcText));
    Ensure (result.IsSeekable ());
    return result;
}
