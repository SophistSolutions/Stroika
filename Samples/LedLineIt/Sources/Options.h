/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef __Options_h__
#define __Options_h__ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <afxwin.h>

#include "Stroika/Frameworks/Led/GDI.h"
#include "Stroika/Frameworks/Led/Support.h"
#include "Stroika/Frameworks/Led/TextInteractor.h"

#include "LedLineItConfig.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Led;

class Options {
public:
    Options ()  = default;
    ~Options () = default;

public:
    nonvirtual TextInteractor::SearchParameters GetSearchParameters () const;
    nonvirtual void                             SetSearchParameters (const TextInteractor::SearchParameters& searchParameters);

public:
    nonvirtual const CDockState& GetDocBarState () const;
    nonvirtual void              SetDocBarState (const CDockState& dockState);

public:
    nonvirtual bool GetSmartCutAndPaste () const;
    nonvirtual void SetSmartCutAndPaste (bool smartCutAndPaste);

public:
    nonvirtual bool GetAutoIndent () const;
    nonvirtual void SetAutoIndent (bool autoIndent);

public:
    nonvirtual bool GetTreatTabAsIndentChar () const;
    nonvirtual void SetTreatTabAsIndentChar (bool tabAsIndentChar);

#if qSupportSyntaxColoring
public:
    enum SyntaxColoringOption { eSyntaxColoringNone      = 1,
                                eSyntaxColoringCPlusPlus = 2,
                                eSyntaxColoringVB        = 3,

                                Stroika_Define_Enum_Bounds (eSyntaxColoringNone, eSyntaxColoringVB)

    };
    nonvirtual SyntaxColoringOption GetSyntaxColoringOption () const;
    nonvirtual void                 SetSyntaxColoringOption (SyntaxColoringOption syntaxColoringOption);
#endif

#if qPlatform_Windows
public:
    nonvirtual bool GetCheckFileAssocsAtStartup () const;
    nonvirtual void SetCheckFileAssocsAtStartup (bool checkFileAssocsAtStartup);
#endif

public:
    nonvirtual Led_FontSpecification GetDefaultNewDocFont () const;
    nonvirtual void                  SetDefaultNewDocFont (const Led_FontSpecification& defaultNewDocFont);
};

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

//NB CANNOT CHANGE VALUES LIGHTLY - WRITTEN TO config files
namespace Stroika::Foundation::Configuration {
#if !qCompilerAndStdLib_template_specialization_internalErrorWithSpecializationSignifier_Buggy
    template <>
#endif
    constexpr EnumNames<Options::SyntaxColoringOption> DefaultNames<Options::SyntaxColoringOption>::k{
        Configuration::EnumNames<Options::SyntaxColoringOption>::BasicArrayInitializer{{
            {Options::SyntaxColoringOption::eSyntaxColoringNone, L"None"},
            {Options::SyntaxColoringOption::eSyntaxColoringCPlusPlus, L"C++"},
            {Options::SyntaxColoringOption::eSyntaxColoringVB, L"Visual-Basic"},
        }}};
}

#endif /*__Options_h__*/
