/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../Foundation/StroikaPreComp.h"

#include "../../Foundation/Characters/CString/Utilities.h"
#include "../../Foundation/Characters/Format.h"

#include "OptionsSupport.h"

using namespace Stroika::Foundation;

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

/**
 *  @todo   Must fix to properly support 32-bit and 64-bit safety
 */
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(4 : 4267)
#endif

/*
 ********************************************************************************
 ********************************* OptionsFileHelper ****************************
 ********************************************************************************
 */
/*
@METHOD:        OptionsFileHelper::OptionsFileHelper
@DESCRIPTION:   CTOR on Mac takes open resource file handle as argument, and on Windows - takes open HKEY registry
    handle. On both cases - this class takes over ownership - and closes the handle upon descruction.
*/
#if qPlatform_MacOS
OptionsFileHelper::OptionsFileHelper (int resFile)
    : fResFile (resFile)
{
}
#elif qPlatform_Windows
OptionsFileHelper::OptionsFileHelper (HKEY hkey)
    : fKey (hkey)
{
}
#endif

OptionsFileHelper::~OptionsFileHelper ()
{
#if qPlatform_MacOS
    Assert (false); // NYI
#elif qPlatform_Windows
    ::RegCloseKey (fKey);
#endif
}

#if qPlatform_Windows
/*
@METHOD:        OptionsFileHelper::OpenWithCreateAlongPath
@DESCRIPTION:   <p>Walk the given path (in segments) - and make sure each exists, and create each segment if it doesn't
            exist. Finally - do a regular registry open with access permissions 'samDesired'.</p>
*/
HKEY OptionsFileHelper::OpenWithCreateAlongPath (HKEY parentKey, const Led_SDK_String& path, REGSAM samDesired)
{
    size_t prevPos = 0;
    HKEY   curPar  = parentKey;
    for (;;) {
        size_t         endPos    = path.find ('\\', prevPos);
        Led_SDK_String segName   = path.substr (prevPos, endPos == Led_SDK_String::npos ? endPos : (endPos - prevPos));
        REGSAM         createSAM = (endPos == Led_SDK_String::npos) ? samDesired : KEY_READ;
        HKEY           newKey    = nullptr;

        Led_ThrowIfNotERROR_SUCCESS (::RegCreateKeyEx (curPar, segName.c_str (),
                                                       0, REG_NONE, REG_OPTION_NON_VOLATILE, createSAM, nullptr, &newKey, nullptr));
        if (curPar != parentKey) {
            ::RegCloseKey (curPar);
        }
        curPar = newKey;
        if (endPos == Led_SDK_String::npos) {
            return newKey;
        }
        prevPos = endPos + 1;
    }
}
#endif

/*
@METHOD:        OptionsFileHelper::LookupPref
@DESCRIPTION:   <p>This function is overloaded to return (through pointer parameters) a number of
            different data types, including string, wstring, int, and vector<Byte>). If the given
            named preference (and type) is available, it is returned through that parameter, and
            true is returned. Otherwise, false is returned.</p>
*/
bool OptionsFileHelper::LookupPref (const Led_SDK_Char* prefName, string* value)
{
    RequireNotNull (prefName);
    RequireNotNull (value);
#if qPlatform_MacOS
    Assert (false); // NYI
#elif qPlatform_Windows
    string strValue;
    DWORD  dwType;
    DWORD  dwCount = 0;
    LONG   lResult = ::RegQueryValueExA (fKey, Led_SDKString2ANSI (prefName).c_str (), nullptr, &dwType, nullptr, &dwCount);
    if (lResult == ERROR_SUCCESS) {
        if (dwType == REG_SZ) {
            strValue.resize (dwCount);
            lResult = ::RegQueryValueExA (fKey, Led_SDKString2ANSI (prefName).c_str (), nullptr, &dwType, (LPBYTE) & (*strValue.begin ()), &dwCount);
        }
        else {
            lResult = !ERROR_SUCCESS;
        }
    }
    if (lResult == ERROR_SUCCESS) {
        if (dwType == REG_SZ) {
            *value = strValue.c_str (); // copying like this loses xtra NUL-byte if there is one from read...
            return true;
        }
    }
    return false;
#endif
}

bool OptionsFileHelper::LookupPref (const Led_SDK_Char* prefName, wstring* value)
{
    RequireNotNull (prefName);
    RequireNotNull (value);
#if qPlatform_MacOS
    Assert (false); // NYI
#elif qPlatform_Windows
    wstring strValue;
    DWORD   dwType;
    DWORD   dwCount = 0;
    LONG    lResult = ::RegQueryValueExW (fKey, Led_SDKString2Wide (prefName).c_str (), nullptr, &dwType, nullptr, &dwCount);
    if (lResult == ERROR_SUCCESS) {
        if (dwType == REG_SZ) {
            strValue.resize (dwCount);
            lResult = ::RegQueryValueExW (fKey, Led_SDKString2Wide (prefName).c_str (), nullptr, &dwType, (LPBYTE) & (*strValue.begin ()), &dwCount);
        }
        else {
            lResult = !ERROR_SUCCESS;
        }
    }
    if (lResult == ERROR_SUCCESS) {
        if (dwType == REG_SZ) {
            *value = strValue.c_str (); // copying like this loses xtra NUL-byte if there is one from read...
            return true;
        }
    }
    // If not success - then maybe cuz non-UNICODE functions not available - so try to read a non-UNICODE string
    {
        string tmp;
        if (LookupPref (prefName, &tmp)) {
            *value = ACP2WideString (tmp);
            return true;
        }
    }
    return false;
#endif
}

bool OptionsFileHelper::LookupPref (const Led_SDK_Char* prefName, bool* value)
{
    RequireNotNull (prefName);
    RequireNotNull (value);
    int tmp = 0;
    if (LookupPref (prefName, &tmp)) {
        *value = !!tmp;
        return true;
    }
    return false;
}

bool OptionsFileHelper::LookupPref (const Led_SDK_Char* prefName, int* value)
{
    RequireNotNull (prefName);
    RequireNotNull (value);
#if qPlatform_MacOS
    Assert (false); // NYI
#elif qPlatform_Windows
    DWORD dwValue;
    DWORD dwType;
    DWORD dwCount = sizeof (DWORD);
    LONG  lResult = ::RegQueryValueEx (fKey, prefName, nullptr, &dwType, (LPBYTE)&dwValue, &dwCount);
    if (lResult == ERROR_SUCCESS) {
        if (dwType == REG_DWORD) {
            *value = dwValue;
            return true;
        }
    }
    return false;
#endif
}

bool OptionsFileHelper::LookupPref (const Led_SDK_Char* prefName, vector<Byte>* value)
{
    RequireNotNull (prefName);
    RequireNotNull (value);
#if qPlatform_MacOS
    Assert (false); // NYI
#elif qPlatform_Windows
    DWORD dwType  = 0;
    DWORD dwCount = 0;
    LONG  lResult = ::RegQueryValueEx (fKey, prefName, nullptr, &dwType, nullptr, &dwCount);
    if (lResult == ERROR_SUCCESS) {
        if (dwType == REG_BINARY) {
            value->resize (dwCount);
            lResult = ::RegQueryValueEx (fKey, prefName, nullptr, &dwType, &(*value->begin ()), &dwCount);
        }
        else {
            lResult = !ERROR_SUCCESS;
        }
    }
    return lResult == ERROR_SUCCESS;
#endif
    return false;
}

bool OptionsFileHelper::LookupPref (const Led_SDK_Char* prefName, vector<string>* value)
{
    RequireNotNull (value);
    value->clear ();
    string tmp;
    while (LookupPref (Characters::CString::Format (Led_SDK_TCHAROF ("%s_%d"), prefName, value->size ()).c_str (), &tmp)) {
        value->push_back (tmp);
    }
    return not value->empty ();
}

bool OptionsFileHelper::LookupPref (const Led_SDK_Char* prefName, vector<wstring>* value)
{
    RequireNotNull (value);
    value->clear ();
    wstring tmp;
    while (LookupPref (Characters::CString::Format (Led_SDK_TCHAROF ("%s_%d"), prefName, value->size ()).c_str (), &tmp)) {
        value->push_back (tmp);
    }
    return not value->empty ();
}

void OptionsFileHelper::StorePref (const Led_SDK_Char* prefName, const string& value)
{
    RequireNotNull (prefName);
#if qPlatform_MacOS
    Assert (false); // NYI
#elif qPlatform_Windows
    (void)::RegSetValueExA (fKey, Led_SDKString2ANSI (prefName).c_str (), 0, REG_SZ, (LPBYTE)value.c_str (), (value.length () + 1) * sizeof (TCHAR));
#endif
}

void OptionsFileHelper::StorePref (const Led_SDK_Char* prefName, const wstring& value)
{
    RequireNotNull (prefName);
#if qPlatform_MacOS
    Assert (false); // NYI
#elif qPlatform_Windows
    if (::RegSetValueExW (fKey, Led_SDKString2Wide (prefName).c_str (), 0, REG_SZ, reinterpret_cast<const Byte*> (value.c_str ()), (value.length () + 1) * sizeof (wchar_t)) != ERROR_SUCCESS) {
        // failure to write a UNICODE string could be because the OS doesn't support it - so try writing it as ANSI
        string tmp = Wide2ACPString (value);
        (void)::RegSetValueExA (fKey, Led_SDKString2ANSI (prefName).c_str (), 0, REG_SZ, reinterpret_cast<const Byte*> (tmp.c_str ()), tmp.length () + 1);
    }
#endif
}

void OptionsFileHelper::StorePref (const Led_SDK_Char* prefName, bool value)
{
    RequireNotNull (prefName);
    StorePref (prefName, static_cast<int> (value));
}

void OptionsFileHelper::StorePref (const Led_SDK_Char* prefName, int value)
{
    RequireNotNull (prefName);
#if qPlatform_MacOS
    Assert (false); // NYI
#elif qPlatform_Windows
    DWORD dwValue = value;
    (void)::RegSetValueEx (fKey, prefName, 0, REG_DWORD, (LPBYTE)&dwValue, sizeof (dwValue));
#endif
}

void OptionsFileHelper::StorePref (const Led_SDK_Char* prefName, size_t nBytes, const Byte* data)
{
    RequireNotNull (prefName);
    RequireNotNull (data);
#if qPlatform_MacOS
    Assert (false); // NYI
#elif qPlatform_Windows
    (void)::RegSetValueEx (fKey, prefName, 0, REG_BINARY, (LPBYTE)data, nBytes);
#endif
}

void OptionsFileHelper::StorePref (const Led_SDK_Char* prefName, const vector<string>& value)
{
    RequireNotNull (prefName);
    for (auto i = value.begin (); i != value.end (); ++i) {
        StorePref (Characters::CString::Format (Led_SDK_TCHAROF ("%s_%d"), prefName, i - value.begin ()).c_str (), *i);
    }
}

void OptionsFileHelper::StorePref (const Led_SDK_Char* prefName, const vector<wstring>& value)
{
    RequireNotNull (prefName);
    for (auto i = value.begin (); i != value.end (); ++i) {
        StorePref (Characters::CString::Format (Led_SDK_TCHAROF ("%s_%d"), prefName, i - value.begin ()).c_str (), *i);
    }
}
