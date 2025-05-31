/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Archive_Writer_h_
#define _Stroika_Foundation_DataExchange_Archive_Writerh_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Memory/BLOB.h"
//#include "Stroika/Foundation/DataExchange/VariantValue.h"
//#include "Stroika/Foundation/Memory/SharedByValue.h"
//#include "Stroika/Foundation/Streams/InputStream.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::DataExchange::Archive::Writer {

    using Characters::String;
    using Memory::BLOB;

    /**
     *  Abstraction for Writers that maps collections of files to a binary stream format (like zipfile)
     *  SUPER simplistic for now (no depth/hierarchy, no editing functions - just create and append).
     */

    class _IRep;

    class Ptr {
    protected:
    public:
        Ptr ()           = delete;
        Ptr (const Ptr&) = delete;
        Ptr (Ptr&& src);
        Ptr& operator= (const Ptr&) = delete;

    protected:
        explicit Ptr (const shared_ptr<_IRep>& rep);

    public:
        /**
             */
        nonvirtual void Add (const String& fileName, const BLOB& data);

    protected:
        nonvirtual _IRep&       _GetRep ();
        nonvirtual const _IRep& _GetRep () const;

    private:
        shared_ptr<_IRep> fRep_;
    };

    /**
     */
    class _IRep {
    public:
        virtual ~_IRep ()                                           = default;
        virtual void Add (const String& fileName, const BLOB& data) = 0;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Writer.inl"

#endif /*_Stroika_Foundation_DataExchange_Archive_Writerh_*/
