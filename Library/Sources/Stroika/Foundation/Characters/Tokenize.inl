/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_Tokenize_inl_
#define _Stroika_Foundation_Characters_Tokenize_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Containers/Common.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


            template    <typename STRING>
            vector<STRING> Tokenize (const STRING& str, const STRING& delimiters)
            {
                vector<STRING>  result;
                typename STRING::size_type  lastPos = str.find_first_not_of (delimiters, 0);        // Skip delimiters at beginning
                typename STRING::size_type  pos     = str.find_first_of (delimiters, lastPos);      // Find first "non-delimiter"
                while (STRING::npos != pos || STRING::npos != lastPos) {
                    Containers::ReserveSpeedTweekAdd1 (result);
                    // Found a token, add it to the vector.
                    result.push_back(str.substr (lastPos, pos - lastPos));
                    // Skip delimiters.  Note the "not_of"
                    lastPos = str.find_first_not_of (delimiters, pos);
                    // Find next "non-delimiter"
                    pos = str.find_first_of (delimiters, lastPos);
                }
                return result;
            }

            template    <>
            vector<String> Tokenize (const String& str, const String& delimiters);

        }
    }
}
#endif  /*_Stroika_Foundation_Characters_Tokenize_inl_*/
