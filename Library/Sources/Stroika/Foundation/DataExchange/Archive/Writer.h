/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Archive_Writer_h_
#define _Stroika_Foundation_DataExchange_Archive_Writer_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Memory/BLOB.h"

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

    class IRep;

    class Ptr {
    protected:
    public:
        Ptr ()           = delete;
        Ptr (const Ptr&) = delete;
        Ptr (Ptr&& src);
        explicit Ptr (const shared_ptr<IRep>& rep);
        Ptr& operator= (const Ptr&) = delete;

    public:
        /**
         */
        nonvirtual void Add (const String& fileName, const BLOB& data);

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
        virtual ~IRep ()                                                        = default;
        virtual void Add (const String& fileName, const span<const byte>& data) = 0;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Writer.inl"

#endif /*_Stroika_Foundation_DataExchange_Archive_Writer_h_*/
