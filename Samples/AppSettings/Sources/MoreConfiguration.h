/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _StroikaSample_AppSettings_MoreConfiguration_h_
#define _StroikaSample_AppSettings_MoreConfiguration_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/DataExchange/OptionsFile.h"
#include "Stroika/Foundation/Execution/ModuleGetterSetter.h"
#include "Stroika/Foundation/Time/DateTime.h"

/**
 *   A more complicated example of configuration, fancier usage patterns (e.g. watching for config changes)
 */

namespace StroikaSample::AppSettings {

    using namespace std;
    using namespace Stroika::Foundation;
    using namespace Stroika::Foundation::DataExchange;
    using namespace Stroika::Foundation::Time;

    /*
     *  This is the data you wish to write out as configuration to control your application, or to track persistently.
     */
    struct MoreOptionsData_ {
        bool               fEnabled = false;
        optional<DateTime> fLastSynchronizedAt;
    };

    namespace Private_ {

        /*
         *  OptionsData_Storage_IMPL_ is a 'traits' object, defining a GET and SET method to save/restore
         *  OptionsData_.
         *
         *  This need not worry about thread safety:
         *      o   in the constructor because C++ guarantees this for statically constructed objects
         *      o   and in the Get/Set methods because ModuleGetterSetter manages this locking
         *
         *  A user COULD either choose NOT to persist the data (in which case the logic with fOptionsFile_ 
         *  would be removed/unneeded). Or could persist another way.
         *
         *  But this example shows using OptionsFile to persist the data to a local JSON file, using
         *  the ObjectVariantMapper to serialize/deserialize C++ data structures.
         */
        struct MoreOptionsData_Storage_IMPL_ {
            MoreOptionsData_Storage_IMPL_ ();
            MoreOptionsData_ Get () const;
            void             Set (const MoreOptionsData_& v);

        private:
            OptionsFile      fOptionsFile_;
            MoreOptionsData_ fActualCurrentConfigData_; // automatically initialized just in time, and externally synchronized
        };

    }

    inline Execution::ModuleGetterSetter<MoreOptionsData_, StroikaSample::AppSettings::Private_::MoreOptionsData_Storage_IMPL_> gModuleConfiguration;

    void TestUse1 ();
    void TestUse2 ();
    void TestUse3 ();
    void TestUse4 ();
}
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "MoreConfiguration.inl"

#endif /*_StroikaSample_AppSettings_MoreConfiguration_h_*/
