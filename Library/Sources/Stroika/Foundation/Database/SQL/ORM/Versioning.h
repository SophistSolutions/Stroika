/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_SQL_ORM_Versioning_h_
#define _Stroika_Foundation_Database_SQL_ORM_Versioning_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Configuration/Version.h"
#include "../Connection.h"
#include "Schema.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Early</a>
 *
 *  TODO:
 *      \todo   Store and pass along actually stored version, and do upgrading.
 */

namespace Stroika::Foundation::Database::SQL::ORM {

    using namespace Containers;
    using namespace DataExchange;

    /**
     */
    struct TableProvisioner {
        String                                                                                                                        fTableName;
        function<void (SQL::Connection::Ptr, optional<Configuration::Version> existingVersion, Configuration::Version targetVersion)> fProvisioner;
    };

    /**
     *  @todo HANDLE THE VERSIONING peice - have hardwired version table, and add support to provisioner to UPGRADE tables
     *  EVENTAULLY even support upgrade by migrating to new DB, but for now in-place is good enuf
     */
    void ProvisionForVersion (SQL::Connection::Ptr conn, Configuration::Version targetDBVersion, const Traversal::Iterable<Schema::Table>& tables);
    void ProvisionForVersion (SQL::Connection::Ptr conn, Configuration::Version targetDBVersion, const Traversal::Iterable<TableProvisioner>& tables);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Versioning.inl"

#endif /*_Stroika_Foundation_Database_SQL_ORM_Versioning_h_*/
