/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Archive_Reader_h_
#define _Stroika_Foundation_DataExchange_Archive_Reader_h_ 1

#include "../../StroikaPreComp.h"

#include <istream>

#include "../../Memory/SharedByValue.h"
#include "../../Streams/InputStream.h"

#include "../VariantValue.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::DataExchange::Archive {

    using Characters::String;
    using Containers::Set;
    using Memory::BLOB;

    /**
     *  Abstraction for Readers that map files or streams to collections of files, like zip files, tar files, etc.
     */
    class Reader {
    protected:
        class _IRep;

    public:
        Reader ()              = delete;
        Reader (const Reader&) = delete;
        Reader (Reader&& src);
        Reader& operator= (const Reader&) = delete;

    protected:
        explicit Reader (const shared_ptr<_IRep>& rep);

    public:
        /**
         */
        nonvirtual Set<String> GetContainedFiles () const;

    public:
        /**
         *  maybe define iterator/indexes and maybe have return stream
         */
        nonvirtual BLOB GetData (const String& fileName) const;

    protected:
        nonvirtual _IRep& _GetRep ();
        nonvirtual const _IRep& _GetRep () const;

    private:
        shared_ptr<_IRep> fRep_;
    };

    /**
     */
    class Reader::_IRep {
    public:
        virtual ~_IRep ()                                          = default;
        virtual Set<String> GetContainedFiles () const             = 0;
        virtual BLOB        GetData (const String& fileName) const = 0;
    };
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Reader.inl"

#endif /*_Stroika_Foundation_DataExchange_Archive_Reader_h_*/
