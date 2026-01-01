/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Archive_Reader_h_
#define _Stroika_Foundation_DataExchange_Archive_Reader_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Memory/BLOB.h"

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

    class Ptr {
    public:
        Ptr ()           = delete;
        Ptr (const Ptr&) = delete;
        Ptr (Ptr&& src);
        explicit Ptr (const shared_ptr<IRep>& rep);
        Ptr& operator= (const Ptr&) = delete;

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
        nonvirtual IRep&       _GetRep ();
        nonvirtual const IRep& _GetRep () const;

    private:
        shared_ptr<IRep> fRep_;
    };

    /**
     */
    class IRep {
    public:
        virtual ~IRep ()                                           = default;
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
