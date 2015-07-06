/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <mutex>

#include    "BufferedBinaryInputStream.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;

using   Execution::make_unique_lock;





class   BufferedBinaryInputStream::Rep_ : public InputStream<Byte>::_IRep {
public:
    Rep_ (const InputStream<Byte>& realIn)
        : InputStream<Byte>::_IRep ()
        , fCriticalSection_ ()
        , fRealIn_ (realIn)
    {
    }
    virtual bool    IsSeekable () const override
    {
        return false;      // @todo - COULD be seekable if underlying fRealIn_ was!!!
    }
    virtual SeekOffsetType  GetReadOffset () const override
    {
        RequireNotReached ();
        return 0;
    }
    virtual SeekOffsetType  SeekRead (Whence whence, SignedSeekOffsetType offset) override
    {
        RequireNotReached ();
        return 0;
    }
    virtual size_t  Read (SeekOffsetType* offset, Byte* intoStart, Byte* intoEnd) override
    {
        // @todo implement 'offset' support
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
        MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
        auto    critSec { make_unique_lock (fCriticalSection_) };
#endif
        return fRealIn_.Read (intoStart, intoEnd);
    }

private:
    mutable mutex         fCriticalSection_;
    InputStream<Byte>     fRealIn_;
};






/*
 ********************************************************************************
 ************************ Streams::BinaryInputStream ****************************
 ********************************************************************************
 */
BufferedBinaryInputStream::BufferedBinaryInputStream (const InputStream<Byte>& realIn)
    : InputStream<Byte> (make_shared<Rep_> (realIn))
{
}
