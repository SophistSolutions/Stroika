/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <mutex>

#include    "BufferedBinaryInputStream.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;




class   BufferedBinaryInputStream::IRep_ : public BinaryInputStream::_IRep {
public:
    IRep_ (const BinaryInputStream& realIn)
        : BinaryInputStream::_IRep ()
        , fCriticalSection_ ()
        , fRealIn_ (realIn) {
    }

    virtual size_t  Read (Byte* intoStart, Byte* intoEnd) override {
        lock_guard<mutex>  critSec (fCriticalSection_);
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
