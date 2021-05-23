/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Samples_SQLite_ComputerNetwork_h_
#define _Samples_SQLite_ComputerNetwork_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Database/SQLite.h"

namespace Stroika::Samples::SQLite {
#if qHasFeature_sqlite
    using Stroika::Foundation::Database::SQLite::Options;
    /**
     *  \brief  Simple test of using threads with SQLite - one thread updates employees table, and one writes paychecks
     */
    void ComputerNetworksModel (const Options& options);
#endif
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Samples_SQLite_ComputerNetwork_h_*/
