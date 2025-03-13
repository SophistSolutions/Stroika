/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_Document_EngineProperties_h_
#define _Stroika_Foundation_Database_Document_EngineProperties_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::Database::Document {

    using Characters::String;

    /**
     *  \brief EngineProperties captures the features associated with a given database engine (being talked to through a Document::Connection::Ptr).
     */
    class EngineProperties {
    public:
        /**
         */
        EngineProperties () = default;

    public:
        /**
         */
        virtual ~EngineProperties () = default;

    public:
        /**
         */
        virtual String GetEngineName () const = 0;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "EngineProperties.inl"

#endif /*_Stroika_Foundation_Database_Document_EngineProperties_h_*/
