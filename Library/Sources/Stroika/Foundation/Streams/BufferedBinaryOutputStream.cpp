/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <mutex>

#include    "../Containers/Common.h"
#include    "../Execution/Common.h"
#include    "../Execution/Exceptions.h"

#include    "BufferedBinaryOutputStream.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;

using   Execution::make_unique_lock;



namespace   {
    const   size_t  kMinBufSize_    =   1 * 1024;
    const   size_t  kDefaultBufSize =   16 * 1024;
}



class   BufferedBinaryOutputStream::IRep_ : public BinaryOutputStream::_IRep {
public:
    IRep_ (const BinaryOutputStream& realOut)
        : BinaryOutputStream::_IRep ()
        , fCriticalSection_ ()
        , fBuffer_ ()
        , fRealOut_ (realOut)
        , fAborted_ (false)
    {
        fBuffer_.reserve (kDefaultBufSize);
    }
    ~IRep_ ()
    {
        IgnoreExceptionsForCall (Flush ());
        Ensure (fBuffer_.size () == 0); // advisory - not quite right - could happen if a flush exception was eaten (@todo clean this up)
    }

public:
    nonvirtual  size_t  GetBufferSize () const
    {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
        MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
        auto    critSec { make_unique_lock (fCriticalSection_) };
#endif
        return fBuffer_.capacity ();
    }
    nonvirtual  void    SetBufferSize (size_t bufSize)
    {
        auto    critSec { make_unique_lock (fCriticalSection_) };
        bufSize = max (bufSize, kMinBufSize_);
        if (bufSize < fBuffer_.size ()) {
            Flush ();
        }
        fBuffer_.reserve (bufSize);
    }

public:
    // Throws away all data about to be written (buffered). Once this is called, its illegal to call Flush or another write
    nonvirtual  void    Abort ()
    {
        auto    critSec { make_unique_lock (fCriticalSection_) };
        fAborted_ = true;   // for debug sake track this
        fBuffer_.clear ();
    }

    //
    virtual  void    Flush () override
    {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
        MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
        auto    critSec { make_unique_lock (fCriticalSection_) };
#endif
        if (fAborted_) {
            fBuffer_.clear ();
        }
        else {
            if (not fBuffer_.empty ()) {
                fRealOut_.Write (Containers::Start (fBuffer_), Containers::End (fBuffer_));
                fBuffer_.clear ();
            }
            fRealOut_.Flush ();
        }
        Ensure (fBuffer_.empty ());
    }

    // pointer must refer to valid memory at least bufSize long, and cannot be nullptr. BufSize must always be >= 1.
    // Writes always succeed fully or throw.
    virtual void            Write (const Byte* start, const Byte* end) override
    {
        Require (start < end);  // for BinaryOutputStream - this funciton requires non-empty write
        Require (not fAborted_);
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
        MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
        auto    critSec { make_unique_lock (fCriticalSection_) };
#endif
        /*
         * Minimize the number of writes at the possible cost of extra copying.
         *
         * See if there is room in the buffer, and use it up. Only when no more room do we flush.
         */
        size_t  bufSpaceRemaining   =   fBuffer_.capacity () - fBuffer_.size ();
        size_t  size2WriteRemaining =   end - start;

        size_t  copy2Buffer =   min (bufSpaceRemaining, size2WriteRemaining);
#if     qDebug
        size_t  oldCap  =   fBuffer_.capacity ();
#endif
        fBuffer_.insert (fBuffer_.end (), start, start + copy2Buffer);
        Assert (oldCap == fBuffer_.capacity ());

        Assert (size2WriteRemaining >= copy2Buffer);
        size2WriteRemaining -= copy2Buffer;

        /*
         * At this point - either the buffer is full, OR we are done writing. EITHER way - if the buffer is full - we may as well write it now.
         */
        if (fBuffer_.capacity () == fBuffer_.size ()) {
            Flush ();
            Assert (fBuffer_.empty ());
        }
        Assert (oldCap == fBuffer_.capacity ());

        // If the remaining will fit in the buffer, then buffer. But if it won't - no point in using the buffer - just write directly to avoid the copy.
        // And no point - even if equal to buffer size - since it won't save any writes...
        if (size2WriteRemaining == 0) {
            // DONE
        }
        else if (size2WriteRemaining < fBuffer_.capacity ()) {
            fBuffer_.insert (fBuffer_.end (), start + copy2Buffer, end);
        }
        else {
            fRealOut_.Write (start + copy2Buffer, end);
        }
    }

private:
    mutable recursive_mutex             fCriticalSection_;
    vector<Byte>                        fBuffer_;
    BinaryOutputStream                  fRealOut_;
    bool                                fAborted_;
};





/*
 ********************************************************************************
 *********************** Streams::BinaryOutputStream ****************************
 ********************************************************************************
 */
BufferedBinaryOutputStream::BufferedBinaryOutputStream (const BinaryOutputStream& realOut)
    : BinaryOutputStream (_SharedIRep (new IRep_ (realOut)))
{
}

void    BufferedBinaryOutputStream::Abort ()
{
    _SharedIRep rep = _GetRep ();
    IRep_* r = dynamic_cast<IRep_*> (rep.get ());
    AssertNotNull (r);
    r->Abort ();
}
