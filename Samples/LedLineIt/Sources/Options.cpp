/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"
#include <afxadv.h>

#include "Stroika/Frameworks/Led/OptionsSupport.h"

#include "Options.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Led;

namespace {
#if qPlatform_Windows
    class MyPrefs : public OptionsFileHelper {
    private:
        using inherited = OptionsFileHelper;

    public:
        MyPrefs ()
            : inherited (OpenWithCreateAlongPath (HKEY_CURRENT_USER, _T("Software\\Sophist Solutions, Inc.\\LedLineIt!\\Settings")))
        {
        }
    };
    MyPrefs gMyPrefsFile;
#endif
}

using SearchParameters = TextInteractor::SearchParameters;

static constexpr TCHAR kDockBarStateEntry[]               = _T ("DockBarState");
static constexpr TCHAR kSearchParamsMatchString[]         = _T ("MatchString");
static constexpr TCHAR kSearchParamsRecentMatchStrings[]  = _T ("RecentMatchStrings");
static constexpr TCHAR kSearchParamsWrapSearch[]          = _T ("WrapSearch");
static constexpr TCHAR kSearchParamsWholeWordSearch[]     = _T ("WholeWordSearch");
static constexpr TCHAR kSearchParamsCaseSensativeSearch[] = _T ("CaseSensativeSearch");
static constexpr TCHAR kSmartCutAndPaste[]                = _T ("SmartCutAndPaste");
static constexpr TCHAR kAutoIndent[]                      = _T ("AutoIndent");
static constexpr TCHAR kTabAutoShiftsText[]               = _T ("TabAutoShiftsText");
static constexpr TCHAR kSyntaxColoring[]                  = _T ("kSyntaxColoring");
static constexpr TCHAR kCheckFileAssocAtStartup[]         = _T ("CheckFileAssocAtStartup");
static constexpr TCHAR kDefaultNewDocFont[]               = _T ("DefaultNewDocFont");

/*
 ********************************************************************************
 *********************************** Options ************************************
 ********************************************************************************
 */

Options::Options ()
{
}

Options::~Options ()
{
}

SearchParameters Options::GetSearchParameters () const
{
    SearchParameters sp;
    sp.fMatchString         = gMyPrefsFile.GetPref (kSearchParamsMatchString, sp.fMatchString);
    sp.fRecentFindStrings   = gMyPrefsFile.GetPref (kSearchParamsRecentMatchStrings, sp.fRecentFindStrings);
    sp.fWrapSearch          = gMyPrefsFile.GetPref (kSearchParamsWrapSearch, sp.fWrapSearch);
    sp.fWholeWordSearch     = gMyPrefsFile.GetPref (kSearchParamsWholeWordSearch, sp.fWholeWordSearch);
    sp.fCaseSensativeSearch = gMyPrefsFile.GetPref (kSearchParamsCaseSensativeSearch, sp.fCaseSensativeSearch);
    return sp;
}

void Options::SetSearchParameters (const SearchParameters& searchParameters)
{
    gMyPrefsFile.StorePref (kSearchParamsMatchString, searchParameters.fMatchString);
    gMyPrefsFile.StorePref (kSearchParamsRecentMatchStrings, searchParameters.fRecentFindStrings);
    gMyPrefsFile.StorePref (kSearchParamsWrapSearch, searchParameters.fWrapSearch);
    gMyPrefsFile.StorePref (kSearchParamsWholeWordSearch, searchParameters.fWholeWordSearch);
    gMyPrefsFile.StorePref (kSearchParamsCaseSensativeSearch, searchParameters.fCaseSensativeSearch);
}

#if qPlatform_Windows
const CDockState& Options::GetDocBarState () const
{
    static CDockState dockState; // keep static copy and clear each time cuz CDocState doesn't support copy CTOR - LGP971214
    dockState.Clear ();
    vector<byte> bytes;
    if (gMyPrefsFile.LookupPref (kDockBarStateEntry, &bytes)) {
        CMemFile file;
        file.Write (&*bytes.begin (), bytes.size ());
        file.SeekToBegin ();
        CArchive ar (&file, CArchive::load);
        dockState.Serialize (ar);
        ar.Close ();
    }
    return dockState;
}

void Options::SetDocBarState (const CDockState& dockState)
{
    CMemFile    file;
    CArchive    ar (&file, CArchive::store);
    CDockState& ds = const_cast<CDockState&> (dockState); // Serialize/Write shouldn't change object!
    ds.Serialize (ar);
    ar.Close ();
    ULONG nSize = static_cast<ULONG> (file.GetLength ());
    ASSERT (nSize < 4096);
    byte* p = new byte[nSize];
    file.SeekToBegin ();
    file.Read (p, nSize);
    gMyPrefsFile.StorePref (kDockBarStateEntry, nSize, p);
    delete[] p;
}
#endif

bool Options::GetSmartCutAndPaste () const
{
    return gMyPrefsFile.GetPref (kSmartCutAndPaste, true);
}

void Options::SetSmartCutAndPaste (bool smartCutAndPaste)
{
    gMyPrefsFile.StorePref (kSmartCutAndPaste, smartCutAndPaste);
}

bool Options::GetAutoIndent () const
{
    return gMyPrefsFile.GetPref (kAutoIndent, true);
}

void Options::SetAutoIndent (bool autoIndent)
{
    gMyPrefsFile.StorePref (kAutoIndent, autoIndent);
}

bool Options::GetTreatTabAsIndentChar () const
{
    return gMyPrefsFile.GetPref (kTabAutoShiftsText, true);
}

void Options::SetTreatTabAsIndentChar (bool tabAsIndentChar)
{
    gMyPrefsFile.StorePref (kTabAutoShiftsText, tabAsIndentChar);
}

#if qSupportSyntaxColoring
Options::SyntaxColoringOption Options::GetSyntaxColoringOption () const
{
    const SyntaxColoringOption kDefault = eSyntaxColoringNone;
    int                        result   = gMyPrefsFile.GetPref<int> (kSyntaxColoring, kDefault);
    if (result >= 1 and result <= 3) {
        return static_cast<Options::SyntaxColoringOption> (result);
    }
    else {
        return kDefault;
    }
}

void Options::SetSyntaxColoringOption (SyntaxColoringOption syntaxColoringOption)
{
    gMyPrefsFile.StorePref (kSyntaxColoring, syntaxColoringOption);
}
#endif

#if qPlatform_Windows
bool Options::GetCheckFileAssocsAtStartup () const
{
    return gMyPrefsFile.GetPref (kCheckFileAssocAtStartup, true);
}

void Options::SetCheckFileAssocsAtStartup (bool checkFileAssocsAtStartup)
{
    gMyPrefsFile.StorePref (kCheckFileAssocAtStartup, checkFileAssocsAtStartup);
}
#endif

Led_FontSpecification Options::GetDefaultNewDocFont () const
{
    vector<byte> bytes;
    if (gMyPrefsFile.LookupPref (kDefaultNewDocFont, &bytes)) {
        if (bytes.size () == sizeof (LOGFONT)) {
            Led_FontSpecification fsp;
            fsp.SetOSRep (*reinterpret_cast<LOGFONT*> (&*bytes.begin ()));
            return fsp;
        }
    }

    // A good default font for LedLineIt - really just want something monospace,
    // but don't know how better to choose...
    // Not TOO important what we do here. Really we should get/save a user-chosen default in the
    // prefs file!
    Led_FontSpecification defFont = TextImager::GetStaticDefaultFont ();
    defFont.SetFontName (_T ("Courier New"));
    defFont.SetPointSize (10);
    return defFont;
}

void Options::SetDefaultNewDocFont (const Led_FontSpecification& defaultNewDocFont)
{
    auto tmp = defaultNewDocFont.GetOSRep ();
    gMyPrefsFile.StorePref (kDefaultNewDocFont, sizeof (LOGFONT), reinterpret_cast<const byte*> (&tmp));
}
