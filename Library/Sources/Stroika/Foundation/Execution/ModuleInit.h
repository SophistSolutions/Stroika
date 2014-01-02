/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ModuleInit_h_
#define _Stroika_Foundation_Execution_ModuleInit_h_ 1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Memory/Common.h"



/**
 *  \version    <a href="code_status.html#Late-Alpha">Late-Alpha</a>
 *
 * TODO
 *
 *      @todo   Consider if we should add assertion usage here. Trouble is - interdependenceis may
 *              cause trouble? Maybe just document in comments requirements (like fStart != nullptr).
 *
 *      @todo   Better Document new Module Dependency code, and verify there is as little wasted overhead
 *              as possible...
 *
 *      @todo   Review SharedStaticData<> module - and see if we can use that here or which works better?
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            using   Memory::Byte;


            /**
             *
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
             */
            class   ModuleDependency {
            public:
                ModuleDependency (void (*start) (), void (*end) ());
                ~ModuleDependency ();

            private:
                void (*fEnd) ();
            };


            /**
             *  \brief  ModuleInitializer<> is a utility class to support controlled order of initialization across modules
             *
             *  OVERVIEW:
             *
             *      This class does little but DOCUMENT a useful design pattern to avoid the nasty cross-module deadly
             *      embrace caused by unreliable static object construction order.
             *
             *  USAGE EXAPMPLE:
             *      namespace   ExampleModule {
             *          namespace   Private {
             *              struct  MyModuleData_ {
             *                  MyModuleData_ () {}
             *                  ~MyModuleData_ () {}
             *                  recursive_mutex fCritSection;
             *              };
             *          }
             *      };
             *      namespace   {
             *          Execution::ModuleInitializer<ExampleModule::Private::MyModuleData_>  _MI_;   // this object constructed for the CTOR/DTOR per-module side-effects
             *          inline recursive_mutex&    GetCritSection_ () { return Execution::ModuleInitializer<Private::MyModuleData_>::Actual ().fCritSection; }
             *      }
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
             *  One slightly subtle point about this - if Module A depends on Module B in its implemenation,
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
             *      @todo   add alignas (or whatever the new C++11 support for alignment is) - to assure
             *              sActualModuleInitializer_Storage_ properly aligned).
             *
             */
            template    <typename MODULE_DATA>
            class   ModuleInitializer {
            public:
                ModuleInitializer ();
                ~ModuleInitializer ();

            public:
                static  void    Start ();
                static  void    End ();

            public:
                static  MODULE_DATA&  Actual ();

            public:
                static  ModuleDependency   GetDependency ();

            private:
                static  Byte                sActualModuleInitializer_Storage_[sizeof (MODULE_DATA)];   // avoid actual memory allocation call - since only one of these
                static  unsigned    short   sInitCnt_;
            };


            /**
             *  Help to construct an object - and NEVER call its DTOR. Assure its
             *  constructed before use. Can be helpful for objects that don't need to be destroyed, but need to
             *  assure they are constructed before use.
             *  Only thought out using this at file scope to wrap construciton of an object.
             *
             *      @todo   add alignas (or whatever the new C++11 support for alignment is) - to assure
             *              fTBuf properly aligned).
             */
            template    <typename   T>
            struct  StaticSingletonObjectConstructionHelper {
                bool    fConstructed;
                Byte    fTBuf[sizeof (T)];

                operator T& ()
                {
                    // for now - OK - I think - to avoid critical section - cuz these get used
                    // before we've had a chance to make threads...
                    if (not fConstructed) {
                        new (&fTBuf) T ();
                        fConstructed = true;
                    }
                    return * (reinterpret_cast<T*> (&fTBuf));
                };
            };


            /**
             * See http://bugzilla/show_bug.cgi?id=439
             *
             * Allow use of regular constant declaration use when we have
             * an underlying system where the constant is actually FETECHED from the argument function.
             *
             * Could use this in someplace like:
             *      const ModuleInit::ConstantViaGetter<Enumeration,&Private::kTypeHiddenFilter_>   kTypeHiddenFilter;
             *
             *  See http://bugzilla/show_bug.cgi?id=439 for details.
             *
             *  Note: it would be HIGHLY DESIRABLE if C++ allowed operator. overloading, as accessing one of these
             *  values without assinging to a temporary first - means that you cannot directly call its methods.
             *  Thats a bit awkward.
             *
             *  So if you have a type T, with method m(), and variable of type T t.
             *      Your starter code might be:
             *          T   t;
             *          t.m ();
             *  When you replace 'T t' with
             *      ConstViaGetter<T,...> t;
             *          you must call t->m();
             */
            template    <typename BASETYPE, const BASETYPE& (*ValueGetter) ()>
            struct ConstantViaGetter {
                inline  operator const BASETYPE () const
                {
                    return (ValueGetter) ();
                }
                inline  const BASETYPE* operator-> () const
                {
                    return &(ValueGetter) ();
                }
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ModuleInit.inl"

#endif  /*_Stroika_Foundation_Execution_ModuleInit_h_*/
