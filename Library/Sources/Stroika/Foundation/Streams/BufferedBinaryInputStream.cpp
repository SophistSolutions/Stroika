/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <mutex>

#include    "BufferedBinaryInputStream.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;

using   Execution::make_unique_lock;





class   BufferedBinaryInputStream::IRep_ : public BinaryInputStream::_IRep {
public:
    IRep_ (const BinaryInputStream& realIn)
        : BinaryInputStream::_IRep ()
        , fCriticalSection_ ()
        , fRealIn_ (realIn)
    {
    }

    virtual size_t  Read (Byte* intoStart, Byte* intoEnd) override
    {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
        MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Wfuture-compat\"");
        auto    critSec { make_unique_lock (fCriticalSection_) };
        DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Wfuture-compat\"");
#endif
        return fRealIn_.Read (intoStart, intoEnd);
    }

private:
    mutable mutex         fCriticalSection_;
    BinaryInputStream     fRealIn_;
};






/*
 ********************************************************************************
 ************************ Streams::BinaryInputStream ****************************
 ********************************************************************************
 */
BufferedBinaryInputStream::BufferedBinaryInputStream (const BinaryInputStream& realIn)
    : BinaryInputStream (shared_ptr<_IRep> (new IRep_ (realIn)))
{
}
