/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::Configuration::Platform::Windows {

    /*
     ********************************************************************************
     ********************************* RegistryKey **********************************
     ********************************************************************************
     */
    constexpr RegistryKey::RegistryKey (HKEY hkey, bool owned)
        : fKey_ (hkey)
        , fOwned_{owned}
    {
        static_assert (nullptr != INVALID_HANDLE_VALUE);
        Require (hkey != nullptr);
        Require (hkey != INVALID_HANDLE_VALUE);
    }
    inline RegistryKey::~RegistryKey ()
    {
        Assert (nullptr != INVALID_HANDLE_VALUE);
        Assert (fKey_ != nullptr);
        Assert (fKey_ != INVALID_HANDLE_VALUE);
        if (fOwned_) {
            ::RegCloseKey (fKey_);
        }
    }
    inline RegistryKey::operator HKEY () const
    {
        Ensure (fKey_ != nullptr);
        Ensure (fKey_ != INVALID_HANDLE_VALUE);
        return fKey_;
    }

}
