/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_StyledTextIO_STYLText_h_
#define _Stroika_Frameworks_Led_StyledTextIO_STYLText_h_ 1



/*
@MODULE:    StyledTextIO_STYLText
@DESCRIPTION:
        <p>Support MacOS resource based STYL text.</p>
 */


#include    "StyledTextIO.h"







#if     qMacOS
/*
@CLASS:         StyledTextIOReader_STYLText
@BASES:         @'StyledTextIOReader'
@DESCRIPTION:   <p><b>MacOS ONLY</b></p>
*/
class   StyledTextIOReader_STYLText : public StyledTextIOReader {
public:
    StyledTextIOReader_STYLText (SrcStream* srcStream, SinkStream* sinkStream);

public:
    override    void    Read ();
    override    bool    QuickLookAppearsToBeRightFormat ();
};
#endif







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */



#endif  /*_Stroika_Frameworks_Led_StyledTextIO_STYLText_h_*/
