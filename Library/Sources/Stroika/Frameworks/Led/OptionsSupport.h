/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_OptionsSupport_h_
#define _Stroika_Frameworks_Led_OptionsSupport_h_ 1

/*
@MODULE:    LedOptionsSupport
@DESCRIPTION:
        <p></p>
 */

#include "Support.h"

namespace Stroika {
    namespace Frameworks {
        namespace Led {

            /*
            @CLASS:         OptionsFileHelper
            @DESCRIPTION:   <p>Helper class to access options files (on Win32 -
                        registry based, and on MacOS - resource file based).</p>
                            <p>NB: only Windows is implemented so far (2003-12-30).</p>
            */
            class OptionsFileHelper {
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
#if qDefaultValueInTemplateParemeterUsingTemplatedTypeBug
                template <typename T>
                nonvirtual T GetPref (const Led_SDK_Char* prefName, T defaultValue);
                template <typename T>
                nonvirtual T GetPref (const Led_SDK_Char* prefName)
                {
                    T defValue;
                    return GetPref (prefName, defValue);
                }
#else
                template <typename T>
                nonvirtual T GetPref (const Led_SDK_Char* prefName, T defaultValue = T{});
#endif

#if qMemberTemplateNeedsExplicitDeclarationForEachTypeBug
            public:
                nonvirtual bool GetPref (const Led_SDK_Char* prefName, bool defaultValue);
                nonvirtual string GetPref (const Led_SDK_Char* prefName, string defaultValue);
                nonvirtual wstring GetPref (const Led_SDK_Char* prefName, wstring defaultValue);
                nonvirtual vector<string> GetPref (const Led_SDK_Char* prefName, vector<string> defaultValue);
                nonvirtual vector<wstring> GetPref (const Led_SDK_Char* prefName, vector<wstring> defaultValue);
#endif

            public:
                nonvirtual bool LookupPref (const Led_SDK_Char* prefName, string* value);
                nonvirtual bool LookupPref (const Led_SDK_Char* prefName, wstring* value);
                nonvirtual bool LookupPref (const Led_SDK_Char* prefName, bool* value);
                nonvirtual bool LookupPref (const Led_SDK_Char* prefName, int* value);
                nonvirtual bool LookupPref (const Led_SDK_Char* prefName, vector<Byte>* value);
                nonvirtual bool LookupPref (const Led_SDK_Char* prefName, vector<string>* value);
                nonvirtual bool LookupPref (const Led_SDK_Char* prefName, vector<wstring>* value);

            public:
                nonvirtual void StorePref (const Led_SDK_Char* prefName, const string& value);
                nonvirtual void StorePref (const Led_SDK_Char* prefName, const wstring& value);
                nonvirtual void StorePref (const Led_SDK_Char* prefName, bool value);
                nonvirtual void StorePref (const Led_SDK_Char* prefName, int value);
                nonvirtual void StorePref (const Led_SDK_Char* prefName, size_t nBytes, const Byte* data);
                nonvirtual void StorePref (const Led_SDK_Char* prefName, const vector<string>& value);
                nonvirtual void StorePref (const Led_SDK_Char* prefName, const vector<wstring>& value);

            private:
#if qPlatform_MacOS
                int fResFile;
#elif qPlatform_Windows
                HKEY         fKey;
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

#if qMemberTemplateNeedsExplicitDeclarationForEachTypeBug
            inline bool OptionsFileHelper::GetPref (const Led_SDK_Char* prefName, bool defaultValue)
            {
                bool result;
                if (LookupPref (prefName, &result)) {
                    return result;
                }
                else {
                    return defaultValue;
                }
            }
            inline string OptionsFileHelper::GetPref (const Led_SDK_Char* prefName, string defaultValue)
            {
                string result;
                if (LookupPref (prefName, &result)) {
                    return result;
                }
                else {
                    return defaultValue;
                }
            }
            inline wstring OptionsFileHelper::GetPref (const Led_SDK_Char* prefName, wstring defaultValue)
            {
                wstring result;
                if (LookupPref (prefName, &result)) {
                    return result;
                }
                else {
                    return defaultValue;
                }
            }
            inline vector<string> OptionsFileHelper::GetPref (const Led_SDK_Char* prefName, vector<string> defaultValue)
            {
                vector<string> result;
                if (LookupPref (prefName, &result)) {
                    return result;
                }
                else {
                    return defaultValue;
                }
            }
            inline vector<wstring> OptionsFileHelper::GetPref (const Led_SDK_Char* prefName, vector<wstring> defaultValue)
            {
                vector<wstring> result;
                if (LookupPref (prefName, &result)) {
                    return result;
                }
                else {
                    return defaultValue;
                }
            }
#endif
        }
    }
}

#endif /*_Stroika_Frameworks_Led_OptionsSupport_h_*/
