/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_OptionsSupport_h_
#define _Stroika_Frameworks_Led_OptionsSupport_h_ 1


//////&&&& FILE DEPRECATED &&&& SINCE STROIKA v2.1d22
/*
@MODULE:    LedOptionsSupport
@DESCRIPTION:
        <p></p>
 */

#include "Support.h"

namespace Stroika::Frameworks::Led {

    using std::byte;

    /**
     *  Helper class to access options files (on Win32 -
     *  registry based, and on MacOS - resource file based).
     *  
     *  NB: only Windows is implemented so far (2003-12-30).
     */

    class [[deprecated ("Use OptionsFile to store options, and use Configuration::Platform::Windows::RegistryKey to read the registry - since Stroika v2.1d22")]] OptionsFileHelper
    {
    public:
#if qPlatform_MacOS
        OptionsFileHelper (int resFile);
#elif qPlatform_Windows
        OptionsFileHelper (HKEY hkey);
#endif
        virtual ~OptionsFileHelper ();

#if qPlatform_Windows
    public:
        static HKEY OpenWithCreateAlongPath (HKEY parentKey, const Led_SDK_String& path, REGSAM samDesired = KEY_READ | KEY_WRITE);
#endif

    public:
        template <typename T>
        nonvirtual T GetPref (const Led_SDK_Char* prefName, T defaultValue = T{});

    public:
        nonvirtual bool LookupPref (const Led_SDK_Char* prefName, string* value);
        nonvirtual bool LookupPref (const Led_SDK_Char* prefName, wstring* value);
        nonvirtual bool LookupPref (const Led_SDK_Char* prefName, bool* value);
        nonvirtual bool LookupPref (const Led_SDK_Char* prefName, int* value);
        nonvirtual bool LookupPref (const Led_SDK_Char* prefName, vector<byte>* value);
        nonvirtual bool LookupPref (const Led_SDK_Char* prefName, vector<string>* value);
        nonvirtual bool LookupPref (const Led_SDK_Char* prefName, vector<wstring>* value);

    public:
        nonvirtual void StorePref (const Led_SDK_Char* prefName, const string& value);
        nonvirtual void StorePref (const Led_SDK_Char* prefName, const wstring& value);
        nonvirtual void StorePref (const Led_SDK_Char* prefName, bool value);
        nonvirtual void StorePref (const Led_SDK_Char* prefName, int value);
        nonvirtual void StorePref (const Led_SDK_Char* prefName, size_t nBytes, const byte* data);
        nonvirtual void StorePref (const Led_SDK_Char* prefName, const vector<string>& value);
        nonvirtual void StorePref (const Led_SDK_Char* prefName, const vector<wstring>& value);

    private:
#if qPlatform_MacOS
        int fResFile;
#elif qPlatform_Windows
        HKEY fKey;
#endif
    };

    /*
        ********************************************************************************
        ***************************** Implementation Details ***************************
        ********************************************************************************
        */
    template <typename T>
    inline T OptionsFileHelper::GetPref (const Led_SDK_Char* prefName, T defaultValue)
    {
        T result;
        if (LookupPref (prefName, &result)) {
            return result;
        }
        else {
            return defaultValue;
        }
    }

}

#endif /*_Stroika_Frameworks_Led_OptionsSupport_h_*/
