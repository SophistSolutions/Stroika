/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <io.h>
#elif   qPlatform_POSIX
#include    <unistd.h>
#endif
#include    <cstdlib>

#include    "../../Execution/Exceptions.h"
#include    "../../Execution/OperationNotSupportedException.h"

#include    "SocketStream.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;



// Very primitive, and unsatistifacotry - but perhaps usable -draft implemenation
//      -- LGP 2011-09-24



class   SocketStream::IRep_ : public BinaryInputOutputStream::_IRep {
public:
    IRep_ (Socket sd)
        : BinaryInputOutputStream::_IRep ()
        , fSD_ (sd)
    {
    }
    virtual bool    IsSeekable () const override
    {
        return false;
    }
    virtual SeekOffsetType  GetReadOffset () const override
    {
        RequireNotReached ();   // not seekable
        return 0;
    }
    virtual SeekOffsetType  SeekRead (Whence whence, SignedSeekOffsetType offset) override
    {
        RequireNotReached ();   // not seekable
        return 0;
    }
    virtual size_t  Read (SeekOffsetType* offset, Byte* intoStart, Byte* intoEnd) override
    {
        Require (offset == nullptr);    // not seekable
        return fSD_.Read (intoStart, intoEnd);
    }
    virtual SeekOffsetType  GetWriteOffset () const override
    {
        RequireNotReached ();   // not seekable
        return 0;
    }
    virtual SeekOffsetType  SeekWrite (Whence whence, SignedSeekOffsetType offset) override
    {
        RequireNotReached ();   // not seekable
        return 0;
    }
    virtual void     Write (const Byte* start, const Byte* end) override
    {
        fSD_.Write (start, end);
    }

    virtual void     Flush () override
    {
        // socket has no flush API, so write must do the trick...
    }

private:
    Socket  fSD_;
};






/*
 ********************************************************************************
 **************************** Streams::SocketStream *****************************
 ********************************************************************************
 */
SocketStream::SocketStream (Socket sd)
    : BinaryTiedStreams (make_shared<IRep_> (sd))
{
}
