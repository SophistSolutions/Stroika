/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/CodePage.h"
#include    "../Containers/Common.h"
#include    "../Execution/OperationNotSupportedException.h"
#include    "../Memory/SmallStackBuffer.h"

#include    "TextInputStreamBinaryAdapter.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;






class   TextInputStreamBinaryAdapter::IRep_ : public TextInputStream::_IRep, public Seekable::_IRep {
public:
    IRep_ (const BinaryInputStream& src)
        : fSource_ (src)
        , fTmpHackTextRemaining_ ()
        , fOffset_ (0) {
    }

protected:
    virtual size_t    _Read (Character* intoStart, Character* intoEnd) override {
        Require ((intoStart == intoEnd) or (intoStart != nullptr));
        Require ((intoStart == intoEnd) or (intoEnd != nullptr));
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

    virtual SeekOffsetType  GetOffset () const override {
        return fOffset_;
    }

    virtual SeekOffsetType  Seek (Whence whence, SignedSeekOffsetType offset) override {
        switch (whence) {
            case    Whence::eFromStart: {
                    if (offset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    if (offset > (fTmpHackTextRemaining_.size ())) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    // Note - warning here  legit - our caching strategy wtih string is bogus and wont work with large streams
                    fOffset_ = offset;
                }
                break;
            case    Whence::eFromCurrent: {
                    Streams::SeekOffsetType         curOffset   =   fOffset_;
                    Streams::SignedSeekOffsetType   newOffset   =   curOffset + offset;
                    if (newOffset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    if (newOffset > (fTmpHackTextRemaining_.size ())) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    // Note - warning here  legit - our caching strategy wtih string is bogus and wont work wtih large streams
                    fOffset_ = newOffset;
                }
                break;
            case    Whence::eFromEnd: {
                    Streams::SeekOffsetType         curOffset   =   fOffset_;
                    Streams::SignedSeekOffsetType   newOffset   =   (fTmpHackTextRemaining_.size ()) + offset;
                    if (newOffset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    if (newOffset > (fTmpHackTextRemaining_.size ())) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    // Note - warning here  legit - our caching strategy wtih string is bogus and wont work wtih large streams
                    fOffset_ = newOffset;
                }
                break;
        }
        Ensure ((0 <= fOffset_) and (fOffset_ <= fTmpHackTextRemaining_.size ()));
        return GetOffset ();
    }

private:
    BinaryInputStream   fSource_;
    String              fTmpHackTextRemaining_;
    size_t              fOffset_;
};




/*
 ********************************************************************************
 ************************** Streams::TextInputStream ****************************
 ********************************************************************************
 */
TextInputStreamBinaryAdapter::TextInputStreamBinaryAdapter (const BinaryInputStream& src)
    : TextInputStream (mk_ (src))
{
}

shared_ptr<TextInputStreamBinaryAdapter::_IRep> TextInputStreamBinaryAdapter::mk_ (const BinaryInputStream& src)
{
    return shared_ptr<_IRep> (new IRep_ (src));
}
