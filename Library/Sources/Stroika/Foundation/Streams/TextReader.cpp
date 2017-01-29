/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/CodePage.h"
#include    "../Containers/Common.h"
#include    "../Debug/AssertExternallySynchronizedLock.h"
#include    "../Execution/Common.h"
#include    "../Execution/OperationNotSupportedException.h"
#include    "../Memory/SmallStackBuffer.h"

#include    "TextReader.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;

using   Characters::String;
using   Execution::make_unique_lock;
using   Memory::Byte;





class   TextReader::BinaryStreamRep_ : public InputStream<Character>::_IRep, private Debug::AssertExternallySynchronizedLock  {
public:
    BinaryStreamRep_ (const InputStream<Byte>& src)
        : fSource_ (src)
        , fTmpHackTextRemaining_ ()
        , fOffset_ (0)
    {
    }

protected:
    virtual bool    IsSeekable () const override
    {
        return true;
    }
    virtual size_t    Read (Character* intoStart, Character* intoEnd) override
    {
        Require ((intoStart == intoEnd) or (intoStart != nullptr));
        Require ((intoStart == intoEnd) or (intoEnd != nullptr));
        lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
#if 1
        if (fTmpHackTextRemaining_.empty ()) {
            // only happens once
            Assert (fOffset_ == 0);
            Memory::BLOB    b   =   fSource_.ReadAll ();
            fTmpHackTextRemaining_ = Characters::MapUNICODETextWithMaybeBOMTowstring ((char*) (b.begin ()), (char*) (b.end ()));
        }
        Character* ci = intoStart;
        for (; ci != intoEnd; ) {
            if (fOffset_ >= fTmpHackTextRemaining_.length ()) {
                return (ci - intoStart);
            }
            else {
                *ci = fTmpHackTextRemaining_[fOffset_];
                fOffset_++;
                ++ci;
            }
        }
        return (ci - intoStart);
#else
        Memory::SmallStackBuffer<Byte>  buf (intoEnd - intoStart);
        size_t  n   =   fSource_.Read (buf.begin (), buf.end ());
        size_t  outN    =   0;
        for (size_t i = 0; i < n; ++i) {
            intoStart[i] = Characters::Character ((char) * (buf.begin () + i));
            outN++;
        }
        Ensure (outN <= static_cast<size_t> (intoEnd - intoStart));
        return outN;
#endif
    }

    virtual Memory::Optional<size_t>  ReadSome (Character* intoStart, Character* intoEnd) override
    {
        // https://stroika.atlassian.net/browse/STK-567 EXPERIMENTAL DRAFT API
        Require ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1);
        WeakAssert (false);
        // @todo - FIX TO REALLY CHECK
        return {};
    }

    virtual SeekOffsetType  GetReadOffset () const override
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
        return fOffset_;
    }

    virtual SeekOffsetType  SeekRead (Whence whence, SignedSeekOffsetType offset) override
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
        switch (whence) {
            case    Whence::eFromStart: {
                    if (offset < 0) {
                        Execution::Throw (std::range_error ("seek"));
                    }
                    SeekOffsetType  uOffset =   static_cast<SeekOffsetType> (offset);
                    if (uOffset > (fTmpHackTextRemaining_.size ())) {
                        Execution::Throw (std::range_error ("seek"));
                    }
                    // Note - warning here  legit - our caching strategy wtih string is bogus and wont work with large streams
                    fOffset_ = static_cast<size_t> (offset);
                }
                break;
            case    Whence::eFromCurrent: {
                    Streams::SeekOffsetType         curOffset   =   fOffset_;
                    Streams::SignedSeekOffsetType   newOffset   =   curOffset + offset;
                    if (newOffset < 0) {
                        Execution::Throw (std::range_error ("seek"));
                    }
                    SeekOffsetType  uNewOffset =   static_cast<SeekOffsetType> (newOffset);
                    if (uNewOffset > (fTmpHackTextRemaining_.size ())) {
                        Execution::Throw (std::range_error ("seek"));
                    }
                    // Note - warning here  legit - our caching strategy wtih string is bogus and wont work wtih large streams
                    fOffset_ = static_cast<size_t> (uNewOffset);
                }
                break;
            case    Whence::eFromEnd: {
                    Streams::SeekOffsetType         curOffset   =   fOffset_;
                    Streams::SignedSeekOffsetType   newOffset   =   fTmpHackTextRemaining_.size () + offset;
                    if (newOffset < 0) {
                        Execution::Throw (std::range_error ("seek"));
                    }
                    SeekOffsetType  uNewOffset =   static_cast<SeekOffsetType> (newOffset);
                    if (uNewOffset > (fTmpHackTextRemaining_.size ())) {
                        Execution::Throw (std::range_error ("seek"));
                    }
                    // Note - warning here  legit - our caching strategy wtih string is bogus and wont work wtih large streams
                    fOffset_ = static_cast<size_t> (uNewOffset);
                }
                break;
        }
        Ensure ((0 <= fOffset_) and (fOffset_ <= fTmpHackTextRemaining_.size ()));
        return fOffset_;
    }

private:
    InputStream<Byte>           fSource_;
    String                      fTmpHackTextRemaining_;
    size_t                      fOffset_;
};





class   TextReader::IterableAdapterStreamRep_ : public InputStream<Character>::_IRep, private Debug::AssertExternallySynchronizedLock  {
public:
    IterableAdapterStreamRep_ (const Traversal::Iterable<Character>& src)
        : fSource_ (src)
        , fSrcIter_ (fSource_.begin ())
    {
    }

protected:
    virtual bool    IsSeekable () const override
    {
        return false;
    }
    virtual size_t    Read (Character* intoStart, Character* intoEnd) override
    {
        Require (intoEnd - intoStart >= 1);
        lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
        Character*  outI    =   intoStart;
        for (; fSrcIter_ != fSource_.end () and outI != intoEnd; ++fSrcIter_, outI++) {
            *outI = *fSrcIter_;
            fOffset_++;
        }
        return outI - intoStart;
    }
    virtual Memory::Optional<size_t>  ReadSome (Character* intoStart, Character* intoEnd) override
    {
        // https://stroika.atlassian.net/browse/STK-567 EXPERIMENTAL DRAFT API
        ///      @todo  NOTE - we may need CTOR flag saying that iterating is non-blocking on type passed in!!!!
        Require ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1);
        if (intoStart == nullptr) {
            Traversal::Iterator<Character>  srcIt = fSrcIter_;
            size_t  cnt{};
            for (; srcIt != fSource_.end (); ++srcIt, ++cnt)
                ;
            return srcIt;
        }
        else {
            return Read (intoStart, intoEnd);       // same presuming the input interable doesn't block, which isn't definitionally guaranteed, but typically true, so assume for now
        }
    }
    virtual SeekOffsetType  GetReadOffset () const override
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
        return fOffset_;
    }
    virtual SeekOffsetType  SeekRead (Whence whence, SignedSeekOffsetType offset) override
    {
        RequireNotReached ();   // not seekable
        return 0;
    }

private:
    Traversal::Iterable<Character>  fSource_;
    Traversal::Iterator<Character>  fSrcIter_;
    size_t                          fOffset_ {};
};





/*
 ********************************************************************************
 ******************************* Streams::TextReader ****************************
 ********************************************************************************
 */
TextReader::TextReader (const InputStream<Byte>& src)
    : InputStream<Character> (make_shared<BinaryStreamRep_> (src))
{
}

TextReader::TextReader (const Memory::BLOB& src)
    : TextReader (src.As<InputStream<Byte>> ())
{
}

TextReader::TextReader (const Traversal::Iterable<Character>& src)
    : InputStream<Character> (make_shared<IterableAdapterStreamRep_> (src))
{
}
