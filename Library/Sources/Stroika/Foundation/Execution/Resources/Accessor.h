/*/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Resources_Accessor_h_
#define _Stroika_Foundation_Execution_Resources_Accessor_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"
#include "../../Configuration/Common.h"

namespace Stroika::Foundation::Execution::Resources {

    class Manager;

    /**
     * This object provides access to the application-lifetime data/pointers associated with a resource
     * retrieved from a Resource::Manager (subclass).
     */
    class Accessor {
    private:
        Accessor (const Byte* start, const Byte* end);

    public:
        nonvirtual const Byte* begin () const;
        nonvirtual const Byte* end () const;

    public:
        nonvirtual size_t size () const;

    public:
        /*
            * @todo    Consider different return value - streams? - or some new BLOB type.
            */
        nonvirtual vector<Byte> GetData () const;

    private:
        const Byte* fDataStart_;
        const Byte* fDataEnd_;

    private:
        friend class Manager;
    };
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Accessor.inl"

#endif /*_Stroika_Foundation_Execution_Resources_Accessor_h_*/
