/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../Statement.h"

#include "Versioning.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Database;
using namespace Stroika::Foundation::Database::SQL;
using namespace Stroika::Foundation::Debug;

// Comment this in to turn on aggressive noisy DbgTrace in this module
// #define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

/*
 ********************************************************************************
 *************************** ORM::ProvisionForVersion ***************************
 ********************************************************************************
 */
void ORM::ProvisionForVersion (SQL::Connection::Ptr conn, Configuration::Version targetDBVersion, const Traversal::Iterable<Schema::Table>& tables)
{
    Collection<TableProvisioner> provisioners;
    for (auto ti : tables) {
        provisioners += TableProvisioner{ti.fName, [=] (SQL::Connection::Ptr conn, optional<Configuration::Version> existingVersion, [[maybe_unused]] Configuration::Version targetDBVersion) -> void {
                                             // properly upgrade - for now just create if doesn't exist
                                             if (!existingVersion) {
                                                 conn.Exec (Schema::StandardSQLStatements{ti}.CreateTable ());
                                             }
                                         }};
    }
    ProvisionForVersion (conn, targetDBVersion, provisioners);
}
void ORM::ProvisionForVersion (SQL::Connection::Ptr conn, Configuration::Version targetDBVersion, const Traversal::Iterable<TableProvisioner>& tables)
{
    TraceContextBumper ctx{L"ORM::ProvisionForVersion", Stroika_Foundation_Debug_OptionalizeTraceArgs (L"conn=%s", Characters::ToString (conn).c_str ())};
    SQL::Statement     doesTableExist = conn.mkStatement (conn.GetEngineProperties ()->GetSQL (SQL::EngineProperties::NonStandardSQL::eDoesTableExist));
    for (auto ti : tables) {
        doesTableExist.Reset ();
        doesTableExist.Bind (SQL::EngineProperties::kDoesTableExistParameterName, ti.fTableName);
        if (not doesTableExist.GetNextRow ()) {
            ti.fProvisioner (conn, nullopt, targetDBVersion);
        }
        else {
            // @todo store / pass along version
            ti.fProvisioner (conn, Configuration::Version{1, 0, Configuration::VersionStage::Alpha, 0}, targetDBVersion);
        }
    }
}