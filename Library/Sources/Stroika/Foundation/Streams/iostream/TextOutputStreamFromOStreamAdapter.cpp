/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Characters/String_Constant.h"

#include    "../../Execution/OperationNotSupportedException.h"

#include    "TextOutputStreamFromOStreamAdapter.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Streams;
using   namespace   Stroika::Foundation::Streams::iostream;



class   TextOutputStreamFromOStreamAdapter::IRep_ : public TextOutputStream::_IRep, public Seekable::_IRep {
public:
    IRep_ (wostream& originalStream)
        : fCriticalSection_ ()
        , fOriginalStream_ (originalStream)
    {
    }

protected:
    virtual void    Write (const Character* start, const Character* end) override
    {
        Require (start != nullptr or start == end);
        Require (end != nullptr or start == end);

        lock_guard<recursive_mutex>  critSec (fCriticalSection_);

        fOriginalStream_.write (reinterpret_cast<const wchar_t*> (start), end - start);
        if (fOriginalStream_.fail ()) {
            Execution::DoThrow (Execution::StringException (String_Constant (L"Failed to write from ostream")));
        }
    }

#if 0
    // NYI in base class
    virtual void    Flush () override
    {
        lock_guard<recursive_mutex>  critSec (fCriticalSection_);
        fOriginalStream_.flush ();
        if (fOriginalStream_.fail ()) {
            Execution::DoThrow (Execution::StringException (L"Failed to flush ostream"));
        }
    }
#endif

    virtual SeekOffsetType  GetOffset () const override
    {
        // instead of tellg () - avoids issue with EOF where fail bit set???
        lock_guard<recursive_mutex>  critSec (fCriticalSection_);
        return fOriginalStream_.rdbuf ()->pubseekoff (0, ios_base::cur, ios_base::out);
    }

    virtual SeekOffsetType  Seek (Whence whence, SignedSeekOffsetType offset) override
    {
        lock_guard<recursive_mutex>  critSec (fCriticalSection_);
        switch (whence) {
            case    Whence::eFromStart:
                fOriginalStream_.seekp (offset, ios::beg);
                break;
            case    Whence::eFromCurrent:
                fOriginalStream_.seekp (offset, ios::cur);
                break;
            case    Whence::eFromEnd:
                fOriginalStream_.seekp (offset, ios::end);
                break;
        }
        return fOriginalStream_.tellp ();
    }

private:
    mutable recursive_mutex     fCriticalSection_;
    wostream&                   fOriginalStream_;
};






/*
 ********************************************************************************
 ********** Streams::iostream::TextOutputStreamFromOStreamAdapter ***************
 ********************************************************************************
 */
TextOutputStreamFromOStreamAdapter::TextOutputStreamFromOStreamAdapter (std::wostream& originalStream)
    : TextOutputStream (shared_ptr<_IRep> (new IRep_ (originalStream)))
{
}
