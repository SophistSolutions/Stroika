/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_StyledTextIO_MIMETextEnriched_h_
#define _Stroika_Frameworks_Led_StyledTextIO_MIMETextEnriched_h_ 1

/*
@MODULE:    StyledTextIO_MIMETextEnriched
@DESCRIPTION:
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Frameworks/Led/StyledTextIO/StyledTextIO.h"

namespace Stroika::Frameworks::Led::StyledTextIO {

    /*
    @CLASS:         StyledTextIOReader_MIMETextEnriched
    @BASES:         @'StyledTextIOReader'
    @DESCRIPTION:
    */
    class StyledTextIOReader_MIMETextEnriched : public StyledTextIOReader {
    public:
        StyledTextIOReader_MIMETextEnriched (SrcStream* srcStream, SinkStream* sinkStream);

    public:
        virtual void Read () override;
        virtual bool QuickLookAppearsToBeRightFormat () override;

    private:
        nonvirtual void SkipWhitespace ();
        nonvirtual void SkipOneLine ();
        nonvirtual bool ScanFor (const char* matchMe, bool ignoreCase = true);
        nonvirtual bool LookingAt (const char* matchMe, bool ignoreCase = true);

    private:
        nonvirtual FontSpecification GetAdjustedCurrentFontSpec () const;

        int fBoldMode;       // non-zero means YES - can be nested
        int fItalicMode;     // ""
        int fUnderlineMode;  // ""
        int fFixedWidthMode; // ""
        int fFontSizeAdjust; // ""
        int fNoFillMode;     // ""
    };

    /*
     ********************************************************************************
     ***************************** Implementation Details ***************************
     ********************************************************************************
     */

}

#endif /*_Stroika_Frameworks_Led_StyledTextIO_MIMETextEnriched_h_*/
