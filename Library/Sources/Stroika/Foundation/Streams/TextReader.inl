/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextReader_inl_
#define _Stroika_Foundation_Streams_TextReader_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Memory/BLOB.h" // just for deprecated API
#include "InternallySynchronizedInputStream.h"

namespace Stroika::Foundation::Streams::TextReader {

    /*
     ********************************************************************************
     ***************************** TextReader::New **********************************
     ********************************************************************************
     */
    inline auto New (const InputStream::Ptr<byte>& src) -> Ptr
    {
        return New (src, src.IsSeekable () ? SeekableFlag::eSeekable : SeekableFlag::eNotSeekable);
    }
    inline auto New (const InputStream::Ptr<Character>& src) -> Ptr
    {
        return src;
    }

    /////////////// ***************** DEPRECATED BELOW /////////////////
    [[deprecated ("Since Stroika v3.0d1 - just use InternallySynchronizedInputOutputStream directly ")]] inline Ptr
    New (Execution::InternallySynchronized internallySynchronized, const Memory::BLOB& src, const optional<Characters::String>& charset = nullopt)
    {
        auto codeCvt = charset ? Characters::CodeCvt<>{Characters::Charset{*charset}} : Characters::CodeCvt<>{};
        switch (internallySynchronized) {
            case Execution::eInternallySynchronized:
                AssertNotImplemented ();
                //return InternalSyncRep_::New ();
                return New (src, codeCvt);
            case Execution::eNotKnownInternallySynchronized:
                return New (src, codeCvt);
            default:
                RequireNotReached ();
                return New (src, codeCvt);
        }
    }
    [[deprecated ("Since Stroika v3.0d1 - just use InternallySynchronizedInputOutputStream directly ")]] inline Ptr
    New (Execution::InternallySynchronized internallySynchronized, const InputStream::Ptr<byte>& src,
         SeekableFlag seekable = SeekableFlag::eSeekable, ReadAhead readAhead = eReadAheadAllowed)
    {
        switch (internallySynchronized) {
            case Execution::eInternallySynchronized:
                AssertNotImplemented ();
                //return InternalSyncRep_::New ();
                return New (src, seekable);
            case Execution::eNotKnownInternallySynchronized:
                return New (src, seekable, readAhead);
            default:
                RequireNotReached ();
                return New (src, seekable, readAhead);
        }
    }
    [[deprecated ("Since Stroika v3.0d1 - just use InternallySynchronizedInputOutputStream directly ")]] inline Ptr
    New (Execution::InternallySynchronized internallySynchronized, const InputStream::Ptr<byte>& src,
         const optional<Characters::String>& charset, SeekableFlag seekable = SeekableFlag::eSeekable, ReadAhead readAhead = eReadAheadAllowed)
    {
        auto codeCvt = charset ? Characters::CodeCvt<>{Characters::Charset{*charset}} : Characters::CodeCvt<>{};
        switch (internallySynchronized) {
            case Execution::eInternallySynchronized:
                AssertNotImplemented ();
                //return InternalSyncRep_::New ();
                return New (src, codeCvt, seekable, readAhead);
            case Execution::eNotKnownInternallySynchronized:
                return New (src, codeCvt, seekable, readAhead);
            default:
                RequireNotReached ();
                return New (src, codeCvt, seekable, readAhead);
        }
    }
    [[deprecated ("Since Stroika v3.0d1 - just use InternallySynchronizedInputOutputStream directly ")]] inline Ptr
    New (Execution::InternallySynchronized internallySynchronized, const Traversal::Iterable<Character>& src)
    {
        switch (internallySynchronized) {
            case Execution::eInternallySynchronized:
                AssertNotImplemented ();
                return New (src);
            case Execution::eNotKnownInternallySynchronized:
                return New (src);
            default:
                RequireNotReached ();
                return New (src);
        }
    }
    [[deprecated ("Since Stroika v3.0d1 - just use InternallySynchronizedInputOutputStream directly ")]] inline auto
    New (Execution::InternallySynchronized internallySynchronized, const InputStream::Ptr<Character>& src) -> Ptr
    {
        switch (internallySynchronized) {
            case Execution::eInternallySynchronized:
                AssertNotImplemented ();
                //return InternalSyncRep_::New ();
                return src;
            case Execution::eNotKnownInternallySynchronized:
                return src;
            default:
                RequireNotReached ();
                return src;
        }
    }

}

#endif /*_Stroika_Foundation_Streams_TextReader_inl_*/
