/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ModuleInit_h_
#define _Stroika_Foundation_Execution_ModuleInit_h_ 1

#include "../StroikaPreComp.h"

#include <cstdint>

#include "../Configuration/Common.h"
#include "../Memory/Common.h"

/**
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 * TODO
 *
 *      @todo   Consider if we should add assertion usage here. Trouble is - interdependenceis may
 *              cause trouble? Maybe just document in comments requirements (like fStart != nullptr).
 *
 *              NO NEED, because C++2a will have builtin (attribute) assertion support, which will hopefully
 *              work better.
 *
 *      @todo   Better Document new Module Dependency code, and verify there is as little wasted overhead
 *              as possible...
 */

namespace Stroika::Foundation::Execution {

    /**
     *  For the most part, using ModuleInitializer<> automatically takes care of all your module interdependencies
     *  automatically, just by #including the right files. This is true because the #include dependency graph
     *  matches the module dependency graph. But if you have module interdependencies which are not captured by
     *  #include interdependencies, explicit ModuleDependency relationships between modules can be created
     *  to force extra dependencies.
     *
     *  To use, in a module which has dependencies on it (and a ModuleInitializer<>) - add a function
     *          Execution::ModuleDependency MakeModuleDependency_PUTMODULENAMEHERE ();
     *
     *      'PUTMODULENAMEHERE' only needs to be unique in the given namespace, as these functions can be placed
     *  in namespaces (@see Characters::MakeModuleDependency_String () for example).
     *
     *  Then, in modules that depend on PUTMODULERNAMEHERE (but dont #include that module from their header), just
     *  add
     *                      Execution::ModuleDependency fPUTMODULERNAMEHEREDependency;
     *  to the 'module data' object in its ModuleInitializer<>. For example:
     *          struct  TraceModuleData_ {
     *              TraceModuleData_ ();
     *              ~TraceModuleData_ ();
     *
     *              Emitter                     fEmitter;
     *              Execution::ModuleDependency fStringDependency;
     *          };
     *  and then initialize it with:
     *      ...
     *      TraceModuleData_::TraceModuleData_ ()
     *          : fStringDependency (Characters::MakeModuleDependency_String ())
     *          ....
     *
     *  The net effect is in the TraceModuleData_ - it will have an object which calls the 'start' and 'stop' methods of
     *  ModuleInitializer<String_ModuleInit_>... accordingly - to force the proper order of initialization.
     *
     *  \note   ModuleDependency<>s are effectively transative, so that if the 'String' module depends on
     *          the 'BlockAllocated' module, for example, then having your module depend on the 'String' module
     *          effectively means it automatically depends on the 'BlockAllocated' module.
     */
    class ModuleDependency {
    public:
        ModuleDependency (void (*start) (), void (*end) ());
        ~ModuleDependency ();

    private:
        void (*fEnd_) ();
    };

    /**
     *  \brief  ModuleInitializer<> is a utility class to support controlled order of initialization across modules
     *
     *  \par OVERVIEW:
     *
     *      This class does little but DOCUMENT a useful design pattern to avoid the nasty cross-module deadly
     *      embrace caused by unreliable static object construction order.
     *
     *  \par Example Usage
     *      \code
     *          namespace   ExampleModule {
     *              namespace   Private {
     *                  struct  MyModuleData_ {
     *                      MyModuleData_ () {}
     *                      ~MyModuleData_ () {}
     *                      recursive_mutex fCritSection;
     *                  };
     *              }
     *          };
     *          namespace   {
     *              Execution::ModuleInitializer<ExampleModule::Private::MyModuleData_>  _MI_;   // this object constructed for the CTOR/DTOR per-module side-effects
     *              inline recursive_mutex&    GetCritSection_ () { return Execution::ModuleInitializer<Private::MyModuleData_>::Actual ().fCritSection; }
     *          }
     *      \endcode
     *
     *  In the MyModuleData_::CTOR you initialize your module (in this case, fCritSection). And in the
     *  ActualModuleInit::DTOR - you uninitialize (e.g. delete fCritSection);
     *
     *  The reason this trick works reliably - is that the anonymouns namespace declaration of _MI_ -
     *  above - bumps/decrements reference counts in EVERY CONTAINING MODULE. The first module initialized -
     *  calls the ActualModuleInit::CTOR - once - and not multiply. And the as the file-scope-lifetime
     *  variables are destroyed, this count drops to zero, and the ActualModuleInit::DTOR gets called (once).
     *
     *  Because of this process, and so long as the details of ActualModuleInit don't go cross-module, we are
     *  assured each module is initialized automatically, but just once, and in an order independent of the
     *  order generated by the linker for static initializers for .obj files.
     *
     *  One slightly subtle point about this - if Module A depends on Module B in its implementation,
     *  but not its HEADERS, it STILL may need to #include the headers for Module B (in cases where
     *  Module B uses this mechanism) to assure the right order of initializaiton.
     *
     * The type argument MODULE_DATA - is the code which actually performs the real, module-specific
     * initialization.
     *
     * Initialiation of MODULE_DATA is done in the constructor, and cleanup in the DTOR.
     * This class makes sure that - if you construct a ModuleInitializer<> object in every module which might
     * use this module, then the is constructed at the earliest time, and destroyed at the latest.
     *
     *  \note   ModuleInitializer works well with @see VirtualConstant<> - which can be used
     *          to expose the carefully initialized data as if it were a constant.
     */
    template <typename MODULE_DATA>
    class ModuleInitializer {
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
        alignas (alignof (MODULE_DATA)) static std::byte sActualModuleInitializer_Storage_[sizeof (MODULE_DATA)]; // avoid actual memory allocation call - since only one of these
#if qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy
        static uint16_t sInitCnt_;
#else
        static inline uint16_t sInitCnt_{0};
#endif
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ModuleInit.inl"

#endif /*_Stroika_Foundation_Execution_ModuleInit_h_*/
