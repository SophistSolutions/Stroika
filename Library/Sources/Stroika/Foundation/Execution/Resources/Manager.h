/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Resources_Manager_h_
#define _Stroika_Foundation_Execution_Resources_Manager_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Configuration/Common.h"

#include "Accessor.h"
#include "Name.h"

namespace Stroika::Foundation::Execution::Resources {

    /**
     * A resource Manager is a abstract class (effectively smart pointer) to a source of resources. You instantiate
     * an appropriate resource manager subclass, and call ReadResource() as needed. ReadResource() returns a proxy
     * which points to the resource data.
     */
    class Manager {
    protected:
        class _IRep {
        public:
            virtual ~_IRep ()                                      = default;
            virtual Accessor ReadResource (const Name& name) const = 0;
        };

    protected:
        static Accessor _mkAccessor (span<const byte> s);

    protected:
        explicit Manager (const shared_ptr<_IRep>& rep);

    private:
        shared_ptr<_IRep> fRep_;

    public:
        // throws if not found
        nonvirtual Accessor ReadResource (const Name& name) const;

    public:
        // Like ReadResource () - but asserts instead  of throwing if resource not found
        nonvirtual Accessor CheckedReadResource (const Name& name) const noexcept;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Manager.inl"

#endif /*_Stroika_Foundation_Execution_Resources_Manager_h_*/
