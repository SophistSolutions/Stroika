/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ModuleInit_inl_
#define _Stroika_Foundation_Execution_ModuleInit_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Debug/Assertions.h"

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ******************************** ModuleDependency ******************************
     ********************************************************************************
     */
    inline ModuleDependency::ModuleDependency (void (*start) (), void (*end) ())
        : fEnd_ (end)
    {
        (*start) ();
    }
    inline ModuleDependency::~ModuleDependency ()
    {
        (*fEnd_) ();
    }

    /*
     ********************************************************************************
     ************************ ModuleInitializer<MODULE_DATA> ************************
     ********************************************************************************
     */
    template <typename MODULE_DATA>
    inline void ModuleInitializer<MODULE_DATA>::Start ()
    {
        if (sInitCnt_++ == 0) {
            // no need to store pointer, cuz its the same as &sActualModuleInitializer_Storage_
            (void)new (&sActualModuleInitializer_Storage_) MODULE_DATA ();
        }
    }
    template <typename MODULE_DATA>
    void ModuleInitializer<MODULE_DATA>::End ()
    {
        if (--sInitCnt_ == 0) {
            reinterpret_cast<MODULE_DATA*> (&sActualModuleInitializer_Storage_)->~MODULE_DATA ();
        }
    }
    template <typename MODULE_DATA>
    inline ModuleInitializer<MODULE_DATA>::ModuleInitializer ()
    {
        Start ();
    }
    template <typename MODULE_DATA>
    inline ModuleInitializer<MODULE_DATA>::~ModuleInitializer ()
    {
        End ();
    }
    template <typename MODULE_DATA>
    inline MODULE_DATA& ModuleInitializer<MODULE_DATA>::Actual ()
    {
        Assert (sInitCnt_ > 0); // we've been initialized, and not yet destroyed...
        return *reinterpret_cast<MODULE_DATA*> (&sActualModuleInitializer_Storage_);
    }
    template <typename MODULE_DATA>
    inline ModuleDependency ModuleInitializer<MODULE_DATA>::GetDependency ()
    {
        return ModuleDependency (Start, End);
    }
    template <typename MODULE_DATA>
    alignas (alignof (MODULE_DATA)) std::byte ModuleInitializer<MODULE_DATA>::sActualModuleInitializer_Storage_[sizeof (MODULE_DATA)]; // avoid actual memory allocation call - since only one of these
#if qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy
    template <typename MODULE_DATA>
    unsigned short ModuleInitializer<MODULE_DATA>::sInitCnt_;
#endif

}

#endif /*_Stroika_Foundation_Execution_ModuleInit_inl_*/
