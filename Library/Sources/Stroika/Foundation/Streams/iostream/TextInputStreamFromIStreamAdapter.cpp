/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Characters/String_Constant.h"
#include    "../../Execution/Common.h"
#include    "../../Execution/OperationNotSupportedException.h"

#include    "TextInputStreamFromIStreamAdapter.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Streams;
using   namespace   Stroika::Foundation::Streams::iostream;

using   Execution::make_unique_lock;



class   TextInputStreamFromIStreamAdapter::Rep_ : public TextInputStream::_IRep {
public:
    Rep_ (wistream& originalStream)
        : fCriticalSection_ ()
        , fOriginalStream_ (originalStream)
    {
    }
    virtual bool    IsSeekable () const override
    {
        return true;
    }
    virtual size_t    Read (SeekOffsetType* offset, Character* intoStart, Character* intoEnd) override
    {
        // @todo 'offset' param NYI
        RequireNotNull (intoStart);
        RequireNotNull (intoEnd);
        Require (intoStart < intoEnd);

        auto    critSec { make_unique_lock (fCriticalSection_) };
        if (fOriginalStream_.eof ()) {
            return 0;
        }
        size_t  maxToRead   =   intoEnd - intoStart;
        fOriginalStream_.read (reinterpret_cast<wchar_t*> (intoStart), maxToRead);
        size_t  n   =    static_cast<size_t> (fOriginalStream_.gcount ());      // cast safe cuz amount asked to read was also size_t

        // apparently based on http://www.cplusplus.com/reference/iostream/istream/read/ EOF sets the EOF bit AND the fail bit
        if (not fOriginalStream_.eof () and fOriginalStream_.fail ()) {
            Execution::DoThrow (Execution::StringException (String_Constant (L"Failed to read from wistream")));
        }
        return n;
    }
    virtual SeekOffsetType  GetReadOffset () const override
    {
        // instead of tellg () - avoids issue with EOF where fail bit set???
        auto    critSec { make_unique_lock (fCriticalSection_) };
        return fOriginalStream_.rdbuf ()->pubseekoff (0, ios_base::cur, ios_base::in);
    }
    virtual SeekOffsetType  SeekRead (Whence whence, SignedSeekOffsetType offset) override
    {
        auto    critSec { make_unique_lock (fCriticalSection_) };
        switch (whence) {
            case    Whence::eFromStart:
                fOriginalStream_.seekg (offset, ios::beg);
                break;
            case    Whence::eFromCurrent:
                fOriginalStream_.seekg (offset, ios::cur);
                break;
            case    Whence::eFromEnd:
                fOriginalStream_.seekg (offset, ios::end);
                break;
        }
        return fOriginalStream_.tellg ();
    }

private:
    mutable mutex   fCriticalSection_;
    wistream&       fOriginalStream_;
};






/*
 ********************************************************************************
 *********** Streams::iostream::TextInputStreamFromIStreamAdapter ***************
 ********************************************************************************
 */
TextInputStreamFromIStreamAdapter::TextInputStreamFromIStreamAdapter (std::wistream& originalStream)
    : TextInputStream (make_shared<Rep_> (originalStream))
{
}
