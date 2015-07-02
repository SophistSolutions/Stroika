/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Memory/BlockAllocated.h"

#include    "BinaryInputOutputStream.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;




/*
 ********************************************************************************
 ******************* Streams::BinaryInputOutputStream ***************************
 ********************************************************************************
 */
BinaryInputOutputStream::BinaryInputOutputStream (const _SharedIRep& rep)
    : BinaryInputStream (rep)
    , BinaryOutputStream (rep)
{
    //Require (dynamic_cast<const Seekable*>(rep.get ()) == nullptr);
}

#if 0

BinaryInputOutputStream::operator BinaryInputStream () const
{
    return *this;
#if 0
    struct XXX : BinaryInputStream {
        struct   InStr_IRep_ : BinaryInputStream::_IRep {
            DECLARE_USE_BLOCK_ALLOCATION (InStr_IRep_);
            BinaryInputOutputStream::_SharedIRep    fMaster_;
            InStr_IRep_ (const BinaryInputOutputStream::_SharedIRep& masterRep)
                : fMaster_ (masterRep)
            {
            }
            virtual bool    IsSeekable () const override
            {
                return true;
            }
            virtual SeekOffsetType  GetReadOffset () const override
            {
                return fMaster_->ReadGetOffset ();
            }
            virtual SeekOffsetType  SeekRead (Whence whence, SignedSeekOffsetType offset) override
            {
                return fMaster_->ReadSeek (whence, offset);
            }
            virtual size_t  Read (SeekOffsetType* offset, Byte* intoStart, Byte* intoEnd) override
            {
                // @todo implement 'offset' support
                return fMaster_->Read (intoStart, intoEnd);
            }
        };
        static  BinaryInputStream   mk ()
        {
            return BinaryInputStream (make_shared<XXX::InStr_IRep_> (s)));
        }
    };
return XXX:
           mk (_GetRep ());
#endif
}

BinaryInputOutputStream::operator BinaryOutputStream () const
{
    return *this;
#if 0
#if     qCompilerAndStdLib_two_levels_nesting_Protected_Access_Buggy
    using TMP = Seekable::_IRep;
    struct   OutStr_IRep_ : BinaryOutputStream::_IRep, TMP {
#else
    struct   OutStr_IRep_ : BinaryOutputStream::_IRep, Seekable::_IRep {
#endif
        DECLARE_USE_BLOCK_ALLOCATION (OutStr_IRep_);
        BinaryInputOutputStream::_SharedIRep    fMaster_;
        OutStr_IRep_ (const BinaryInputOutputStream::_SharedIRep& masterRep)
            : fMaster_ (masterRep)
        {
        }
        virtual void    Write (const Byte* start, const Byte* end) override
        {
            fMaster_->Write (start, end);
        }
        virtual void     Flush () override
        {
            fMaster_->Flush ();
        }
        virtual SeekOffsetType  GetOffset () const override
        {
            return fMaster_->WriteGetOffset ();
        }
        virtual SeekOffsetType    Seek (Whence whence, SignedSeekOffsetType offset) override
        {
            return fMaster_->WriteSeek (whence, offset);
        }
    };
    return BinaryOutputStream (BinaryOutputStream::_SharedIRep (new OutStr_IRep_ (_GetRep ())));
#endif
}
#endif
