/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_ComputerName_h_
#define _Stroika_Foundation_Configuration_ComputerName_h_  1

#include    "../StroikaPreComp.h"

#include    <locale>
#include    <vector>

#include    "../Characters/String.h"
#include    "Common.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 * TODO:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {


            using   Characters::String;


            /**
             *  Returns the best OS dependent guess at a computer name we have.
             *
             *  On windows, this amounts to the NETBIOS name, and on UNIX, this amounts
             *  to the result of 'gethostname' (man 2 hostname).
             *
             *  This is very frequently NOT useful, not unique, but frequently desired/used,
             *  so at least you can get to it uniformly, cross-platform.
             */
            String  GetComputerName ();


        }
    }
}






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ComputerName.inl"

#endif  /*_Stroika_Foundation_Configuration_ComputerName_h_*/
