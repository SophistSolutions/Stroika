/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Archive_Reader_h_
#define _Stroika_Foundation_DataExchange_Archive_Reader_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Streams/InputStream.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::DataExchange::Archive::Reader {

    using Characters::String;
    using Containers::Set;
    using Memory::BLOB;

    /**
     *  Abstraction for Readers that map files or streams to collections of files, like zip files, tar files, etc.
     */
    class IRep;

    /**
     *  A reader is a view over the stream it was made from (GetSource ()) - an extension of that InputStream::Ptr: its calls, const
     *  ones included, read and move that stream.
     *
     *  Unlike a Compression::Reader - a one-pass transform, which takes its source with each call and keeps none - an archive reader
     *  keeps its source, since it reads the files in it by name, in any order.
     *
     *  \note Since Archive::Reader::Ptr is a smart pointer, the constness of its methods says whether they modify the smart pointer,
     *        not the reader it points to - as for Streams::InputStream::Ptr. So two calls at once, const ones included, are safe only
     *        where that reader's rules say so (see the New () that made it).
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Thread-Safety-Rules-Depends-On-Subtype">Thread-Safety-Rules-Depends-On-Subtype</a>
     *          Typically - as for the stream it reads - the caller must synchronize all access, const calls included: see the New ()
     *          that made it.
     */
    class Ptr {
    public:
        Ptr ()           = delete;
        Ptr (const Ptr&) = delete;
        Ptr (Ptr&& src);
        explicit Ptr (const shared_ptr<IRep>& rep);
        Ptr& operator= (const Ptr&) = delete;

    public:
        /**
         *  \note const as a smart pointer's methods are: not safe to call concurrently on one reader unless its New () says so
         */
        nonvirtual Set<String> GetContainedFiles () const;

    public:
        /**
         *  maybe define iterator/indexes and maybe have return stream
         *
         *  \note const as a smart pointer's methods are: not safe to call concurrently on one reader unless its New () says so
         */
        nonvirtual BLOB GetData (const String& fileName) const;

    public:
        /**
         *  \brief The stream this reader reads - as given its New (). Reading or seeking it moves the reader's position too, so
         *         the two need the same synchronization.
         */
        nonvirtual Streams::InputStream::Ptr<byte> GetSource () const;

    protected:
        nonvirtual const IRep& _ConstGetRep () const;

    private:
        shared_ptr<const IRep> fRep_;
    };

    /**
     */
    class IRep {
    public:
        virtual ~IRep ()                                                               = default;
        virtual Set<String>                     GetContainedFiles () const             = 0;
        virtual BLOB                            GetData (const String& fileName) const = 0;
        virtual Streams::InputStream::Ptr<byte> GetSource () const                     = 0;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Reader.inl"

#endif /*_Stroika_Foundation_DataExchange_Archive_Reader_h_*/
