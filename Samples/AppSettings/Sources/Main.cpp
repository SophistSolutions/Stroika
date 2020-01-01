/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/DataExchange/OptionsFile.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/ModuleGetterSetter.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"
#include "Stroika/Foundation/Time/Duration.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Time;

namespace {
    struct OptionsData_ {
        bool               fEnabled = false;
        optional<DateTime> fLastSynchronizedAt;
    };
    /*
     *  OptionsData_Storage_IMPL_ is a 'traits' object, defining a GET and SET method to save/restore
     *  OptionsData_.
     *
     *  This need not worry about thread safety:
     *      o   in the constructor because C++ guarantees this for statically constructed objects
     *      o   and in the Get/Set methods because ModuleGetterSetter manages this locking
     *
     *  A user COULD either choose NOT to persist the data (in which case the logic with fOptionsFile_ 
     *  would be removed/unneeded). Or could perist another way.
     *
     *  But this example shows using OptionsFile to persist the data to a local JSON file, using
     *  the ObjectVariantMapper to serialize/deserialize C++ data structures.
     */
    struct OptionsData_Storage_IMPL_ {
        OptionsData_Storage_IMPL_ ()
            : fOptionsFile_{
                  /*
                   * Any module name will do. This will map (by default) to a MyModule.json file in XXX.
                   * If you require a single configuration file 'Main" might be a better module name.
                   * But if you have multiple modules with configuration data, pick a name that matches that module,
                   * and they will all be stored under a folder for all your apps configuration.
                   */
                  L"MyModule"sv,

                  /*
                   * C++ doesn't have intrinsically enough metadata to effectively serialize deserialize data, but its close.
                   * You have to give it class mappings, and other non-builtin types mappings, so that it can serialize.
                   *
                   * Note - this serializing logic is VERY widely useful outside of configuration - for example it can be used
                   * to provide WebService/REST interfaces, or for debugging/logging output.
                   */
                  [] () -> ObjectVariantMapper {
                      ObjectVariantMapper mapper;
                      mapper.AddClass<OptionsData_> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
                          {L"Enabled", Stroika_Foundation_DataExchange_StructFieldMetaInfo (OptionsData_, fEnabled)},
                          {L"Last-Synchronized-At", Stroika_Foundation_DataExchange_StructFieldMetaInfo (OptionsData_, fLastSynchronizedAt)},
                      });
                      return mapper;
                  }(),

                  /*
                   * Hooks for versioning, to manage as your application evolves and the configuration data changes
                   */
                  OptionsFile::kDefaultUpgrader,

                  /*
                   * Hook to decide the folder (and filename pattern) where the configuration data will be stored.
                   *
                   * This defaults to 
                   *       FileSystem::WellKnownLocations::GetApplicationData () + appName + String (IO::FileSystem::kPathComponentSeperator) + moduleName + suffix
                   * or folder:
                   *      L"/var/opt/Put-Your-App-Name-Here" or "C:\ProgramData\Put-Your-App-Name-Here"
                   *  and this module configuration file would be:
                   *      L"/var/opt/Put-Your-App-Name-Here/MyModule.json" OR
                   *      L"C:/ProgramData/Put-Your-App-Name-Here/MyModule.json" OR
                   *
                   *    \note - this function does NOT create the 'Put-Your-App-Name-Here' folder first, and will NOT persist
                   *            files if this folder does not exist.
                   *
                   *            Callers can easily repalce the default function provided in OptionsFile::mkFilenameMapper - just
                   *            dont call that and provide your own lambda - to create the folder.
                   *
                   *            But a better pattern is to create the folder in your application installer, typically.
                   */
                  OptionsFile::mkFilenameMapper (L"Put-Your-App-Name-Here")}
            , fActualCurrentConfigData_ (fOptionsFile_.Read<OptionsData_> (OptionsData_{}))
        {
            Set (fActualCurrentConfigData_); // assure derived data (and changed fields etc) up to date
        }
        OptionsData_ Get () const
        {
            // no locking required here for thread safety.
            // This is always done inside of a read or a full lock by ModuleGetterSetter
            return fActualCurrentConfigData_;
        }
        void Set (const OptionsData_& v)
        {
            // no locking required here for thread safety.
            // This is always done inside of a write lock by ModuleGetterSetter
            fActualCurrentConfigData_ = v;
            fOptionsFile_.Write (v);
        }

    private:
        OptionsFile  fOptionsFile_;
        OptionsData_ fActualCurrentConfigData_; // automatically initialized just in time, and externally synchronized
    };

}

namespace {
    ModuleGetterSetter<OptionsData_, OptionsData_Storage_IMPL_> sModuleConfiguration_;
    WaitableEvent                                               sWaitableEvent_; // some thread could be waiting on this, and perform some reactive task when the module settings change

    void TestUse1_ ()
    {
        // This will be by far the most common use pattern - just read some field of the configuraiton object
        if (sModuleConfiguration_.Get ().fEnabled) {
            // do something
        }
    }
    void TestUse2_ ()
    {
        // or read several fields all guaranteed within this same snapshot (not holding a lock duing the action)
        auto d = sModuleConfiguration_.Get ();
        // lock not held here so configuration could change but this code remains safe and crash free
        if (d.fEnabled and d.fLastSynchronizedAt) {
            // do something
        }
    }
    void TestUse3_ ()
    {
        if (sModuleConfiguration_.Get ().fEnabled) {
            // a non-atomic update of the entire OptionsData_ object
            auto n     = sModuleConfiguration_.Get ();
            n.fEnabled = false; // change something in 'n' here
            sModuleConfiguration_.Set (n);
        }
    }
    void TestUse4_ ()
    {
        // Use Update () to atomically update data
        // Use the return value to tell if a real change was made (so you can invoke some sort of notication/action)
        static const Duration kMinTime_ = 2min;
        if (sModuleConfiguration_.Update ([] (const OptionsData_& data) -> optional<OptionsData_> {  if (data.fLastSynchronizedAt && *data.fLastSynchronizedAt + kMinTime_ > DateTime::Now ()) { OptionsData_ result = data; result.fLastSynchronizedAt = DateTime::Now (); return result; } return {}; })) {
            sWaitableEvent_.Set (); // e.g. trigger someone to wakeup and used changes? - no global lock held here...
        }
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"main", L"argv=%s", Characters::ToString (vector<const char*>{argv, argv + argc}).c_str ())};
    TestUse1_ ();
    TestUse2_ ();
    TestUse3_ ();
    TestUse4_ ();
    return EXIT_SUCCESS;
}
