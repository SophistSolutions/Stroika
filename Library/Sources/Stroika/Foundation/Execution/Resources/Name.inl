/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../../Characters/Format.h"

namespace Stroika::Foundation::Execution::Resources {

    /*
     ********************************************************************************
     ********************************* Resources::Name ******************************
     ********************************************************************************
     */
    inline Name::Name (const String& name, ResourceType type)
        : fName_ (name.AsSDKString ())
#if qPlatform_Windows
        , fIntName_ ()
#endif
        , fType_ (type)
    {
    }
#if qPlatform_Windows
    inline Name::Name (const int intResName, ResourceType type)
        : fName_ ()
        , fIntName_ (intResName)
        , fType_ (type)
    {
    }
#endif
    inline String Name::GetPrintName () const
    {
#if qPlatform_Windows
        if (fIntName_.has_value ()) {
            return Characters::Format ("#{}"_f, *fIntName_);
        }
#endif
        return String::FromSDKString (fName_);
    }
    inline const SDKChar* Name::GetSDKString () const
    {
#if qPlatform_Windows
        if (fIntName_.has_value ()) {
            return MAKEINTRESOURCE (*fIntName_);
        }
#endif
        return fName_.c_str ();
    }
    inline ResourceType Name::GetType () const
    {
        return fType_;
    }

}
