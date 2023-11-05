/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ModuleInit_h_
#define _Stroika_Foundation_Execution_ModuleInit_h_ 1

#include "../StroikaPreComp.h"

#include <cstdint>

#include "../Configuration/Common.h"
#include "../Memory/Common.h"

/**
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 */

namespace Stroika::Foundation::Execution {

    class [[deprecated ("Since Stroika v3.0d4 - use call_once or static inline")]] ModuleDependency {
    public:
        ModuleDependency (void (*start) (), void (*end) ());
        ~ModuleDependency ();

    private:
        void (*fEnd_) ();
    };

    template <typename MODULE_DATA>
    class [[deprecated ("Since Stroika v3.0d4 - use call_once or static inline")]] ModuleInitializer {
    public:
        /**
         */
        ModuleInitializer ();
        ~ModuleInitializer ();

    public:
        /**
         */
        static void Start ();

    public:
        /**
         */
        static void End ();

    public:
        /**
         */
        static MODULE_DATA& Actual ();

    public:
        /**
         *  In order for one module to depend on another, the other module calls this modules 'GetDependency' method, and stores
         *  the ModuleDependency object in its ModuleInitializer. That way - this dependency gets started before, and gets terminated after
         *  the referring dependent module.
         */
        static ModuleDependency GetDependency ();

    private:
        alignas (alignof (MODULE_DATA)) static byte sActualModuleInitializer_Storage_[sizeof (MODULE_DATA)]; // avoid actual memory allocation call - since only one of these
        static inline uint16_t sInitCnt_{0};
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ModuleInit.inl"

#endif /*_Stroika_Foundation_Execution_ModuleInit_h_*/
