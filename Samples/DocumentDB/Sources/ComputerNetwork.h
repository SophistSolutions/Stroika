/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Samples_Document_ComputerNetwork_h_
#define _Samples_Document_ComputerNetwork_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <functional>

#include "Stroika/Foundation/Database/Document/Connection.h"

namespace Stroika::Samples::Document {

    using namespace Stroika::Foundation::Database::Document;

    /**
     *  \brief  Simple test of using threads with SQL - one thread updates employees table, and one writes paychecks
     */
    void ComputerNetworksModel (const function<Connection::Ptr ()>& connectionFactory);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Samples_Document_ComputerNetwork_h_*/
