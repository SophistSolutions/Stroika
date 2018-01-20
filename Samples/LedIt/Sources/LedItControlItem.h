/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

#ifndef __LedItControlItem_h__
#define __LedItControlItem_h__ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Frameworks/Led/Platform/MFC_WordProcessor.h"

#include "LedItConfig.h"

class LedItDocument;
class LedItView;

class LedItControlItem : public Platform::Led_MFC_ControlItem {
public:
    LedItControlItem (COleDocument* pContainer = NULL);

public:
    // NB: See Led_MFC_ControlItem docs about DocContextDefiner
    static SimpleEmbeddedObjectStyleMarker* mkLedItControlItemStyleMarker (const char* embeddingTag, const void* data, size_t len);
    static SimpleEmbeddedObjectStyleMarker* mkLedItControlItemStyleMarker (ReaderFlavorPackage& flavorPackage);

protected:
    using inherited = Led_MFC_ControlItem;

public:
    virtual BOOL CanActivate () override;

public:
    nonvirtual LedItDocument& GetDocument () const;

private:
    DECLARE_SERIAL (LedItControlItem)
};

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#if !qDebug
inline LedItDocument& LedItControlItem::GetDocument () const
{
    // See debug version for ensures...
    return *(LedItDocument*)COleClientItem::GetDocument ();
}
#endif

#endif /*__LedItControlItem_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
