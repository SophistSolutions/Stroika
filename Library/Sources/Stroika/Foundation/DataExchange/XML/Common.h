/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Common_h_
#define _Stroika_Foundation_DataExchange_XML_Common_h_ 1

#include "../../StroikaPreComp.h"

////TODO:
///         o   Add Reader/Writer API like with JSON - using tree of Variants! - this produces essentially the same thing as a DOM reader/writer, but you can
//              go back and forth with JSON or XML this way...

/*
@CONFIGVAR:     qHasFeature_Xerces
@DESCRIPTION:   <p>Stroika currently depends on Xerces to provide SAX-reader services./p>
*/
#ifndef qHasFeature_Xerces
#error "qHasFeature_Xerces should normally be defined indirectly by StroikaConfig.h"
#endif

namespace Stroika {
    namespace Foundation {
        namespace DataExchange {
            namespace XML {
            }
        }
    }
}
#endif /*_Stroika_Foundation_DataExchange_XML_Common_h_*/
