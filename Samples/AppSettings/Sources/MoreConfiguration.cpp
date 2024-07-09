/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"

#include "MoreConfiguration.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Execution;

using namespace StroikaSample;
using namespace StroikaSample::AppSettings;

namespace {
    WaitableEvent sWaitableEvent_; // some thread could be waiting on this, and perform some reactive task when the module settings change

}

/*
 ********************************************************************************
 ********** AppSettings::Private_::MoreOptionsData_Storage_IMPL_ ****************
 ********************************************************************************
 */

AppSettings::Private_::MoreOptionsData_Storage_IMPL_::MoreOptionsData_Storage_IMPL_ ()
            : fOptionsFile_{
                  /*
                   * Any module name will do. This will map (by default) to a MyModule.json file in XXX.
                   * If you require a single configuration file 'Main" might be a better module name.
                   * But if you have multiple modules with configuration data, pick a name that matches that module,
                   * and they will all be stored under a folder for all your apps configuration.
                   */
                  "MyModule"sv,

                  /*
                   * C++ doesn't have intrinsically enough metadata to effectively serialize deserialize data, but its close.
                   * You have to give it class mappings, and other non-builtin types mappings, so that it can serialize.
                   *
                   * Note - this serializing logic is VERY widely useful outside of configuration - for example it can be used
                   * to provide WebService/REST interfaces, or for debugging/logging output.
                   */
                  [] () -> ObjectVariantMapper {
                      ObjectVariantMapper mapper;
                      mapper.AddClass<MoreOptionsData_> ({
                          {"Enabled"sv, StructFieldMetaInfo{&MoreOptionsData_::fEnabled}},
                          {"Last-Synchronized-At"sv, StructFieldMetaInfo{&MoreOptionsData_::fLastSynchronizedAt}},
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
                   *       FileSystem::WellKnownLocations::GetApplicationData () + appName + String{IO::FileSystem::kPathComponentSeperator} + moduleName + suffix
                   * or folder:
                   *      "/var/opt/Put-Your-App-Name-Here" or "C:\ProgramData\Put-Your-App-Name-Here"
                   *  and this module configuration file would be:
                   *      "/var/opt/Put-Your-App-Name-Here/MyModule.json" OR
                   *      "C:/ProgramData/Put-Your-App-Name-Here/MyModule.json" OR
                   *
                   *    \note - this function does NOT create the 'Put-Your-App-Name-Here' folder first, and will NOT persist
                   *            files if this folder does not exist.
                   *
                   *            Callers can easily replace the default function provided in OptionsFile::mkFilenameMapper - just
                   *            don't call that and provide your own lambda - to create the folder.
                   *
                   *            But a better pattern is to create the folder in your application installer, typically.
                   */
                  OptionsFile::mkFilenameMapper ("Put-Your-App-Name-Here"sv)}
            , fActualCurrentConfigData_{fOptionsFile_.Read<MoreOptionsData_> (MoreOptionsData_{})}
{
    Set (fActualCurrentConfigData_); // assure derived data (and changed fields etc) up to date
}
MoreOptionsData_ AppSettings::Private_::MoreOptionsData_Storage_IMPL_::Get () const
{
    // no locking required here for thread safety.
    // This is always done inside of a read or a full lock by ModuleGetterSetter
    return fActualCurrentConfigData_;
}
void AppSettings::Private_::MoreOptionsData_Storage_IMPL_::Set (const MoreOptionsData_& v)
{
    // no locking required here for thread safety.
    // This is always done inside of a write lock by ModuleGetterSetter
    fActualCurrentConfigData_ = v;
    fOptionsFile_.Write (v);
}

/*
 ********************************************************************************
 *************************** AppSettings::TestUse1 ******************************
 ********************************************************************************
 */
void AppSettings::TestUse1 ()
{
    // This will be by far the most common use pattern - just read some field of the configuration object
    if (gModuleConfiguration.Get ().fEnabled) {
        // do something
    }
}

/*
 ********************************************************************************
 *************************** AppSettings::TestUse2 ******************************
 ********************************************************************************
 */
void AppSettings::TestUse2 ()
{
    // or read several fields all guaranteed within this same snapshot (not holding a lock duing the action)
    auto d = gModuleConfiguration.Get ();
    // lock not held here so configuration could change but this code remains safe and crash free
    if (d.fEnabled and d.fLastSynchronizedAt) {
        // do something
    }
}

/*
 ********************************************************************************
 *************************** AppSettings::TestUse3 ******************************
 ********************************************************************************
 */
void AppSettings::TestUse3 ()
{
    if (gModuleConfiguration.Get ().fEnabled) {
        // a non-atomic update of the entire MoreOptionsData_ object
        auto n     = gModuleConfiguration.Get ();
        n.fEnabled = false; // change something in 'n' here
        gModuleConfiguration.Set (n);
    }
}

/*
 ********************************************************************************
 *************************** AppSettings::TestUse4 ******************************
 ********************************************************************************
 */
void AppSettings::TestUse4 ()
{
    // Use Update () to atomically update data
    // Use the return value to tell if a real change was made (so you can invoke some sort of notification/action)
    static const Duration kMinTime_ = 2min;
    if (gModuleConfiguration.Update ([] (const MoreOptionsData_& data) -> optional<MoreOptionsData_> {
            if (data.fLastSynchronizedAt and *data.fLastSynchronizedAt + kMinTime_ > DateTime::Now ()) {
                MoreOptionsData_ result    = data;
                result.fLastSynchronizedAt = DateTime::Now ();
                return result;
            }
            return {};
        })) {
        sWaitableEvent_.Set (); // e.g. trigger someone to wakeup and used changes? - no global lock held here...
    }
}