/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ModuleGetterSetter_h_
#define _Stroika_Foundation_Execution_ModuleGetterSetter_h_ 1

#include "../StroikaPreComp.h"

#include <optional>

#include "../Configuration/Common.h"
#include "../Memory/Common.h"

/**
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Late</a>
 */

namespace Stroika::Foundation ::Execution {

    /**
     *  \brief  Helper to define synchronized, lazy constructed, module initialization (intended to work with DataExchange::OptionFile)
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
     *              bool                fEnabled = false;
     *              optional<DateTime>  fLastSynchronizedAt;
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
     *                  n.fEnabled = false; // change something in 'n' here
     *                  sModuleConfiguration_.Set (n);
     *              }
     *          }
     *          void TestUse3_ ()
     *          {
     *              if (sModuleConfiguration_.Update ([](const MyData_& data) -> optional<MyData_> {  if (data.fLastSynchronizedAt && *data.fLastSynchronizedAt + kMinTime_ > DateTime::Now ()) { MyData_ result = data; result.fLastSynchronizedAt = DateTime::Now (); return result; } return {}; })) {
     *                  sWaitableEvent.Set ();  // e.g. trigger someone to wakeup and used changes? - no global lock held here...
     *              }
     *          }
     *      \endcode
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
     *
     */
    template <typename T, typename IMPL>
    struct ModuleGetterSetter {
    public:
        /**
         *  Grab the global value, performing any necessary read-locks automatically.
         *
         *  \note - this returns a copy of T, not a reference, in order to properly manage locking, though
         *          since its inlined, for most cases, much of that case may be optimized away (the most expensive
         *          unavoidable peice being the readlock).
         */
        nonvirtual T Get ();

    public:
        /**
         *  Set the global value, performing any necessary write-locks automatically.
         */
        nonvirtual void Set (const T& v);

    public:
        /**
         *  \brief Call this with a lambda that will update the associated value (INSIDE a lock (synchronized))
         *
         *  Call this with a lambda that will update the associated value. The update will happen INSIDE
         *  a lock (synchronized). {{ reconsider if we should make that explicit promise or upgradelock
         *  so we can do update and check for change inside a shared_lock and never write lock if no change}}
         *
         *  updaterFunction should return nullopt if no change, or the new value if changed.
         *
         *  Update () assures atomic update of your global data, and returns copy of the new value set (optional - this can be ignored).
         *  But since it returns an optional<> you can test the result to see if any update was made, and trigger a wakeup or
         *  further processing (without the global lock held).
         *
         *  \par Example Usage
         *      \code
         *          sModuleConfiguration_.Update ([] (MyData_ data) { if (data.fLastSynchronizedAt + kMinTime_ > DateTime::Now ()) { data.fLastSynchronizedAt = DateTime::Now ();} return data; });
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          if (sModuleConfiguration_.Update ([] (const MyData_& data) -> optional<MyData_> { if (data.fLastSynchronizedAt + kMinTime_ > DateTime::Now ()) { MyData_ result = data; result.fLastSynchronizedAt = DateTime::Now (); return result;} return {};  })) {
         *              sWaitableEvent.Set ();  // e.g. trigger someone to wakeup and used changes? - no global lock held here...
         *          }
         *      \endcode
         */
        nonvirtual optional<T> Update (const function<optional<T> (const T&)>& updaterFunction);

    private:
        RWSynchronized<optional<IMPL>> fIndirect_;

    private:
        // separate this method out so the callers can be inlined, this more rarely executed, and longer segment of code is not
        nonvirtual void DoInitOutOfLine_ (typename RWSynchronized<optional<IMPL>>::WritableReference* ref);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ModuleGetterSetter.inl"

#endif /*_Stroika_Foundation_Execution_ModuleGetterSetter_h_*/
