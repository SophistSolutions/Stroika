/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Characters/String_Constant.h"

#include    "../../Execution/Common.h"
#include    "../../Execution/OperationNotSupportedException.h"

#include    "TextOutputStreamFromOStreamAdapter.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Streams;
using   namespace   Stroika::Foundation::Streams::iostream;


using   Execution::make_unique_lock;



class   TextOutputStreamFromOStreamAdapter::Rep_ : public OutputStream<Character>::_IRep {
public:
    Rep_ (wostream& originalStream)
        : fCriticalSection_ ()
        , fOriginalStream_ (originalStream)
    {
    }

protected:
    virtual void    Write (const Character* start, const Character* end) override
    {
        Require (start != nullptr or start == end);
        Require (end != nullptr or start == end);

        auto    critSec { make_unique_lock (fCriticalSection_) };

        fOriginalStream_.write (reinterpret_cast<const wchar_t*> (start), end - start);
        if (fOriginalStream_.fail ()) {
            Execution::DoThrow (Execution::StringException (String_Constant (L"Failed to write from ostream")));
        }
    }
    virtual void    Flush () override
    {
        auto    critSec { make_unique_lock (fCriticalSection_) };
        fOriginalStream_.flush ();
        if (fOriginalStream_.fail ()) {
            Execution::DoThrow (Execution::StringException (L"Failed to flush ostream"));
        }
    }
    virtual bool    IsSeekable () const override
    {
        return true;
    }
    virtual SeekOffsetType  GetWriteOffset () const override
    {
        // instead of tellg () - avoids issue with EOF where fail bit set???
        auto    critSec { make_unique_lock (fCriticalSection_) };
        return fOriginalStream_.rdbuf ()->pubseekoff (0, ios_base::cur, ios_base::out);
    }
    virtual SeekOffsetType  SeekWrite (Whence whence, SignedSeekOffsetType offset) override
    {
        auto    critSec { make_unique_lock (fCriticalSection_) };
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
    : OutputStream<Character> (make_shared<Rep_> (originalStream))
{
}
