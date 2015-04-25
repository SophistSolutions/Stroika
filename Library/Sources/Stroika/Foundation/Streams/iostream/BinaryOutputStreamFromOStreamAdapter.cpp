/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    <mutex>

#include    "../../Characters/String_Constant.h"
#include    "../../Execution/Common.h"
#include    "../../Execution/OperationNotSupportedException.h"

#include    "BinaryOutputStreamFromOStreamAdapter.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;
using   namespace   Stroika::Foundation::Streams::iostream;

using   Characters::String_Constant;
using   Execution::make_unique_lock;



class   BinaryOutputStreamFromOStreamAdapter::IRep_ : public BinaryOutputStream::_IRep, public Seekable::_IRep {
public:
    IRep_ (ostream& originalStream)
        : fCriticalSection_ ()
        , fOriginalStream_ (originalStream)
    {
    }

protected:
    virtual void    Write (const Byte* start, const Byte* end) override
    {
        Require (start != nullptr or start == end);
        Require (end != nullptr or start == end);

        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Wfuture-compat\"");
        auto    critSec { make_unique_lock (fCriticalSection_) };
        DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Wfuture-compat\"");

        fOriginalStream_.write (reinterpret_cast<const char*> (start), end - start);
        if (fOriginalStream_.fail ()) {
            Execution::DoThrow (Execution::StringException (String_Constant (L"Failed to write from ostream")));
        }
    }

    virtual void    Flush () override
    {
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Wfuture-compat\"");
        auto    critSec { make_unique_lock (fCriticalSection_) };
        DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Wfuture-compat\"");
        fOriginalStream_.flush ();
        if (fOriginalStream_.fail ()) {
            Execution::DoThrow (Execution::StringException (String_Constant (L"Failed to flush ostream")));
        }
    }

    virtual SeekOffsetType  GetOffset () const override
    {
        // instead of tellg () - avoids issue with EOF where fail bit set???
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Wfuture-compat\"");
        auto    critSec { make_unique_lock (fCriticalSection_) };
        DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Wfuture-compat\"");
        return fOriginalStream_.rdbuf ()->pubseekoff (0, ios_base::cur, ios_base::out);
    }

    virtual SeekOffsetType  Seek (Whence whence, SignedSeekOffsetType offset) override
    {
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Wfuture-compat\"");
        auto    critSec { make_unique_lock (fCriticalSection_) };
        DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Wfuture-compat\"");
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
    mutable recursive_mutex fCriticalSection_;
    ostream&                fOriginalStream_;
};




/*
 ********************************************************************************
 ********* Streams::iostream::BinaryOutputStreamFromOStreamAdapter *********(****
 ********************************************************************************
 */
BinaryOutputStreamFromOStreamAdapter::BinaryOutputStreamFromOStreamAdapter (ostream& originalStream)
    : BinaryOutputStream (shared_ptr<_IRep> (new IRep_ (originalStream)))
{
}

