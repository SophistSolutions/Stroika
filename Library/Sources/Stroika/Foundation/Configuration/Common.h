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


            /**
             *  \def nonvirtual
             *      Is for documentation purposes, to make clear a method is intended to be not 'virtual'
             */
#define nonvirtual



            /**
             *  @todo - maybe move this and other helpers into 'enumeration' module
             *          maybe stuff like Add(ENUM, ENUM), and DIFF (ENum,ENUM) to workouarnd
             *          issues with too-strong typing with enum class?? (avoid so many casts)
             *
             *      Maybe move this (and that future module) to memory).
             *      Maybe move definestartendcount stuff to memory/Enumerations module?
             */
            template    <typename   ENUM>
            ENUM    Inc (ENUM e);


            /**
             *  \def Define_Start_End_Count
             *      Define meta information on enums using standardized names, so you can generically
             *      write things like Can do for (i = eSTART; i != eEND; i = Inc (i));
             */
#define Define_Start_End_Count(FIRST_ITEM,LAST_ITEM)\
    eSTART      =   FIRST_ITEM,\
                    eEND        =   LAST_ITEM + 1,\
                                    eCOUNT      =   eEND - eSTART,


        }
    }
}


#endif  /*_Stroika_Foundation_Configuration_Common_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Common.inl"


