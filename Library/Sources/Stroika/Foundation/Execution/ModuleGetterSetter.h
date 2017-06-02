/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ModuleGetterSetter_h_
#define _Stroika_Foundation_Execution_ModuleGetterSetter_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Common.h"
#include "../Memory/Common.h"

/**
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Late</a>
 *
 * TODO
 *
 *      @todo   Consider addiing Update method (with func arg taking T by reference?/optr or taking and returing
 */

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            /**
             *  \breif  Helper to define synchronized, lazy constructed, module initialization (intended to work with DataExchange::OptionFile)
             *
             * Features:
             *      o   Simple API - get/set
             *      o   automatically intrinsically threadsafe
             *      o   Init underling object on first access, so easy to declare globally (static init) and less worry about running before main
             *      o   IMPL need not worry about thread safety. Just init on CTOR, and implement Get/Set methods.
             *
             *  \par Example Usage
             *      \code
             *          struct  MyData_ {
             *              bool        fEnabled = false;
             *              DateTime    fLastSynchronizedAt;
             *          };
             *          struct  ModuleGetterSetter_Implementation_MyData_ {
             *              ModuleGetterSetter_Implementation_MyData_ ()
             *                  : fOptionsFile_ {
             *                      L"MyModule",
             *                      [] () -> ObjectVariantMapper {
             *                          ObjectVariantMapper mapper;
             *                          mapper.AddClass<MyData_> (initializer_list<ObjectVariantMapper::StructFieldInfo> {
             *                              { L"Enabled", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MyData_, fEnabled) },
             *                              { L"Last-Synchronized-At", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MyData_, fLastSynchronizedAt) },
             *                          });
             *                          return mapper;
             *                      } ()
             *                      , OptionsFile::kDefaultUpgrader
             *                      , OptionsFile::mkFilenameMapper (L"Put-Your-App-Name-Here")
             *                  }
             *                  , fActualCurrentConfigData_ (fOptionsFile_.Read<MyData_> (MyData_ ()))
             *                  {
             *                      Set (fActualCurrentConfigData_); // assure derived data (and changed fields etc) up to date
             *                  }
             *                  MyData_   Get () const
             *                  {
             *                      return fActualCurrentConfigData_;
             *                  }
             *                  void    Set (const MyData_& v)
             *                  {
             *                      fActualCurrentConfigData_ = v;
             *                      fOptionsFile_.Write (v);
             *                  }
             *              private:
             *                  OptionsFile     fOptionsFile_;
             *                  MyData_         fActualCurrentConfigData_;      // automatically initialized just in time, and externally synchronized
             *          };
             *
             *          using   Execution::ModuleGetterSetter;
             *          ModuleGetterSetter<MyData_, ModuleGetterSetter_Implementation_MyData_>  sModuleConfiguration_;
             *
             *          void    TestUse_ ()
             *          {
             *              if (sModuleConfiguration_.Get ().fEnabled) {
             *                  auto n = sModuleConfiguration_.Get ();
             *                  sModuleConfiguration_.Set (n);
             *              }
             *          }
             *      \endcode
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
             *
             */
            template <typename T, typename IMPL>
            struct ModuleGetterSetter {
                /**
                 */
                nonvirtual T Get ();

                /**
                 */
                nonvirtual void Set (const T& v);

            private:
                Synchronized<Memory::Optional<IMPL>> fIndirect_;

            private:
                // Force IMPL CTOR out of line
                nonvirtual void DoInitOutOfLine_ (typename Synchronized<Memory::Optional<IMPL>>::WritableReference* ref);
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ModuleGetterSetter.inl"

#endif /*_Stroika_Foundation_Execution_ModuleGetterSetter_h_*/
