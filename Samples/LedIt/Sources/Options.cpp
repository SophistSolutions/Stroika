/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
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
            : fOptionsFile_{
                  L"AppSettings"sv,
                  []() -> ObjectVariantMapper {
                      ObjectVariantMapper mapper;

                      // really should use String, no longer Led_tString, but for now... (note this only works as is for wchar_t Led_tString
                      mapper.Add<Led_tString> (
                          [](const ObjectVariantMapper& /*mapper*/, const Led_tString* obj) -> VariantValue {
                              return String{*obj};
                          },
                          [](const ObjectVariantMapper& /*mapper*/, const VariantValue& d, Led_tString* intoObj) -> void {
                              *intoObj = d.As<String> ().As<Led_tString> ();
                          });
                      mapper.AddCommonType<vector<Led_tString>> ();
                      mapper.AddCommonType<Memory::BLOB> ();

                      mapper.AddClass<SearchParameters> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
                          {L"MatchString", Stroika_Foundation_DataExchange_StructFieldMetaInfo (SearchParameters, fMatchString)},
                          {L"WrapSearch", Stroika_Foundation_DataExchange_StructFieldMetaInfo (SearchParameters, fWrapSearch)},
                          {L"WholeWordSearch", Stroika_Foundation_DataExchange_StructFieldMetaInfo (SearchParameters, fWholeWordSearch)},
                          {L"CaseSensativeSearch", Stroika_Foundation_DataExchange_StructFieldMetaInfo (SearchParameters, fCaseSensativeSearch)},
                          {L"RecentMatchStrings", Stroika_Foundation_DataExchange_StructFieldMetaInfo (SearchParameters, fRecentFindStrings)},
                      });

                      mapper.AddClass<Options_> (initializer_list<ObjectVariantMapper::StructFieldInfo> {
                          {L"DockBarState", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Options_, fDockBarState)},
                              {L"Search-Parameters", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Options_, fSearchParameters)},
                              {L"SmartCutAndPaste", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Options_, fSmartCutAndPaste)},
                              {L"WrapToWindow", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Options_, fWrapToWindow)},
                              {L"ShowHiddenText", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Options_, fShowHiddenText)},
                              {L"ShowParagraphGlyphs", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Options_, fShowParagraphGlyphs)},
                              {L"ShowTabGlyphs", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Options_, fShowTabGlyphs)},
                              {L"ShowSpaceGlyphs", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Options_, fShowSpaceGlyphs)},
#if qPlatform_Windows
                              {L"CheckFileAssocAtStartup", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Options_, fCheckFileAssocAtStartup)},
                              {L"DefaultNewDocFont", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Options_, fDefaultNewDocFont)},
#endif
                      });
                      return mapper;
                  }(),

                  OptionsFile::kDefaultUpgrader,

                  OptionsFile::mkFilenameMapper (L"LedIt")}
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
    sOptions_.Update ([=](Options_ d) { d.fSearchParameters = searchParameters; return d; });
}

#if qPlatform_Windows
const CDockState& Options::GetDocBarState () const
{
    static CDockState     dockState; // keep static copy and clear each time cuz CDocState doesn't support copy CTOR - LGP971214
    static std::once_flag sOnce_;
    std::call_once (sOnce_, []() {
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
    sOptions_.Update ([=](Options_ d) { d.fDockBarState = BLOB{p, p+nSize}; return d; });
    delete[] p;
}
#endif

bool Options::GetSmartCutAndPaste () const
{
    return sOptions_.Get ().fSmartCutAndPaste;
}

void Options::SetSmartCutAndPaste (bool smartCutAndPaste)
{
    sOptions_.Update ([=](Options_ d) { d.fSmartCutAndPaste = smartCutAndPaste; return d; });
}

bool Options::GetWrapToWindow () const
{
    return sOptions_.Get ().fWrapToWindow;
}

void Options::SetWrapToWindow (bool wrapToWindow)
{
    sOptions_.Update ([=](Options_ d) { d.fWrapToWindow = wrapToWindow; return d; });
}

bool Options::GetShowHiddenText () const
{
    return sOptions_.Get ().fShowHiddenText;
}

void Options::SetShowHiddenText (bool showHiddenText)
{
    sOptions_.Update ([=](Options_ d) { d.fShowHiddenText = showHiddenText; return d; });
}

bool Options::GetShowParagraphGlyphs () const
{
    return sOptions_.Get ().fShowParagraphGlyphs;
}

void Options::SetShowParagraphGlyphs (bool showParagraphGlyphs)
{
    sOptions_.Update ([=](Options_ d) { d.fShowParagraphGlyphs = showParagraphGlyphs; return d; });
}

bool Options::GetShowTabGlyphs () const
{
    return sOptions_.Get ().fShowTabGlyphs;
}

void Options::SetShowTabGlyphs (bool showTabGlyphs)
{
    sOptions_.Update ([=](Options_ d) { d.fShowTabGlyphs = showTabGlyphs; return d; });
}

bool Options::GetShowSpaceGlyphs () const
{
    return sOptions_.Get ().fShowSpaceGlyphs;
}

void Options::SetShowSpaceGlyphs (bool showSpaceGlyphs)
{
    sOptions_.Update ([=](Options_ d) { d.fShowSpaceGlyphs = showSpaceGlyphs; return d; });
}

#if qPlatform_Windows
bool Options::GetCheckFileAssocsAtStartup () const
{
    return sOptions_.Get ().fCheckFileAssocAtStartup;
}

void Options::SetCheckFileAssocsAtStartup (bool checkFileAssocsAtStartup)
{
    sOptions_.Update ([=](Options_ d) { d.fCheckFileAssocAtStartup = checkFileAssocsAtStartup; return d; });
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
    return TextImager::GetStaticDefaultFont ();
}

void Options::SetDefaultNewDocFont (const Led_FontSpecification& defaultNewDocFont)
{
#if qPlatform_Windows
    sOptions_.Update ([&](Options_ d) { d.fDefaultNewDocFont = BLOB::Raw (defaultNewDocFont.GetOSRep ()); return d; });
#else
    Led_Arg_Unused (defaultNewDocFont);
#endif
}
