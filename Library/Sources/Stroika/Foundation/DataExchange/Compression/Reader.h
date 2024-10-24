/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
     *  Abstraction for Readers that map files or streams to collections of files, like zip files, tar files, etc.
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
        nonvirtual IRep&       _GetRep ();
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
