/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_Document_Projection_h_
#define _Stroika_Foundation_Database_Document_Projection_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
//#include "Stroika/Foundation/Common/Property.h"
//#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Containers/Set.h"
//#include "Stroika/Foundation/DataExchange/VariantValue.h"
//#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"

/**
 *  \file
 * 
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::Database::Document {

    /*
    using Containers::Sequence;
  
    using DataExchange::VariantValue;
    ;*/
    using Characters::String;
    using Containers::Set;
    using Traversal::Iterable;

    /**
       */
    struct Projection {

        optional<Set<String>> onlyTheseFields;

        optional<Set<String>> omitTheseFields;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Projection.inl"

#endif /*_Stroika_Foundation_Database_Document_Collection_h_*/
