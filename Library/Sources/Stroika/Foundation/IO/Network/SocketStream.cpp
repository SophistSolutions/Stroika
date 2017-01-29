/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
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





/*
 ********************************************************************************
 ************************* Streams::SocketStream::Rep_ **************************
 ********************************************************************************
 */
class   SocketStream::Rep_ : public InputOutputStream<Byte>::_IRep {
public:
    Rep_ (Socket sd)
        : InputOutputStream<Byte>::_IRep ()
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
    virtual size_t  Read (Byte* intoStart, Byte* intoEnd) override
    {
        return fSD_.Read (intoStart, intoEnd);
    }
    virtual Memory::Optional<size_t>  ReadSome (ElementType* intoStart, ElementType* intoEnd) override
    {
        // https://stroika.atlassian.net/browse/STK-567 EXPERIMENTAL DRAFT API
        Require ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1);
        WeakAssert (false);
        // @todo - FIX TO REALLY CHECK
        return {};
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
    : inherited (make_shared<Rep_> (sd))
{
}
