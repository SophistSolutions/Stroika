/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"

#include "LedItDocument.h"
#include "LedItView.h"

#include "LedItControlItem.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Led;
using namespace Stroika::Frameworks::Led::Platform;
using namespace Stroika::Frameworks::Led::StyledTextIO;

/*
 ********************************************************************************
 ******************************** LedItControlItem ******************************
 ********************************************************************************
 */
IMPLEMENT_SERIAL (LedItControlItem, Led_MFC_ControlItem, 0)

LedItControlItem::LedItControlItem (COleDocument* pContainer)
    : Led_MFC_ControlItem (pContainer)
{
}

SimpleEmbeddedObjectStyleMarker* LedItControlItem::mkLedItControlItemStyleMarker (const char* embeddingTag, const void* data, size_t len)
{
    RequireNotNull (DocContextDefiner::GetDoc ()); // See doc in header for class DocContextDefiner.
    // must declare one of these on call stack before calling this
    // method...
    Led_MFC_ControlItem* e = new LedItControlItem (DocContextDefiner::GetDoc ());
    try {
        return mkLed_MFC_ControlItemStyleMarker_ (embeddingTag, data, len, e);
    }
    catch (...) {
        delete e;
        throw;
    }
    Assert (false);
    return NULL; //notreached
}

SimpleEmbeddedObjectStyleMarker* LedItControlItem::mkLedItControlItemStyleMarker (ReaderFlavorPackage& flavorPackage)
{
    RequireNotNull (DocContextDefiner::GetDoc ()); // See doc in header for class DocContextDefiner.
    // must declare one of these on call stack before calling this
    // method...
    Led_MFC_ControlItem* e = new LedItControlItem (DocContextDefiner::GetDoc ());
    try {
        return mkLed_MFC_ControlItemStyleMarker_ (flavorPackage, e);
    }
    catch (...) {
        delete e;
        throw;
    }
    Assert (false);
    return NULL; //notreached
}

BOOL LedItControlItem::CanActivate ()
{
    // Editing in-place while the server itself is being edited in-place
    //  does not work and is not supported.  So, disable in-place
    //  activation in this case.
    LedItDocument& pDoc = GetDocument ();
    if (pDoc.IsInPlaceActive ()) {
        return FALSE;
    }

    // otherwise, rely on default behavior
    return inherited::CanActivate ();
}

#if qDebug
LedItDocument& LedItControlItem::GetDocument () const
{
    LedItDocument* result = (LedItDocument*)COleClientItem::GetDocument ();
    EnsureNotNull (result);
    ASSERT_VALID (result);
    ASSERT (result->IsKindOf (RUNTIME_CLASS (LedItDocument)));
    return *result;
}
#endif
