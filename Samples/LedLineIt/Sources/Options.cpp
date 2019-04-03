/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"
#include <afxadv.h>

#include "Stroika/Foundation/DataExchange/OptionsFile.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/ModuleGetterSetter.h"
#include "Stroika/Foundation/Memory/BLOB.h"

#include "Options.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Execution;

using namespace Stroika::Frameworks::Led;

using Memory::BLOB;

using SearchParameters = TextInteractor::SearchParameters;
#if qSupportSyntaxColoring
using SyntaxColoringOption = Options::SyntaxColoringOption;
#endif

namespace {
    struct Options_ {
        BLOB             fDockBarState;
        SearchParameters fSearchParameters{};
        bool             fSmartCutAndPaste{true};
        bool             fAutoIndent{true};
        bool             fTabsAutoShiftsText{true};
#if qSupportSyntaxColoring
        SyntaxColoringOption fSyntaxColoring{SyntaxColoringOption::eSyntaxColoringNone};
#endif
#if qPlatform_Windows
        bool fCheckFileAssocAtStartup{true};
        BLOB fDefaultNewDocFont;
#endif
    };

    struct Options_Storage_IMPL_ {
        Options_Storage_IMPL_ ()
            : fOptionsFile_{
                  L"AppSettings"sv,
                  [] () -> ObjectVariantMapper {
                      ObjectVariantMapper mapper;

                      // really should use String, no longer Led_tString, but for now... (note this only works as is for wchar_t Led_tString
                      mapper.Add<Led_tString> (
                          [] (const ObjectVariantMapper& /*mapper*/, const Led_tString* obj) -> VariantValue {
                              return String{*obj};
                          },
                          [] (const ObjectVariantMapper& /*mapper*/, const VariantValue& d, Led_tString* intoObj) -> void {
                              *intoObj = d.As<String> ().As<Led_tString> ();
                          });
                      mapper.AddCommonType<vector<Led_tString>> ();
                      mapper.AddCommonType<Memory::BLOB> ();

#if qSupportSyntaxColoring
                      mapper.AddCommonType<SyntaxColoringOption> ();
#endif

                      mapper.AddClass<SearchParameters> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
                          {L"Match-String", Stroika_Foundation_DataExchange_StructFieldMetaInfo (SearchParameters, fMatchString)},
                          {L"Wrap-Search", Stroika_Foundation_DataExchange_StructFieldMetaInfo (SearchParameters, fWrapSearch)},
                          {L"Whole-Word-Search", Stroika_Foundation_DataExchange_StructFieldMetaInfo (SearchParameters, fWholeWordSearch)},
                          {L"Case-Sensative-Search", Stroika_Foundation_DataExchange_StructFieldMetaInfo (SearchParameters, fCaseSensativeSearch)},
                          {L"Recent-Match-Strings", Stroika_Foundation_DataExchange_StructFieldMetaInfo (SearchParameters, fRecentFindStrings)},
                      });

                      mapper.AddClass<Options_> (initializer_list<ObjectVariantMapper::StructFieldInfo> {
                          {L"Dock-Bar-State", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Options_, fDockBarState)},
                              {L"Search-Parameters", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Options_, fSearchParameters)},
                              {L"Smart-Cut-And-Paste", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Options_, fSmartCutAndPaste)},
                              {L"Auto-Indent", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Options_, fAutoIndent)},
                              {L"Tabs-Auto-Shifts-Text", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Options_, fTabsAutoShiftsText)},

#if qSupportSyntaxColoring
                              {L"Syntax-Coloring", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Options_, fSyntaxColoring)},
#endif

#if qPlatform_Windows
                              {L"Check-File-Assoc-At-Startup", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Options_, fCheckFileAssocAtStartup)},
                              {L"Default-New-Doc-Font", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Options_, fDefaultNewDocFont)},
#endif
                      });
                      return mapper;
                  }(),

                  OptionsFile::kDefaultUpgrader,

                  OptionsFile::mkFilenameMapper (L"LedLineIt"sv)}
            , fActualCurrentConfigData_ (fOptionsFile_.Read<Options_> (Options_{}))
        {
            Set (fActualCurrentConfigData_); // assure derived data (and changed fields etc) up to date
        }
        Options_ Get () const
        {
            return fActualCurrentConfigData_;
        }
        void Set (const Options_& v)
        {
            fActualCurrentConfigData_ = v;
            fOptionsFile_.Write (v);
        }

    private:
        OptionsFile fOptionsFile_;
        Options_    fActualCurrentConfigData_;
    };

    ModuleGetterSetter<Options_, Options_Storage_IMPL_> sOptions_;
}

/*
 ********************************************************************************
 *********************************** Options ************************************
 ********************************************************************************
 */
SearchParameters Options::GetSearchParameters () const
{
    return sOptions_.Get ().fSearchParameters;
}

void Options::SetSearchParameters (const SearchParameters& searchParameters)
{
    sOptions_.Update ([=] (Options_ d) { d.fSearchParameters = searchParameters; return d; });
}

#if qPlatform_Windows
const CDockState& Options::GetDocBarState () const
{
    static CDockState     dockState; // keep static copy and clear each time cuz CDocState doesn't support copy CTOR - LGP971214
    static std::once_flag sOnce_;
    std::call_once (sOnce_, [] () {
        dockState.Clear ();
        BLOB bytes = sOptions_.Get ().fDockBarState;
        if (not bytes.empty ()) {
            CMemFile file;
            file.Write (&*bytes.begin (), bytes.size ());
            file.SeekToBegin ();
            CArchive ar (&file, CArchive::load);
            dockState.Serialize (ar);
            ar.Close ();
        }
    });
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
    sOptions_.Update ([=] (Options_ d) { d.fDockBarState = BLOB{ p, p + nSize }; return d; });
    delete[] p;
}
#endif

bool Options::GetSmartCutAndPaste () const
{
    return sOptions_.Get ().fSmartCutAndPaste;
}

void Options::SetSmartCutAndPaste (bool smartCutAndPaste)
{
    sOptions_.Update ([=] (Options_ d) { d.fSmartCutAndPaste = smartCutAndPaste; return d; });
}

bool Options::GetAutoIndent () const
{
    return sOptions_.Get ().fAutoIndent;
}

void Options::SetAutoIndent (bool autoIndent)
{
    sOptions_.Update ([=] (Options_ d) { d.fAutoIndent = autoIndent; return d; });
}

bool Options::GetTreatTabAsIndentChar () const
{
    return sOptions_.Get ().fTabsAutoShiftsText;
}

void Options::SetTreatTabAsIndentChar (bool tabAsIndentChar)
{
    sOptions_.Update ([=] (Options_ d) { d.fTabsAutoShiftsText = tabAsIndentChar; return d; });
}

#if qSupportSyntaxColoring
Options::SyntaxColoringOption Options::GetSyntaxColoringOption () const
{
    return sOptions_.Get ().fSyntaxColoring;
}

void Options::SetSyntaxColoringOption (SyntaxColoringOption syntaxColoringOption)
{
    sOptions_.Update ([=] (Options_ d) { d.fSyntaxColoring = syntaxColoringOption; return d; });
}
#endif

#if qPlatform_Windows
bool Options::GetCheckFileAssocsAtStartup () const
{
    return sOptions_.Get ().fCheckFileAssocAtStartup;
}

void Options::SetCheckFileAssocsAtStartup (bool checkFileAssocsAtStartup)
{
    sOptions_.Update ([=] (Options_ d) { d.fCheckFileAssocAtStartup = checkFileAssocsAtStartup; return d; });
}
#endif

Led_FontSpecification Options::GetDefaultNewDocFont () const
{
#if qPlatform_Windows
    BLOB bytes = sOptions_.Get ().fDefaultNewDocFont;
    if (not bytes.empty ()) {
        if (bytes.size () == sizeof (LOGFONT)) {
            Led_FontSpecification fsp;
            fsp.SetOSRep (bytes.As<LOGFONT> ());
            return fsp;
        }
    }
#endif
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
#if qPlatform_Windows
    sOptions_.Update ([&] (Options_ d) { d.fDefaultNewDocFont = BLOB::Raw (defaultNewDocFont.GetOSRep ()); return d; });
#else
    Led_Arg_Unused (defaultNewDocFont);
#endif
}
