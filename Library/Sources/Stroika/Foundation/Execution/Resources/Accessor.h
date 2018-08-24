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
        Accessor (const std::byte* start, const std::byte* end);

    public:
        nonvirtual const std::byte* begin () const;
        nonvirtual const std::byte* end () const;

    public:
        nonvirtual size_t size () const;

    public:
        /*
            * @todo    Consider different return value - streams? - or some new BLOB type.
            */
        nonvirtual vector<std::byte> GetData () const;

    private:
        const std::byte* fDataStart_;
        const std::byte* fDataEnd_;

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
