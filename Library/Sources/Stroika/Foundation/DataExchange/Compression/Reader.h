/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Compression_Reader_h_
#define _Stroika_Foundation_DataExchange_Compression_Reader_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Streams/InputStream.h"

// FILE DEPRECATED!!!
/// FILE DEPRECATED

namespace Stroika::Foundation::DataExchange::Compression {

    using namespace Streams;
    using Memory::BLOB;

    /**
     *  Compresses or decompresses a stream - a one-pass transform: each call takes its source and returns the transformed stream,
     *  keeping nothing but its options, so its const methods may be called concurrently (each stream it returns follows a stream's
     *  rules). Unlike an Archive::Reader, which keeps its source, to read the files in it by name.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    class Reader {
    public:
        class IRep;

    public:
        Reader ()              = delete;
        Reader (const Reader&) = delete;
        Reader (Reader&& src);
        Reader& operator= (const Reader&) = delete;

    protected:
        explicit Reader (const shared_ptr<IRep>& rep);

    public:
        nonvirtual InputStream::Ptr<byte> Compress (const InputStream::Ptr<byte>& src) const;
        nonvirtual BLOB                   Compress (const BLOB& src) const;

    public:
        nonvirtual InputStream::Ptr<byte> Decompress (const InputStream::Ptr<byte>& src) const;
        nonvirtual BLOB                   Decompress (const BLOB& src) const;

    protected:
        nonvirtual const IRep& _GetRep () const;

    private:
        shared_ptr<IRep> fRep_;
    };

    /**
     */
    class Reader::IRep {
    public:
        virtual ~IRep () = default;

    public:
        virtual InputStream::Ptr<byte> Compress (const InputStream::Ptr<byte>& src) const = 0;

    public:
        virtual InputStream::Ptr<byte> Decompress (const InputStream::Ptr<byte>& src) const = 0;
    };
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Reader.inl"

#endif /*_Stroika_Foundation_DataExchange_Compression_Reader_h_*/
