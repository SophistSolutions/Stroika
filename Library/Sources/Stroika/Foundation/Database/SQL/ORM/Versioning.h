/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_SQL_ORM_Versioning_h_
#define _Stroika_Foundation_Database_SQL_ORM_Versioning_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation//Common/Version.h"
#include "Stroika/Foundation/Database/SQL/Connection.h"
#include "Stroika/Foundation/Database/SQL/ORM/Schema.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 *
 *  TODO:
 *      \todo   Store and pass along actually stored version, and do upgrading.
 * 
 *      \todo   Provide configuration options and ability to EITHER version# tables (table-v) or do
 *              that at the database level (differnt connection) - so must be able to construct the connection
 *              from here). A sensible pattern is what I do for WTF, where I say db-v3, db-v4 etccc each time
 *              I change db version, and have migration code to migrate data from v3 to v4.
 * 
 *              todo this maybe have IN-ConnectionPtr and OUT-Connection-Ptr in TableProvisioner API? or
 *              pair<Connection::Ptr,optional<Version>> from, and same-type to. maybe optional<pair> for the
 *              from for the case of no DB to start with.
 * 
 *              Extend versioninfo so we track actual app code version and format version; (so instead of pair<>
 *              probably struct).
 */

namespace Stroika::Foundation::Database::SQL::ORM {

    using namespace Containers;
    using namespace DataExchange;

    /**
     */
    struct TableProvisioner {
        String                                                                                                          fTableName;
        function<void (SQL::Connection::Ptr, optional<Common::Version> existingVersion, Common::Version targetVersion)> fProvisioner;
    };

    /**
     *  @todo HANDLE THE VERSIONING piece - have hardwired version table, and add support to provisioner to UPGRADE tables
     *  EVENTAULLY even support upgrade by migrating to new DB, but for now in-place is good enuf
     */
    void ProvisionForVersion (SQL::Connection::Ptr conn, Common::Version targetDBVersion, const Traversal::Iterable<Schema::Table>& tables);
    void ProvisionForVersion (SQL::Connection::Ptr conn, Common::Version targetDBVersion, const Traversal::Iterable<TableProvisioner>& tables);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Versioning.inl"

#endif /*_Stroika_Foundation_Database_SQL_ORM_Versioning_h_*/
