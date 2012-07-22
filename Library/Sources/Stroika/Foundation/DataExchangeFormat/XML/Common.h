/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchangeFormat_XML_Common_h_
#define _Stroika_Foundation_DataExchangeFormat_XML_Common_h_    1

#include    "../../StroikaPreComp.h"


////TODO:
///         o   Add Reader/Writer API like with JSON - using tree of Variants! - this produces essentially the same thing as a DOM reader/writer, but you can
//              go back and forth with JSON or XML this way...




/*
@CONFIGVAR:     qHasLibrary_Xerces
@DESCRIPTION:   <p>Stroika currently depends on Xerces to provide SAX-reader services./p>
*/
#ifndef qHasLibrary_Xerces
#error "qHasLibrary_Xerces should normally be defined indirectly by StroikaConfig.h"
#endif



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchangeFormat {
            namespace   XML {

            }
        }
    }
}
#endif  /*_Stroika_Foundation_DataExchangeFormat_XML_Common_h_*/

