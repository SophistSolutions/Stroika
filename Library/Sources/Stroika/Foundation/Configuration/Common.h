/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Common_h_
#define _Stroika_Foundation_Configuration_Common_h_ 1

#include    <cstddef>
#include    <cstdint>
#include    <ciso646>


namespace   Stroika {
    namespace   Foundation {

        using   namespace   std;

        namespace   Configuration {

            typedef unsigned char   Byte;

#define nonvirtual

#define NEltsOf(X)      (sizeof((X))/sizeof((X)[0]))
#define StartOfArray(X) (&(X)[0])
#define EndOfArray(X)   (&(X)[NEltsOf(X)])

            template    <typename   ENUM>
            ENUM    Inc (ENUM e);


// Can do for (i = eSTART; i != eEND; i = Inc (i));
#define Define_Start_End_Count(FIRST_ITEM,LAST_ITEM)\
    eSTART      =   FIRST_ITEM,\
                    eEND        =   LAST_ITEM + 1,\
                                    eCOUNT      =   eEND - eSTART,




        }

        using   Configuration::Byte;
    }
}


#endif  /*_Stroika_Foundation_Configuration_Common_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Common.inl"


