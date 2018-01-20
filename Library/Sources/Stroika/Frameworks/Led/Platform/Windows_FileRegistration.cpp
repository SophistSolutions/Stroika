/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../../Foundation/StroikaPreComp.h"

#include "../../../Foundation/Memory/SmallStackBuffer.h"

#include "Windows_FileRegistration.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;
using namespace Stroika::Frameworks::Led::Platform;

using Memory::SmallStackBuffer;

/**
 *  @todo   Must fix to properly support 32-bit and 64-bit safety
 */
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(4 : 4267)
#endif

#if qPlatform_Windows

inline void ThrowIfRegError (LONG e)
{
    if (e != ERROR_SUCCESS) {
        throw e;
    }
}

/*
 ********************************************************************************
 ************ Win32FileAssociationRegistrationHelper::KeyHolder *****************
 ********************************************************************************
 */
inline Win32FileAssociationRegistrationHelper::KeyHolder::KeyHolder (HKEY hk)
    : fKey (hk)
{
}
inline Win32FileAssociationRegistrationHelper::KeyHolder::KeyHolder (HKEY baseKey, LPCTSTR lpSubKey)
    : fKey (NULL)
{
    ThrowIfRegError (::RegOpenKey (baseKey, lpSubKey, &fKey));
}
inline Win32FileAssociationRegistrationHelper::KeyHolder::KeyHolder (HKEY baseKey, LPCTSTR lpSubKey, CreateIfNotThereFlag createIfNotThereFlag)
    : fKey (NULL)
{
    Assert (createIfNotThereFlag == eCreateIfNotThere);
    DWORD ignored = 0;
    ThrowIfRegError (::RegCreateKeyEx (baseKey, lpSubKey, 0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_READ, NULL, &fKey, &ignored));
}
inline Win32FileAssociationRegistrationHelper::KeyHolder::~KeyHolder ()
{
    ::RegCloseKey (fKey);
}
inline Win32FileAssociationRegistrationHelper::KeyHolder::operator HKEY ()
{
    return fKey;
}

/*
 ********************************************************************************
 ***************** Win32FileAssociationRegistrationHelper ***********************
 ********************************************************************************
 */
Win32FileAssociationRegistrationHelper::Win32FileAssociationRegistrationHelper (const Led_SDK_String& fileSuffix)
    : fFileSuffix (fileSuffix)
{
}

Led_SDK_String Win32FileAssociationRegistrationHelper::GetAssociatedProgID () const
{
    try {
        LONG itemLen = 0;
        ThrowIfRegError (::RegQueryValue (HKEY_CLASSES_ROOT, fFileSuffix.c_str (), NULL, &itemLen));
        SmallStackBuffer<Led_SDK_Char> buf (itemLen + 1);
        ThrowIfRegError (::RegQueryValue (HKEY_CLASSES_ROOT, fFileSuffix.c_str (), buf, &itemLen));
        return Led_SDK_String (buf);
    }
    catch (...) {
        return Led_SDK_String ();
    }
}

Led_SDK_String Win32FileAssociationRegistrationHelper::GetAssociatedDefaultIcon () const
{
    try {
        Led_SDK_String progID = GetAssociatedProgID ();
        KeyHolder      progIDKey (HKEY_CLASSES_ROOT, progID.c_str ());
        LONG           itemLen = 0;
        ThrowIfRegError (::RegQueryValue (progIDKey, Led_SDK_TCHAROF ("DefaultIcon"), NULL, &itemLen));
        SmallStackBuffer<Led_SDK_Char> buf (itemLen + 1);
        ThrowIfRegError (::RegQueryValue (progIDKey, Led_SDK_TCHAROF ("DefaultIcon"), buf, &itemLen));
        return Led_SDK_String (buf);
    }
    catch (...) {
        return Led_SDK_String ();
    }
}

Led_SDK_String Win32FileAssociationRegistrationHelper::GetAssociatedEditCommand () const
{
    try {
        Led_SDK_String progID = GetAssociatedProgID ();
        KeyHolder      progIDKey (HKEY_CLASSES_ROOT, progID.c_str ());
        KeyHolder      shellKey (progIDKey, Led_SDK_TCHAROF ("shell"));
        KeyHolder      openKey (shellKey, Led_SDK_TCHAROF ("edit"));
        LONG           itemLen = 0;
        ThrowIfRegError (::RegQueryValue (openKey, Led_SDK_TCHAROF ("command"), NULL, &itemLen));
        SmallStackBuffer<Led_SDK_Char> buf (itemLen + 1);
        ThrowIfRegError (::RegQueryValue (openKey, Led_SDK_TCHAROF ("command"), buf, &itemLen));
        return Led_SDK_String (buf);
    }
    catch (...) {
        return Led_SDK_String ();
    }
}

Led_SDK_String Win32FileAssociationRegistrationHelper::GetAssociatedOpenCommand () const
{
    try {
        Led_SDK_String progID = GetAssociatedProgID ();
        KeyHolder      progIDKey (HKEY_CLASSES_ROOT, progID.c_str ());
        KeyHolder      shellKey (progIDKey, Led_SDK_TCHAROF ("shell"));
        KeyHolder      openKey (shellKey, Led_SDK_TCHAROF ("open"));
        LONG           itemLen = 0;
        ThrowIfRegError (::RegQueryValue (openKey, Led_SDK_TCHAROF ("command"), NULL, &itemLen));
        SmallStackBuffer<Led_SDK_Char> buf (itemLen + 1);
        ThrowIfRegError (::RegQueryValue (openKey, Led_SDK_TCHAROF ("command"), buf, &itemLen));
        return Led_SDK_String (buf);
    }
    catch (...) {
        return Led_SDK_String ();
    }
}

void Win32FileAssociationRegistrationHelper::SetAssociatedProgIDAndOpenCommand (const Led_SDK_String& progID, const Led_SDK_String& progIDPrettyName, const Led_SDK_String& defaultIcon, const Led_SDK_String& editCommandLine, const Led_SDK_String& openCommandLine)
{
    /*
     *  Make HKCR/SUFFIX point to the progID
     */
    ThrowIfRegError (::RegSetValue (HKEY_CLASSES_ROOT, fFileSuffix.c_str (), REG_SZ, progID.c_str (), progID.length ()));

    /*
     *  The Create/Make the pointed to progID, with appropriate subkeys.
     */
    ThrowIfRegError (::RegSetValue (HKEY_CLASSES_ROOT, progID.c_str (), REG_SZ, progIDPrettyName.c_str (), progIDPrettyName.length ()));

    KeyHolder progIDKey (HKEY_CLASSES_ROOT, progID.c_str (), KeyHolder::eCreateIfNotThere);
    if (defaultIcon != Win32UIFileAssociationInfo::kNoChange) {
        ThrowIfRegError (::RegSetValue (progIDKey, Led_SDK_TCHAROF ("DefaultIcon"), REG_SZ, defaultIcon.c_str (), defaultIcon.length ()));
    }
    if (editCommandLine != Win32UIFileAssociationInfo::kNoChange) {
        KeyHolder shellKey (progIDKey, Led_SDK_TCHAROF ("shell"), KeyHolder::eCreateIfNotThere);
        KeyHolder openKey (shellKey, Led_SDK_TCHAROF ("edit"), KeyHolder::eCreateIfNotThere);
        ThrowIfRegError (::RegSetValue (openKey, Led_SDK_TCHAROF ("command"), REG_SZ, editCommandLine.c_str (), editCommandLine.length ()));
    }
    if (openCommandLine != Win32UIFileAssociationInfo::kNoChange) {
        KeyHolder shellKey (progIDKey, Led_SDK_TCHAROF ("shell"), KeyHolder::eCreateIfNotThere);
        KeyHolder openKey (shellKey, Led_SDK_TCHAROF ("open"), KeyHolder::eCreateIfNotThere);
        ThrowIfRegError (::RegSetValue (openKey, Led_SDK_TCHAROF ("command"), REG_SZ, openCommandLine.c_str (), openCommandLine.length ()));
    }
}

/*
 ********************************************************************************
 ************************ Win32UIFileAssociationInfo ****************************
 ********************************************************************************
 */
Led_SDK_String Win32UIFileAssociationInfo::kNoChange;
Win32UIFileAssociationInfo::Win32UIFileAssociationInfo (
    const Led_SDK_String& fileSuffix,
    const Led_SDK_String& fileProgID,
    const Led_SDK_String& fileProgIDPrettyName,
    const Led_SDK_String& defaultIcon,
    const Led_SDK_String& shellEditNOpenCommandLine)
    : fFileSuffix (fileSuffix)
    , fFileProgID (fileProgID)
    , fFileProgIDPrettyName (fileProgIDPrettyName)
    , fDefaultIcon (defaultIcon)
    , fShellEditCommandLine (shellEditNOpenCommandLine)
    , fShellOpenCommandLine (shellEditNOpenCommandLine)
{
}

Win32UIFileAssociationInfo::Win32UIFileAssociationInfo (
    const Led_SDK_String& fileSuffix,
    const Led_SDK_String& fileProgID,
    const Led_SDK_String& fileProgIDPrettyName,
    const Led_SDK_String& defaultIcon,
    const Led_SDK_String& shellEditCommandLine,
    const Led_SDK_String& shellOpenCommandLine)
    : fFileSuffix (fileSuffix)
    , fFileProgID (fileProgID)
    , fFileProgIDPrettyName (fileProgIDPrettyName)
    , fDefaultIcon (defaultIcon)
    , fShellEditCommandLine (shellEditCommandLine)
    , fShellOpenCommandLine (shellOpenCommandLine)
{
}

/*
 ********************************************************************************
 *************** Win32UIFileAssociationRegistrationHelper ***********************
 ********************************************************************************
 */
Win32UIFileAssociationRegistrationHelper::Win32UIFileAssociationRegistrationHelper (HINSTANCE hInstance)
    : fHINSTANCE (hInstance)
    , fInfoRecs ()
{
}

Win32UIFileAssociationRegistrationHelper::Win32UIFileAssociationRegistrationHelper (HINSTANCE hInstance, const vector<Win32UIFileAssociationInfo>& infoRecs)
    : fHINSTANCE (hInstance)
    , fInfoRecs (infoRecs)
{
}

void Win32UIFileAssociationRegistrationHelper::Add (const Win32UIFileAssociationInfo& infoRec)
{
    fInfoRecs.push_back (infoRec);
}

void Win32UIFileAssociationRegistrationHelper::DoIt () noexcept
{
    /*
     *  By default - if this fails - just ignore the failure.
     */
    try {
        bool doRegister = true;
        if (RegisteredToSomeoneElse () and not CheckUserSaysOKToUpdate ()) {
            doRegister = false;
        }
        if (doRegister) {
            ApplyChangesSilently ();
        }
    }
    catch (...) {
    }
}

bool Win32UIFileAssociationRegistrationHelper::RegisteredToSomeoneElse () const
{
    for (auto i = fInfoRecs.begin (); i != fInfoRecs.end (); ++i) {
        /*
         *  NB: Only check if the progid and the open command are the same as us. Don't bother reporting to the user we need to
         *  update things if all that differs is the icon, or something like that.
         *
         *  Also - if there are any exceptions - its probably cuz the keys aren't there to begin with, and so we assume
         *  we are the first to specify that key/suffix/etc.
         */
        try {
            Win32FileAssociationRegistrationHelper registryAssoc ((*i).fFileSuffix);
            Led_SDK_String                         progid          = registryAssoc.GetAssociatedProgID ();
            Led_SDK_String                         assocEditCmd    = registryAssoc.GetAssociatedEditCommand ();
            Led_SDK_String                         assocOpenCmd    = registryAssoc.GetAssociatedOpenCommand ();
            Led_SDK_String                         editCommandLine = (*i).fShellEditCommandLine;
            Led_SDK_String                         openCommandLine = (*i).fShellOpenCommandLine;
            ExpandVariables (&editCommandLine);
            ExpandVariables (&openCommandLine);
            if (progid != (*i).fFileProgID or
                (not editCommandLine.empty () and assocEditCmd != editCommandLine) or
                (not openCommandLine.empty () and assocOpenCmd != openCommandLine)) {
                return true;
            }
        }
        catch (...) {
            // ignore - check for conflicts on the rest...
        }
    }
    return false;
}

void Win32UIFileAssociationRegistrationHelper::ApplyChangesSilently ()
{
    // for each info guy - check that ALL already pointed to our app
    for (auto i = fInfoRecs.begin (); i != fInfoRecs.end (); ++i) {
        Win32FileAssociationRegistrationHelper registryAssoc ((*i).fFileSuffix);
        Led_SDK_String                         defaultIcon     = (*i).fDefaultIcon;
        Led_SDK_String                         editCommandLine = (*i).fShellEditCommandLine;
        Led_SDK_String                         openCommandLine = (*i).fShellOpenCommandLine;
        ExpandVariables (&defaultIcon);
        ExpandVariables (&editCommandLine);
        ExpandVariables (&openCommandLine);
        try {
            registryAssoc.SetAssociatedProgIDAndOpenCommand ((*i).fFileProgID, (*i).fFileProgIDPrettyName, defaultIcon, editCommandLine, openCommandLine);
        }
        catch (...) {
            // Ignore any errors updating these registry settings - not important enough to warn users about, and not clearly a problem
            // (users may have security settings on these registration keys preventing our update).
        }
    }
}

bool Win32UIFileAssociationRegistrationHelper::CheckUserSaysOKToUpdate () const
{
    // subclasses may want to put up a dialog here asking the user. Here we just assume they want the changes
    // applied
    return true;
}

void Win32UIFileAssociationRegistrationHelper::ExpandVariables (Led_SDK_String* valWithVars) const
{
    /*
     *  Search for strings like $EXE$ and replace them with the value of the EXE location.
     */
    size_t varAt = 0;
    if ((varAt = valWithVars->find (Led_SDK_TCHAROF ("$EXE$"))) != Led_SDK_String::npos) {
        TCHAR szLongPathName[_MAX_PATH];
        ::GetModuleFileName (fHINSTANCE, szLongPathName, _MAX_PATH);
        valWithVars->replace (varAt, 5, szLongPathName);
    }
}
#endif
