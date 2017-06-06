/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/CodePage.h"
#include "../Characters/String_Constant.h"
#include "../Containers/Common.h"
#include "../Debug/AssertExternallySynchronizedLock.h"
#include "../Execution/Common.h"
#include "../Execution/OperationNotSupportedException.h"
#include "../Memory/SmallStackBuffer.h"

#include "TextReader.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Streams;

using Characters::String;
using Characters::String_Constant;
using Execution::make_unique_lock;
using Memory::Byte;

namespace {
    const codecvt_utf8<wchar_t> kConverter_; // safe to keep static because only read-only const methods used
}

class TextReader::BinaryStreamRep_ : public InputStream<Character>::_IRep, protected Debug::AssertExternallySynchronizedLock {
public:
    BinaryStreamRep_ (const InputStream<Byte>& src)
        : fSource_ (src)
        , fOffset_ (0)
    {
    }

protected:
    virtual bool IsSeekable () const override
    {
        return false;
    }

    virtual size_t Read (Character* intoStart, Character* intoEnd) override
    {
        Require ((intoStart == intoEnd) or (intoStart != nullptr));
        Require ((intoStart == intoEnd) or (intoEnd != nullptr));
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Memory::SmallStackBuffer<wchar_t>                  outBuf{size_t (intoEnd - intoStart)};
        Memory::SmallStackBuffer<Byte>                     inBuf{size_t (intoEnd - intoStart)}; // wag at size

        size_t      inBytes = fSource_.Read (begin (inBuf), end (inBuf));
        const char* firstB  = reinterpret_cast<const char*> (begin (inBuf));
        const char* endB    = firstB + inBytes;
        Assert (endB <= reinterpret_cast<const char*> (end (inBuf)));
        const char*                   cursorB   = firstB;
        wchar_t*                      outCursor = begin (outBuf);
        codecvt_utf8<wchar_t>::result r         = kConverter_.in (fMBState_, firstB, endB, cursorB, std::begin (outBuf), std::end (outBuf), outCursor);
        Assert (std::begin (outBuf) <= outCursor and outCursor <= std::end (outBuf));
        if (r == codecvt_utf8<wchar_t>::error) {
            // not sure what to throw!
            // This makes sense to throw, but previous code didnt -- and we currently use this with bad code pages in some regression tests.
            // @see https://stroika.atlassian.net/browse/STK-274
            WeakAsserteNotReached ();
            //            Execution::Throw (Execution::StringException (String_Constant (L"Error converting characters codepage")));
        }
        // ignore partial - OK - data just went into
        if (outCursor == std::begin (outBuf) and r == codecvt_utf8<wchar_t>::partial) {
            // see if we can read more from binary source
        }
        Assert (end (outBuf) - outCursor <= (intoEnd - intoStart));
        Character* resultCharP = intoStart;
        for (const wchar_t* p = std::begin (outBuf); p < outCursor; ++p) {
            Assert (resultCharP < intoEnd);
            *resultCharP = *p;
            ++resultCharP;
        }
        size_t n = resultCharP - intoStart;
        fOffset_ += n;
        return n;
    }

    virtual Memory::Optional<size_t> ReadSome (Character* intoStart, Character* intoEnd) override
    {
        // https://stroika.atlassian.net/browse/STK-567 EXPERIMENTAL DRAFT API
        Require ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1);
        WeakAssert (false);
        // @todo - FIX TO REALLY CHECK
        return {};
    }

    virtual SeekOffsetType GetReadOffset () const override
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        return fOffset_;
    }

    virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        AssertNotReached ();
        return fOffset_;
    }

protected:
    InputStream<Byte> fSource_;
    mbstate_t         fMBState_{};
    SeekOffsetType    fOffset_;
};

class TextReader::BaseSeekingBinaryStreamRep_ : public BinaryStreamRep_ {
    using inherited = BinaryStreamRep_;

public:
    BaseSeekingBinaryStreamRep_ (const InputStream<Byte>& src)
        : BinaryStreamRep_ (src)
    {
    }

protected:
    virtual bool IsSeekable () const override
    {
        return true;
    }

    void DoSlowSeek_ (SeekOffsetType offset)
    {
        fOffset_ = 0;
        fSource_.Seek (0);
        for (SeekOffsetType i = 0; i < offset; ++i) {
            Character c;
            if (Read (&c, &c + 1) == 0) {
                Execution::Throw (std::range_error ("seek"));
            }
        }
        Ensure (fOffset_ == offset);
    }

    virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        switch (whence) {
            case Whence::eFromStart: {
                if (offset < 0) {
                    Execution::Throw (std::range_error ("seek"));
                }
                DoSlowSeek_ (static_cast<SeekOffsetType> (offset));
            } break;
            case Whence::eFromCurrent: {
                Streams::SeekOffsetType       curOffset = fOffset_;
                Streams::SignedSeekOffsetType newOffset = curOffset + offset;
                if (newOffset < 0) {
                    Execution::Throw (std::range_error ("seek"));
                }
                SeekOffsetType uNewOffset = static_cast<SeekOffsetType> (newOffset);
                DoSlowSeek_ (static_cast<size_t> (uNewOffset));
            } break;
            case Whence::eFromEnd: {
                Execution::Throw (Execution::StringException (L"BaseSeekingBinaryStreamRep_ cannot seek from end"));
            } break;
        }
        return fOffset_;
    }
};

class TextReader::CachingSeekableBinaryStreamRep_ : public BinaryStreamRep_ {
    using inherited = BinaryStreamRep_;

public:
    CachingSeekableBinaryStreamRep_ (const InputStream<Byte>& src)
        : BinaryStreamRep_ (src)
    {
    }

protected:
    virtual bool IsSeekable () const override
    {
        return true;
    }

    virtual size_t Read (Character* intoStart, Character* intoEnd) override
    {
        Require ((intoStart == intoEnd) or (intoStart != nullptr));
        Require ((intoStart == intoEnd) or (intoEnd != nullptr));

        // if already cached, return from cache. If not already cached, add to cache
        if (fOffset_ < fCache_.size ()) {
            // return data from cache
            size_t nToRead     = intoEnd - intoStart;
            size_t nInBufAvail = fCache_.size () - static_cast<size_t> (fOffset_);
            nToRead            = min (nToRead, nInBufAvail);
            Assert (nToRead > 0);
            for (size_t i = 0; i < nToRead; ++i) {
                intoStart[i] = fCache_[i + static_cast<size_t> (fOffset_)];
            }
            fOffset_ += nToRead;
            return nToRead;
        }
        SeekOffsetType origOffset = fOffset_;
        size_t         n          = inherited::Read (intoStart, intoEnd);
        if (n != 0) {
            if (origOffset + n > numeric_limits<size_t>::max ()) {
                // size_t can be less bits than SeekOffsetType, in which case we cannot cahce all in RAM
                Execution::Throw (std::range_error ("seek past max size for size_t"));
            }
            size_t newCacheSize = static_cast<size_t> (origOffset + n);
            Assert (fCache_.size () == static_cast<size_t> (origOffset));
            Assert (newCacheSize > fCache_.size ());
            Containers::ReserveSpeedTweekAddN (fCache_, n);
            fCache_.resize (newCacheSize);
            for (size_t i = 0; i < n; ++i) {
                fCache_[i + static_cast<size_t> (origOffset)] = intoStart[i].As<wchar_t> ();
            }
        }
        return n;
    }

    virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        switch (whence) {
            case Whence::eFromStart: {
                if (offset < 0) {
                    Execution::Throw (std::range_error ("seek"));
                }
                SeekTo_ (static_cast<SeekOffsetType> (offset));
            } break;
            case Whence::eFromCurrent: {
                Streams::SeekOffsetType       curOffset = fOffset_;
                Streams::SignedSeekOffsetType newOffset = curOffset + offset;
                if (newOffset < 0) {
                    Execution::Throw (std::range_error ("seek"));
                }
                SeekOffsetType uNewOffset = static_cast<SeekOffsetType> (newOffset);
                SeekTo_ (static_cast<size_t> (uNewOffset));
            } break;
            case Whence::eFromEnd: {
                Character c;
                if (Read (&c, &c + 1) == 0) {
                    break; // read til EOF
                }
                SeekTo_ (fOffset_ + offset);
            } break;
        }
        return fOffset_;
    }

private:
    void SeekFowardTo_ (SeekOffsetType offset)
    {
        // easy - keep reading
        while (fOffset_ < offset) {
            Character c;
            if (Read (&c, &c + 1) == 0) {
                Execution::Throw (std::range_error ("seek"));
            }
        }
        Ensure (fOffset_ == offset);
    }

    void SeekBackwardTo_ (SeekOffsetType offset)
    {
        fOffset_ = offset;
    }

    void SeekTo_ (SeekOffsetType offset)
    {
        if (offset > fOffset_) {
            SeekFowardTo_ (offset);
        }
        else if (offset < fOffset_) {
            SeekBackwardTo_ (offset);
        }
        Ensure (fOffset_ == offset);
    }

private:
    vector<wchar_t> fCache_;
};

class TextReader::IterableAdapterStreamRep_ : public InputStream<Character>::_IRep, private Debug::AssertExternallySynchronizedLock {
public:
    IterableAdapterStreamRep_ (const Traversal::Iterable<Character>& src)
        : fSource_ (src)
        , fSrcIter_ (fSource_.begin ())
    {
    }

protected:
    virtual bool IsSeekable () const override
    {
        return false;
    }
    virtual size_t Read (Character* intoStart, Character* intoEnd) override
    {
        Require (intoEnd - intoStart >= 1);
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Character*                                         outI = intoStart;
        for (; fSrcIter_ != fSource_.end () and outI != intoEnd; ++fSrcIter_, outI++) {
            *outI = *fSrcIter_;
            fOffset_++;
        }
        return outI - intoStart;
    }
    virtual Memory::Optional<size_t> ReadSome (Character* intoStart, Character* intoEnd) override
    {
        // https://stroika.atlassian.net/browse/STK-567 EXPERIMENTAL DRAFT API
        ///      @todo  NOTE - we may need CTOR flag saying that iterating is non-blocking on type passed in!!!!
        Require ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1);
        if (intoStart == nullptr) {
            Traversal::Iterator<Character> srcIt = fSrcIter_;
            size_t                         cnt{};
            for (; srcIt != fSource_.end (); ++srcIt, ++cnt)
                ;
            return srcIt;
        }
        else {
            return Read (intoStart, intoEnd); // same presuming the input interable doesn't block, which isn't definitionally guaranteed, but typically true, so assume for now
        }
    }
    virtual SeekOffsetType GetReadOffset () const override
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        return fOffset_;
    }
    virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
    {
        RequireNotReached (); // not seekable
        return 0;
    }

private:
    Traversal::Iterable<Character> fSource_;
    Traversal::Iterator<Character> fSrcIter_;
    size_t                         fOffset_{};
};

/*
 ********************************************************************************
 ******************************* Streams::TextReader ****************************
 ********************************************************************************
 */
TextReader::TextReader (const InputStream<Byte>& src, bool seekable)
    : InputStream<Character> (seekable ? make_shared<CachingSeekableBinaryStreamRep_> (src) : make_shared<BinaryStreamRep_> (src))
{
    Assert (this->IsSeekable () == seekable);
}

TextReader::TextReader (const Memory::BLOB& src)
    : TextReader (src.As<InputStream<Byte>> (), true)
{
}

TextReader::TextReader (const Traversal::Iterable<Character>& src)
    : InputStream<Character> (make_shared<IterableAdapterStreamRep_> (src))
{
}
