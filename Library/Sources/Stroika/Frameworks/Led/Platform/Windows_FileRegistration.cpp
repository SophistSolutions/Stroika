/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../../Foundation/StroikaPreComp.h"

#include "../../../Foundation/Memory/StackBuffer.h"

#include "Windows_FileRegistration.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;
using namespace Stroika::Frameworks::Led::Platform;

using Memory::StackBuffer;

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
inline Win32FileAssociationRegistrationHelper::KeyHolder::KeyHolder (HKEY baseKey, LPCTSTR lpSubKey, [[maybe_unused]] CreateIfNotThereFlag createIfNotThereFlag)
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
Win32FileAssociationRegistrationHelper::Win32FileAssociationRegistrationHelper (const SDKString& fileSuffix)
    : fFileSuffix (fileSuffix)
{
}

SDKString Win32FileAssociationRegistrationHelper::GetAssociatedProgID () const
{
    try {
        LONG itemLen = 0;
        ThrowIfRegError (::RegQueryValue (HKEY_CLASSES_ROOT, fFileSuffix.c_str (), NULL, &itemLen));
        StackBuffer<Characters::SDKChar> buf{Memory::eUninitialized, static_cast<size_t> (itemLen + 1)};
        ThrowIfRegError (::RegQueryValue (HKEY_CLASSES_ROOT, fFileSuffix.c_str (), buf.data (), &itemLen));
        return SDKString{buf.data ()};
    }
    catch (...) {
        return SDKString{};
    }
}

SDKString Win32FileAssociationRegistrationHelper::GetAssociatedDefaultIcon () const
{
    try {
        SDKString progID = GetAssociatedProgID ();
        KeyHolder progIDKey (HKEY_CLASSES_ROOT, progID.c_str ());
        LONG      itemLen = 0;
        ThrowIfRegError (::RegQueryValue (progIDKey, Led_SDK_TCHAROF ("DefaultIcon"), NULL, &itemLen));
        StackBuffer<Characters::SDKChar> buf{Memory::eUninitialized, static_cast<size_t> (itemLen + 1)};
        ThrowIfRegError (::RegQueryValue (progIDKey, Led_SDK_TCHAROF ("DefaultIcon"), buf.data (), &itemLen));
        return SDKString{buf.data ()};
    }
    catch (...) {
        return SDKString{};
    }
}

SDKString Win32FileAssociationRegistrationHelper::GetAssociatedEditCommand () const
{
    try {
        SDKString progID = GetAssociatedProgID ();
        KeyHolder progIDKey (HKEY_CLASSES_ROOT, progID.c_str ());
        KeyHolder shellKey (progIDKey, Led_SDK_TCHAROF ("shell"));
        KeyHolder openKey (shellKey, Led_SDK_TCHAROF ("edit"));
        LONG      itemLen = 0;
        ThrowIfRegError (::RegQueryValue (openKey, Led_SDK_TCHAROF ("command"), NULL, &itemLen));
        StackBuffer<Characters::SDKChar> buf{Memory::eUninitialized, static_cast<size_t> (itemLen + 1)};
        ThrowIfRegError (::RegQueryValue (openKey, Led_SDK_TCHAROF ("command"), buf.data (), &itemLen));
        return SDKString{buf.data ()};
    }
    catch (...) {
        return SDKString{};
    }
}

SDKString Win32FileAssociationRegistrationHelper::GetAssociatedOpenCommand () const
{
    try {
        SDKString progID = GetAssociatedProgID ();
        KeyHolder progIDKey (HKEY_CLASSES_ROOT, progID.c_str ());
        KeyHolder shellKey (progIDKey, Led_SDK_TCHAROF ("shell"));
        KeyHolder openKey (shellKey, Led_SDK_TCHAROF ("open"));
        LONG      itemLen = 0;
        ThrowIfRegError (::RegQueryValue (openKey, Led_SDK_TCHAROF ("command"), NULL, &itemLen));
        StackBuffer<Characters::SDKChar> buf{Memory::eUninitialized, static_cast<size_t> (itemLen + 1)};
        ThrowIfRegError (::RegQueryValue (openKey, Led_SDK_TCHAROF ("command"), buf.data (), &itemLen));
        return SDKString{buf.data ()};
    }
    catch (...) {
        return SDKString{};
    }
}

void Win32FileAssociationRegistrationHelper::SetAssociatedProgIDAndOpenCommand (const SDKString& progID, const SDKString& progIDPrettyName,
                                                                                const SDKString& defaultIcon, const SDKString& editCommandLine,
                                                                                const SDKString& openCommandLine)
{
    /*
     *  Make HKCR/SUFFIX point to the progID
     */
    ThrowIfRegError (::RegSetValue (HKEY_CLASSES_ROOT, fFileSuffix.c_str (), REG_SZ, progID.c_str (), static_cast<DWORD> (progID.length ())));

    /*
     *  The Create/Make the pointed to progID, with appropriate subkeys.
     */
    ThrowIfRegError (::RegSetValue (HKEY_CLASSES_ROOT, progID.c_str (), REG_SZ, progIDPrettyName.c_str (),
                                    static_cast<DWORD> (progIDPrettyName.length ())));

    KeyHolder progIDKey (HKEY_CLASSES_ROOT, progID.c_str (), KeyHolder::eCreateIfNotThere);
    if (defaultIcon != Win32UIFileAssociationInfo::kNoChange) {
        ThrowIfRegError (::RegSetValue (progIDKey, Led_SDK_TCHAROF ("DefaultIcon"), REG_SZ, defaultIcon.c_str (),
                                        static_cast<DWORD> (defaultIcon.length ())));
    }
    if (editCommandLine != Win32UIFileAssociationInfo::kNoChange) {
        KeyHolder shellKey (progIDKey, Led_SDK_TCHAROF ("shell"), KeyHolder::eCreateIfNotThere);
        KeyHolder openKey (shellKey, Led_SDK_TCHAROF ("edit"), KeyHolder::eCreateIfNotThere);
        ThrowIfRegError (::RegSetValue (openKey, Led_SDK_TCHAROF ("command"), REG_SZ, editCommandLine.c_str (),
                                        static_cast<DWORD> (editCommandLine.length ())));
    }
    if (openCommandLine != Win32UIFileAssociationInfo::kNoChange) {
        KeyHolder shellKey (progIDKey, Led_SDK_TCHAROF ("shell"), KeyHolder::eCreateIfNotThere);
        KeyHolder openKey (shellKey, Led_SDK_TCHAROF ("open"), KeyHolder::eCreateIfNotThere);
        ThrowIfRegError (::RegSetValue (openKey, Led_SDK_TCHAROF ("command"), REG_SZ, openCommandLine.c_str (),
                                        static_cast<DWORD> (openCommandLine.length ())));
    }
}

/*
 ********************************************************************************
 ************************ Win32UIFileAssociationInfo ****************************
 ********************************************************************************
 */
SDKString Win32UIFileAssociationInfo::kNoChange;
Win32UIFileAssociationInfo::Win32UIFileAssociationInfo (const SDKString& fileSuffix, const SDKString& fileProgID, const SDKString& fileProgIDPrettyName,
                                                        const SDKString& defaultIcon, const SDKString& shellEditNOpenCommandLine)
    : fFileSuffix{fileSuffix}
    , fFileProgID{fileProgID}
    , fFileProgIDPrettyName{fileProgIDPrettyName}
    , fDefaultIcon{defaultIcon}
    , fShellEditCommandLine{shellEditNOpenCommandLine}
    , fShellOpenCommandLine{shellEditNOpenCommandLine}
{
}

Win32UIFileAssociationInfo::Win32UIFileAssociationInfo (const SDKString& fileSuffix, const SDKString& fileProgID,
                                                        const SDKString& fileProgIDPrettyName, const SDKString& defaultIcon,
                                                        const SDKString& shellEditCommandLine, const SDKString& shellOpenCommandLine)
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
            SDKString                              progid          = registryAssoc.GetAssociatedProgID ();
            SDKString                              assocEditCmd    = registryAssoc.GetAssociatedEditCommand ();
            SDKString                              assocOpenCmd    = registryAssoc.GetAssociatedOpenCommand ();
            SDKString                              editCommandLine = (*i).fShellEditCommandLine;
            SDKString                              openCommandLine = (*i).fShellOpenCommandLine;
            ExpandVariables (&editCommandLine);
            ExpandVariables (&openCommandLine);
            if (progid != (*i).fFileProgID or (not editCommandLine.empty () and assocEditCmd != editCommandLine) or
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
        SDKString                              defaultIcon     = (*i).fDefaultIcon;
        SDKString                              editCommandLine = (*i).fShellEditCommandLine;
        SDKString                              openCommandLine = (*i).fShellOpenCommandLine;
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

void Win32UIFileAssociationRegistrationHelper::ExpandVariables (SDKString* valWithVars) const
{
    /*
     *  Search for strings like $EXE$ and replace them with the value of the EXE location.
     */
    size_t varAt = 0;
    if ((varAt = valWithVars->find (Led_SDK_TCHAROF ("$EXE$"))) != SDKString::npos) {
        TCHAR szLongPathName[_MAX_PATH];
        ::GetModuleFileName (fHINSTANCE, szLongPathName, _MAX_PATH);
        valWithVars->replace (varAt, 5, szLongPathName);
    }
}
#endif
