/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */

#ifndef __Options_h__
#define __Options_h__ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#if defined(WIN32)
#include <afxwin.h>
#endif

#include "Stroika/Frameworks/Led/GDI.h"
#include "Stroika/Frameworks/Led/Support.h"
#include "Stroika/Frameworks/Led/TextInteractor.h"

#include "LedItConfig.h"

class Options {
public:
    Options ()  = default;
    ~Options () = default;

public:
    using SearchParameters = TextInteractor::SearchParameters;

public:
    nonvirtual SearchParameters GetSearchParameters () const;
    nonvirtual void             SetSearchParameters (const SearchParameters& searchParameters);

#if qPlatform_Windows
public:
    nonvirtual const CDockState& GetDocBarState () const;
    nonvirtual void              SetDocBarState (const CDockState& dockState);
#endif

public:
    nonvirtual bool GetSmartCutAndPaste () const;
    nonvirtual void SetSmartCutAndPaste (bool smartCutAndPaste);

public:
    nonvirtual bool GetWrapToWindow () const;
    nonvirtual void SetWrapToWindow (bool wrapToWindow);

public:
    nonvirtual bool GetShowHiddenText () const;
    nonvirtual void SetShowHiddenText (bool showHiddenText);

public:
    nonvirtual bool GetShowParagraphGlyphs () const;
    nonvirtual void SetShowParagraphGlyphs (bool showParagraphGlyphs);

public:
    nonvirtual bool GetShowTabGlyphs () const;
    nonvirtual void SetShowTabGlyphs (bool showTabGlyphs);

public:
    nonvirtual bool GetShowSpaceGlyphs () const;
    nonvirtual void SetShowSpaceGlyphs (bool showSpaceGlyphs);

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

#endif /*__Options_h__*/
