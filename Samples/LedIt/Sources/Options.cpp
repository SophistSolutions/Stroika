/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"

#if defined(WIN32)
#include <afxadv.h>
#endif

#include "Stroika/Foundation/DataExchange/OptionsFile.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/ModuleGetterSetter.h"
#include "Stroika/Foundation/Memory/BLOB.h"

#include "Options.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Time;

using namespace Stroika::Frameworks::Led;

using Characters::String;
using Memory::BLOB;

using SearchParameters = TextInteractor::SearchParameters;

namespace {

    struct Options_ {
        BLOB             fDockBarState;
        SearchParameters fSearchParameters{};
        bool             fSmartCutAndPaste{true};
        bool             fWrapToWindow{false};
        bool             fShowHiddenText{true};
        bool             fShowParagraphGlyphs{false};
        bool             fShowTabGlyphs{false};
        bool             fShowSpaceGlyphs{false};
#if qPlatform_Windows
        bool fCheckFileAssocAtStartup{true};
        BLOB fDefaultNewDocFont;
#endif
    };

    struct Options_Storage_IMPL_ {
        Options_Storage_IMPL_ ()
            : fOptionsFile_{"AppSettings"sv,
                            [] () -> ObjectVariantMapper {
                                ObjectVariantMapper mapper;

                                // really should use String, no longer Led_tString, but for now... (note this only works as is for wchar_t Led_tString
                                mapper.Add<Led_tString> ([] (const ObjectVariantMapper& /*mapper*/,
                                                             const Led_tString* obj) -> VariantValue { return String{*obj}; },
                                                         [] (const ObjectVariantMapper& /*mapper*/, const VariantValue& d,
                                                             Led_tString* intoObj) -> void { *intoObj = d.As<String> ().As<Led_tString> (); });
                                mapper.AddCommonType<vector<Led_tString>> ();
                                mapper.AddCommonType<Memory::BLOB> ();

                                mapper.AddClass<SearchParameters> ({
                                    {"MatchString"sv, &SearchParameters::fMatchString},
                                    {"WrapSearch"sv, &SearchParameters::fWrapSearch},
                                    {"WholeWordSearch"sv, &SearchParameters::fWholeWordSearch},
                                    {"CaseSensativeSearch"sv, &SearchParameters::fCaseSensativeSearch},
                                    {"RecentMatchStrings"sv, &SearchParameters::fRecentFindStrings},
                                });

                                mapper.AddClass<Options_> ({
                                    {"DockBarState"sv, &Options_::fDockBarState}, {"Search-Parameters"sv, &Options_::fSearchParameters},
                                        {"SmartCutAndPaste"sv, &Options_::fSmartCutAndPaste}, {"WrapToWindow"sv, &Options_::fWrapToWindow},
                                        {"ShowHiddenText"sv, &Options_::fShowHiddenText},
                                        {"ShowParagraphGlyphs"sv, &Options_::fShowParagraphGlyphs},
                                        {"ShowTabGlyphs"sv, &Options_::fShowTabGlyphs}, {"ShowSpaceGlyphs"sv, &Options_::fShowSpaceGlyphs},
#if qPlatform_Windows
                                        {"CheckFileAssocAtStartup"sv, &Options_::fCheckFileAssocAtStartup},
                                        {"DefaultNewDocFont"sv, &Options_::fDefaultNewDocFont},
#endif
                                });
                                return mapper;
                            }(),

                            OptionsFile::kDefaultUpgrader,

                            OptionsFile::mkFilenameMapper ("LedIt"sv)}
            , fActualCurrentConfigData_{fOptionsFile_.Read<Options_> (Options_{})}
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
    sOptions_.Update ([=] (Options_ d) {
        d.fSearchParameters = searchParameters;
        return d;
    });
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
            file.Write (Traversal::Iterator2Pointer (bytes.begin ()), static_cast<UINT> (bytes.size ()));
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
    sOptions_.Update ([=] (Options_ d) {
        d.fDockBarState = BLOB{p, p + nSize};
        return d;
    });
    delete[] p;
}
#endif

bool Options::GetSmartCutAndPaste () const
{
    return sOptions_.Get ().fSmartCutAndPaste;
}

void Options::SetSmartCutAndPaste (bool smartCutAndPaste)
{
    sOptions_.Update ([=] (Options_ d) {
        d.fSmartCutAndPaste = smartCutAndPaste;
        return d;
    });
}

bool Options::GetWrapToWindow () const
{
    return sOptions_.Get ().fWrapToWindow;
}

void Options::SetWrapToWindow (bool wrapToWindow)
{
    sOptions_.Update ([=] (Options_ d) {
        d.fWrapToWindow = wrapToWindow;
        return d;
    });
}

bool Options::GetShowHiddenText () const
{
    return sOptions_.Get ().fShowHiddenText;
}

void Options::SetShowHiddenText (bool showHiddenText)
{
    sOptions_.Update ([=] (Options_ d) {
        d.fShowHiddenText = showHiddenText;
        return d;
    });
}

bool Options::GetShowParagraphGlyphs () const
{
    return sOptions_.Get ().fShowParagraphGlyphs;
}

void Options::SetShowParagraphGlyphs (bool showParagraphGlyphs)
{
    sOptions_.Update ([=] (Options_ d) {
        d.fShowParagraphGlyphs = showParagraphGlyphs;
        return d;
    });
}

bool Options::GetShowTabGlyphs () const
{
    return sOptions_.Get ().fShowTabGlyphs;
}

void Options::SetShowTabGlyphs (bool showTabGlyphs)
{
    sOptions_.Update ([=] (Options_ d) {
        d.fShowTabGlyphs = showTabGlyphs;
        return d;
    });
}

bool Options::GetShowSpaceGlyphs () const
{
    return sOptions_.Get ().fShowSpaceGlyphs;
}

void Options::SetShowSpaceGlyphs (bool showSpaceGlyphs)
{
    sOptions_.Update ([=] (Options_ d) {
        d.fShowSpaceGlyphs = showSpaceGlyphs;
        return d;
    });
}

#if qPlatform_Windows
bool Options::GetCheckFileAssocsAtStartup () const
{
    return sOptions_.Get ().fCheckFileAssocAtStartup;
}

void Options::SetCheckFileAssocsAtStartup (bool checkFileAssocsAtStartup)
{
    sOptions_.Update ([=] (Options_ d) {
        d.fCheckFileAssocAtStartup = checkFileAssocsAtStartup;
        return d;
    });
}
#endif

FontSpecification Options::GetDefaultNewDocFont () const
{
#if qPlatform_Windows
    BLOB bytes = sOptions_.Get ().fDefaultNewDocFont;
    if (not bytes.empty ()) {
        if (bytes.size () == sizeof (LOGFONT)) {
            FontSpecification fsp;
            fsp.SetOSRep (bytes.As<LOGFONT> ());
            return fsp;
        }
    }
#endif
    return GetStaticDefaultFont ();
}

void Options::SetDefaultNewDocFont ([[maybe_unused]] const FontSpecification& defaultNewDocFont)
{
#if qPlatform_Windows
    sOptions_.Update ([&] (Options_ d) {
        d.fDefaultNewDocFont = BLOB::FromRaw (defaultNewDocFont.GetOSRep ());
        return d;
    });
#endif
}
