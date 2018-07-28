/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_StyledTextIO_PlainText_h_
#define _Stroika_Frameworks_Led_StyledTextIO_PlainText_h_ 1

#include "../../../Foundation/StroikaPreComp.h"

/*
@MODULE:    StyledTextIO_PlainText
@DESCRIPTION:
        <p>Support reading/writing plain text to/from word-processing Src/Sink streams.</p>
 */

#include "StyledTextIO.h"

namespace Stroika::Frameworks::Led::StyledTextIO {

    /*
    @CLASS:         StyledTextIOReader_PlainText
    @BASES:         @'StyledTextIOReader'
    @DESCRIPTION:
    */
    class StyledTextIOReader_PlainText : public StyledTextIOReader {
    public:
        StyledTextIOReader_PlainText (SrcStream* srcStream, SinkStream* sinkStream);

    public:
        virtual void Read () override;
        virtual bool QuickLookAppearsToBeRightFormat () override;
    };

    /*
    @CLASS:         StyledTextIOWriter_PlainText
    @BASES:         @'StyledTextIOWriter'
    @DESCRIPTION:
    */
    class StyledTextIOWriter_PlainText : public StyledTextIOWriter {
    public:
        StyledTextIOWriter_PlainText (SrcStream* srcStream, SinkStream* sinkStream);

    public:
        virtual void Write () override;
    };

    /*
     ********************************************************************************
     ***************************** Implementation Details ***************************
     ********************************************************************************
     */

}

#endif /*_Stroika_Frameworks_Led_StyledTextIO_PlainText_h_*/
