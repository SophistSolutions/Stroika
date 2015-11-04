/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReader_h_
#define _Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReader_h_   1

#include    "../../StroikaPreComp.h"

#include    <memory>

#include    "../../Memory/Optional.h"
#include    "../../Time/DateTime.h"

//tmphac
#include    "../XML/SAXReader.h"


////// @todo I THINK LOSE THIS FILE

/**
 *  \file
 *
 *              \\\ MIDDLE OF MASSIVE REFACTORING - SO DONT TOUCH FOR A WHILE TIL STABLE....
 *              \\\ AND MANY COMMENTS MAYBE WRONG/OUTDATED
 *  TODO:
 *      @todo   This was hard to use. See if I can find example of where I used it, and either cleanup,
 *              or maybe get rid of this - OBSOLETE - and see if easier using new ObjectVairantMapper stuff.
 *
 *              BUT - note - this had the advnatage of using no intermediate format!!! So see about
 *              capturing its adnvanteas?? Maybe re-use some stuff with new metadata?
 *
 *              Note - there is an EXAMPLE of use of this in the regression tests. Use that to evaluate.
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            namespace   StructuredStreamEvents {


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ObjectReader.inl"

#endif  /*_Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReader_h_*/
