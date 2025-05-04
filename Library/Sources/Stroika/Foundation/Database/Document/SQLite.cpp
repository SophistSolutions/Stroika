/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <system_error>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Reader.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Writer.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Time/Duration.h"

#include "SQLite.h"

using namespace Stroika::Foundation;

using namespace Characters;
using namespace Containers;
using namespace Debug;
using namespace DataExchange;
using namespace Database;
using namespace Database::Document::SQLite;
using namespace Execution;
using namespace Time;

using Database::Document::EngineProperties;
using Database::Document::Filter;
using Database::Document::IDType;
using Database::Document::Projection;

using Database::Document::kID;
using Database::Document::kOnlyIDs;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

using Database::Document::EngineProperties;

#if qStroika_HasComponent_sqlite
namespace {
    struct ModuleShutdown_ {
        ~ModuleShutdown_ ()
        {
            Verify (::sqlite3_shutdown () == SQLITE_OK); // mostly pointless but avoids memory leak complaints
        }
    } sModuleShutdown_;
    [[noreturn]] void ThrowSQLiteError_ (int errCode, sqlite3* sqliteConnection)
    {
        Require (errCode != SQLITE_OK);
        optional<String> errMsgDetails;
        if (sqliteConnection != nullptr) {
            errMsgDetails = String::FromUTF8 (::sqlite3_errmsg (sqliteConnection));
        }
        switch (errCode) {
            case SQLITE_BUSY: {
                DbgTrace ("SQLITE_BUSY"_f); //  The database file is locked
                Throw (system_error{make_error_code (errc::device_or_resource_busy)});
            } break;
            case SQLITE_LOCKED: {
                DbgTrace ("SQLITE_LOCKED"_f); //  A table in the database is locked
                Throw (system_error{make_error_code (errc::device_or_resource_busy)});
            } break;
            case SQLITE_CONSTRAINT: {
                if (errMsgDetails) {
                    Throw (Exception{"SQLITE_CONSTRAINT: {}"_f(errMsgDetails)});
                }
                else {
                    static const auto kEx_ = Exception{"SQLITE_CONSTRAINT"sv};
                    Throw (kEx_);
                }
            } break;
            case SQLITE_TOOBIG: {
                static const auto kEx_ = Exception{"SQLITE_TOOBIG"sv};
                Throw (kEx_);
            } break;
            case SQLITE_FULL: {
                DbgTrace ("SQLITE_FULL"_f);
                Throw (system_error{make_error_code (errc::no_space_on_device)});
            } break;
            case SQLITE_READONLY: {
                static const auto kEx_ = Exception{"SQLITE_READONLY"sv};
                Throw (kEx_);
            } break;
            case SQLITE_MISUSE: {
                if (errMsgDetails) {
                    Throw (Exception{"SQLITE_MISUSE: {}"_f(errMsgDetails)});
                }
                else {
                    static const auto kEx_ = Exception{"SQLITE_MISUSE"sv};
                    Throw (kEx_);
                }
            } break;
            case SQLITE_ERROR: {
                if (errMsgDetails) {
                    Throw (Exception{"SQLITE_ERROR: {}"_f(errMsgDetails)});
                }
                else {
                    static const auto kEx_ = Exception{"SQLITE_ERROR"sv};
                    Throw (kEx_);
                }
            } break;
            case SQLITE_NOMEM: {
                DbgTrace ("SQLITE_NOMEM translated to bad_alloc"_f);
                Throw (bad_alloc{});
            } break;
        }
        if (errMsgDetails) {
            Throw (Exception{"SQLite Error: {} (code {})"_f(errMsgDetails, errCode)});
        }
        else {
            Throw (Exception{"SQLite Error: {}"_f(errCode)});
        }
    }
    void ThrowSQLiteErrorIfNotOK_ (int errCode, sqlite3* sqliteConnection)
    {
        static_assert (SQLITE_OK == 0);
        if (errCode != SQLITE_OK) [[unlikely]] {
            ThrowSQLiteError_ (errCode, sqliteConnection);
        }
    }

    /*
     *  Simple utility to be able to use lambdas with arbitrary captures more easily with sqlite c API
     */
    template <invocable<int, char**, char**> CB>
    struct SQLiteCallback_ {
        CB fCallback_;

        using STATIC_FUNCTION_TYPE = int (*) (void*, int, char**, char**);

        SQLiteCallback_ (CB&& cb)
            : fCallback_{forward<CB> (cb)}
        {
        }
        STATIC_FUNCTION_TYPE GetStaticFunction ()
        {
            return STATICFUNC_;
        }
        void* GetData ()
        {
            return this;
        }

    private:
        static int STATICFUNC_ (void* SQLiteCallbackData, int argc, char** argv, char** azColName)
        {
            SQLiteCallback_* sqc = reinterpret_cast<SQLiteCallback_*> (SQLiteCallbackData);
            return sqc->fCallback_ (argc, argv, azColName);
        }
    };

    // not reference counted; caller responsibility to destroy before db connection
    struct MyPreparedStatement_ {
        MyPreparedStatement_ () = default;
        MyPreparedStatement_ (sqlite3* db, const String& statement)
        {
            RequireNotNull (db);
            const char* pzTail = nullptr;
            string utfStatement = statement.AsUTF8<string> (); // subtle - need explicit named temporary (in debug builds) so we can check assertion after - which points inside utfStatement
            ThrowSQLiteErrorIfNotOK_ (::sqlite3_prepare_v2 (db, utfStatement.c_str (), -1, &fObj_, &pzTail), db);
            Assert (pzTail != nullptr);
            Require (*pzTail == '\0'); // else argument string had cruft at the end or was a compound statement, not allowed by sqlite and this api/mechanism
            EnsureNotNull (fObj_);
        }
        MyPreparedStatement_ (const MyPreparedStatement_&)     = delete;
        MyPreparedStatement_ (MyPreparedStatement_&&) noexcept = default;
        ~MyPreparedStatement_ ()
        {
            if (fObj_ != nullptr) {
                (void)::sqlite3_finalize (fObj_);    // ignore result - errors indicate error on last evaluation of prepared statement, not on deletion of it
            }
        }
        MyPreparedStatement_& operator= (const MyPreparedStatement_&) = delete;
        MyPreparedStatement_& operator= (MyPreparedStatement_&& rhs) noexcept
        {
            fObj_     = rhs.fObj_;
            rhs.fObj_ = nullptr;
            return *this;
        };
        operator sqlite3_stmt* () const
        {
            return fObj_;
        }

    private:
        sqlite3_stmt* fObj_{nullptr};
    };
    static_assert (movable<MyPreparedStatement_>);
    static_assert (not copyable<MyPreparedStatement_>);
}

/*
 ********************************************************************************
 *************************** SQLite::CompiledOptions ****************************
 ********************************************************************************
 */
namespace {
    struct VerifyFlags_ {
        VerifyFlags_ ()
        {
            Assert (CompiledOptions::kThe.ENABLE_NORMALIZE == !!::sqlite3_compileoption_used ("ENABLE_NORMALIZE"));
            Assert (CompiledOptions::kThe.THREADSAFE == !!::sqlite3_compileoption_used ("THREADSAFE"));
#if SQLITE_VERSION_NUMBER < 3038000
            Assert (::sqlite3_compileoption_used ("ENABLE_JSON1"));
#else
            Assert (!::sqlite3_compileoption_used ("OMIT_JSON1"));
#endif
        }
    } sVerifyFlags_;
}

namespace {
    /**
     * Break the given Stroika filter into parts that can be remoted to sqldb, and parts that must be handled locally
     * 
   
     */
    tuple<optional<String>, optional<Filter>> Partition_ (const optional<Filter>& filter)
    {
        // @todo
        //  return make_tuple (nullopt, filter);
        if (filter) {
            /*
             *  For now just look for FIELD EQUALS VALUE expressions in the top level conjunction. These can be done
             *  server or client side transparently, and moving them server side is more efficient.
             *
             *  Much more could be done, but this is a good cost/benefit start.
             */
            Sequence<Document::FilterElements::Operation> clientSideOps;
            optional<String>                              idWhereTest;
            Set<String>                                   jsonWhereTests;
            for (Document::FilterElements::Operation op : filter->GetConjunctionOperations ()) {
                bool transferred = false;
                if (const Document::FilterElements::Equals* eqOp = get_if<Document::FilterElements::Equals> (&op)) {
                    //String useFieldName = eqOp->fLHS == Database::Document::kID ? kMongoID_ : eqOp->fLHS;
                    if (const Document::FilterElements::Value* rhsValue = get_if<Document::FilterElements::Value> (&eqOp->fRHS)) {
                        // move to server side
                        if (eqOp->fLHS == Database::Document::kID) {
                            idWhereTest = "{} == '{}'"_f(Database::Document::kID, rhsValue->As<String> ()); // not sure this is right way to compare?
                        }
                        else {
                            // others compared in json part
                            jsonWhereTests += "$.{} == '{}'"_f(String{eqOp->fLHS}, rhsValue->As<String> ()); // not sure this is right way to compare?
                        }
                    }
                }
                if (not transferred) {
                    clientSideOps += op; // keep for client side
                }
            }
            if (idWhereTest or not jsonWhereTests.empty ()) {
                StringBuilder whereClause;
                if (idWhereTest) {
                    whereClause << *idWhereTest;
                }
                if (not jsonWhereTests.empty ()) {
                    if (not whereClause.empty ()) {
                        whereClause << " && "sv; // dont recall right sql syntax - will need to lookup
                    }
                    whereClause << "json_extract(json, ";
                    size_t n = 0;
                    for (auto i : jsonWhereTests) {
                        if (n != 0) {
                            whereClause << ", ";
                        }
                        whereClause << "'{}'"_f(i);
                        n++;
                    }
                    whereClause << ")";
                }
                // if we moved any to server side, then return the filterDoc and the client side ops
                return make_tuple (whereClause, clientSideOps.empty () ? optional<Filter>{} : make_optional (Filter{clientSideOps}));
            }
            // else no change
            return make_tuple (nullopt, filter);
        }
        return make_tuple (nullopt, nullopt);
    }
}

namespace {
    /**
     * Break the given Stroika filter into parts that can be handled in sqlite, and parts that must be handled locally
     * Also return array showing names cuz sqlite returns array (for > 1) and these are the names in this order of the fields/objects:
     */
    tuple<optional<tuple<String, Sequence<String>>>, optional<Projection>> Partition_ (const optional<Projection>& p)
    {
        if (p) {
            /*
             *  json_extract appears to only support 'include' and not 'omit' operations
             */
            tuple<Document::Projection::Flag, Set<String>> fields = p->GetFields ();
            Require (get<1> (fields).size () >= 1); // cannot (usefully) project to null-space
            if (get<Document::Projection::Flag> (fields) == Document::Projection::Flag::eInclude) {
                StringBuilder    projectionQuery;
                Sequence<String> fieldNames;
                for (String f : get<1> (fields)) {
                    String mongoFieldName = f;
                    if (fieldNames.empty ()) {
                        projectionQuery << "json_extract(json,'";
                    }
                    else {
                        projectionQuery << ","sv;
                    }
                    projectionQuery << "$."sv << mongoFieldName;
                    fieldNames.push_back (mongoFieldName);
                }
                if (not fieldNames.empty ()) {
                    projectionQuery << "')"sv;
                }
                return make_tuple (make_tuple (projectionQuery, fieldNames), nullopt);
            }
            else {
                return make_tuple (nullopt, p); // cannot optimize exclude (yet)
            }
        }
        return make_tuple (nullopt, nullopt);
    }
    // called with the result of a statement after a 'step' operation that produced a ROW.
    // And assumes the row contains data from the Partition_ algorithm above
    Document::Document ExtractRowValueAfterStep_ (sqlite3_stmt* statement, int dataCol, const IDType& id,
                                                  const optional<tuple<String, Sequence<String>>>& sqliteProjection,
                                                  const optional<Projection>&                      remainingProjection)
    {
        /*
         *      https://www.sqlite.org/json1.html
         *          "There is a subtle incompatibility between the json_extract() function in SQLite and the json_extract() function in MySQL. The MySQL version of json_extract() always returns JSON. The SQLite version of json_extract() only returns JSON if there are two or more PATH arguments"
         */
        VariantValue valueReadBackFromDB =
            Variant::JSON::Reader{}.Read (String::FromUTF8 (reinterpret_cast<const char*> (::sqlite3_column_text (statement, dataCol))));
        Document::Document dr;
        if (sqliteProjection == nullopt) {
            dr = valueReadBackFromDB.As<Mapping<String, VariantValue>> ();
        }
        else {
            auto arrayOfFieldNames = get<Sequence<String>> (*sqliteProjection);
            if (arrayOfFieldNames.size () == 1) {
                dr = Mapping<String, VariantValue>{{arrayOfFieldNames[0], valueReadBackFromDB}};
            }
            else {
                Assert (valueReadBackFromDB.GetType () == VariantValue::eArray);
                Assert (valueReadBackFromDB.As<Sequence<VariantValue>> ().size () == arrayOfFieldNames.size ());
                Iterator<String> nameI = arrayOfFieldNames.begin ();
                dr                     = valueReadBackFromDB.As<Sequence<VariantValue>> ().Map<Document::Document> (
                    [&nameI] (const VariantValue& vv) mutable { return KeyValuePair{*nameI++, vv}; });
                Assert (nameI == arrayOfFieldNames.end ());
            }
        }
        if ((sqliteProjection and get<Sequence<String>> (*sqliteProjection).Contains (Document::kID)) or remainingProjection == nullopt or
            remainingProjection->Includes (Document::kID)) {
            dr.Add (Document::kID, id);
        }
        if (remainingProjection) {
            dr = remainingProjection->Apply (dr);
        }
        return dr;
    };
}

namespace {
    using Connection::Options;
    struct ConnectionRep_ final : Database::Document::SQLite::Connection::IRep {

        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fAssertExternallySynchronizedMutex_;

        struct CollectionRep_ final : Stroika::Foundation::Database::Document::Collection::IRep {
            [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fAssertExternallySynchronizedMutex_; // since shares unsynchronized connection, share its context
            shared_ptr<ConnectionRep_> fConnectionRep_; // save to bump reference count
            String                     fTableName_;

            MyPreparedStatement_ fAddStatement_{};
            MyPreparedStatement_ fGetOneStatement_{};
            MyPreparedStatement_ fRemoveStatement_{};
            MyPreparedStatement_ fUpdateStatement_{};

            CollectionRep_ (const shared_ptr<ConnectionRep_>& connectionRep, const String& collectionName)
                : fConnectionRep_{connectionRep}
                , fTableName_{collectionName}
            {
#if qStroika_Foundation_Debug_AssertExternallySynchronizedMutex_Enabled
                fAssertExternallySynchronizedMutex_.SetAssertExternallySynchronizedMutexContext (
                    connectionRep->fAssertExternallySynchronizedMutex_.GetSharedContext ());
#endif
            }
            virtual ~CollectionRep_ () = default;
            virtual IDType Add (const Document::Document& v) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"SQLite::CollectionRep_::Add()"};
#endif
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySynchronizedMutex_};
                /**
                 *  UNCLEAR if this way of capturing row_id is threadsafe or not.
                 *  MAYBE OK if not using 'full mutex' mode on database connection? @todo FIGURE OUT!!!!
                 * 
                 * @todo: SIMONE suggests using GUID, and pre-computing the ID, and using that.
                 *      COULD just precompute the id (easier if sqlite had sequence type) - or do two inserts - lots of tricky ways.
                 *      none that efficient and clean and simple. I guess this is clean and simple and efficient, just probably a race
                 */
                if (fAddStatement_ == nullptr) [[unlikely]] {
                    fAddStatement_ = MyPreparedStatement_{fConnectionRep_->fDB_, "insert into {} (json) values(?);"_f(fTableName_)};
                }
                string jsonText = Variant::JSON::Writer{}.WriteAsString (VariantValue{v}).AsUTF8<string> ();
                ThrowSQLiteErrorIfNotOK_ (::sqlite3_reset (fAddStatement_), fConnectionRep_->fDB_);
                ThrowSQLiteErrorIfNotOK_ (::sqlite3_bind_text (fAddStatement_, 1, jsonText.c_str (), static_cast<int> (jsonText.length ()), SQLITE_TRANSIENT),
                                          fConnectionRep_->fDB_);
                int rc = ::sqlite3_step (fAddStatement_);
                if (rc != SQLITE_DONE) {
                    ThrowSQLiteErrorIfNotOK_ (rc, fConnectionRep_->fDB_);
                }
                return "{}"_f(sqlite3_last_insert_rowid (fConnectionRep_->fDB_));
            }
            virtual optional<Document::Document> GetOne (const IDType& id, const optional<Projection>& projection) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"SQLite::CollectionRep_::GetOne()", "id={}, projection={}"_f, id, projection};
#endif
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySynchronizedMutex_};

                // locally construct MyPreparedStatement_ for case with projection, and/or cache statement for grabbing whole thing
                auto [sqliteProjection, remainingAfterProjection] = Partition_ (projection);
                optional<MyPreparedStatement_> sqliteProjectionStatement;
                if (sqliteProjection) {
                    sqliteProjectionStatement.emplace (fConnectionRep_->fDB_,
                                                       "select {} from {} where id=?;"_f(get<String> (*sqliteProjection), fTableName_));
                }
                else if (fGetOneStatement_ == nullptr) [[unlikely]] {
                    fGetOneStatement_ = MyPreparedStatement_{fConnectionRep_->fDB_, "select json from {} where id=?;"_f(fTableName_)};
                }
                sqlite3_stmt* useStatment = sqliteProjectionStatement.has_value () ? *sqliteProjectionStatement : fGetOneStatement_;
                AssertNotNull (useStatment);

                ThrowSQLiteErrorIfNotOK_ (::sqlite3_reset (useStatment), fConnectionRep_->fDB_);
                string idAsUTFSTR = id.AsUTF8<string> ();
                ThrowSQLiteErrorIfNotOK_ (::sqlite3_bind_text (useStatment, 1, idAsUTFSTR.c_str (), static_cast<int> (idAsUTFSTR.length ()), SQLITE_TRANSIENT),
                                          fConnectionRep_->fDB_);

                int                          rc = ::sqlite3_step (useStatment);
                optional<Document::Document> result;
                if (rc == SQLITE_ROW) [[likely]] {
                    result = ExtractRowValueAfterStep_ (useStatment, 0, id, sqliteProjection, remainingAfterProjection);
                    rc     = ::sqlite3_step (useStatment);
                }
                if (rc != SQLITE_DONE) [[unlikely]] {
                    ThrowSQLiteErrorIfNotOK_ (rc, fConnectionRep_->fDB_);
                }
                return result;
            }
            virtual Sequence<Document::Document> GetAll (const optional<Filter>& filter, const optional<Projection>& projection) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"SQLite::CollectionRep_::GetAll()", "filter={}, projection={}"_f, filter, projection};
#endif
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySynchronizedMutex_};

                // Optimize some important special cases
                Sequence<Document::Document> result;
                if (filter == nullopt and projection == nullopt) {
                    MyPreparedStatement_ statement{fConnectionRep_->fDB_, "select id,json from {};"_f(fTableName_)};
                    ThrowSQLiteErrorIfNotOK_ (::sqlite3_reset (statement), fConnectionRep_->fDB_);
                    int rc;
                    while ((rc = ::sqlite3_step (statement)) == SQLITE_ROW) {
                        String       id = String::FromUTF8 (reinterpret_cast<const char*> (::sqlite3_column_text (statement, 0)));
                        VariantValue valueReadBackFromDB =
                            Variant::JSON::Reader{}.Read (String::FromUTF8 (reinterpret_cast<const char*> (::sqlite3_column_text (statement, 1))));
                        Document::Document vDoc = valueReadBackFromDB.As<Mapping<String, VariantValue>> ();
                        vDoc.Add (Document::kID, id);
                        result += vDoc;
                    }
                    if (rc != SQLITE_DONE) [[unlikely]] {
                        ThrowSQLiteErrorIfNotOK_ (rc, fConnectionRep_->fDB_);
                    }
                }
                else if (filter == nullopt and projection == kOnlyIDs) {
                    MyPreparedStatement_ statement{fConnectionRep_->fDB_, "select id from {};"_f(fTableName_)};
                    ThrowSQLiteErrorIfNotOK_ (::sqlite3_reset (statement), fConnectionRep_->fDB_);
                    int rc;
                    while ((rc = ::sqlite3_step (statement)) == SQLITE_ROW) {
                        String             id = String::FromUTF8 (reinterpret_cast<const char*> (::sqlite3_column_text (statement, 0)));
                        Document::Document vDoc;
                        vDoc.Add (Document::kID, id);
                        result += vDoc;
                    }
                    if (rc != SQLITE_DONE) [[unlikely]] {
                        ThrowSQLiteErrorIfNotOK_ (rc, fConnectionRep_->fDB_);
                    }
                }
                else {
                    // general case
                    auto [sqliteWhereClause, remainingFilter] = Partition_ (filter);

                    // if there is a remainingFilter (performed after sqlite) - we need to form full objects and then apply the filter at the end
                    // so the filter can access those fields. REALLY - we could do a little better than this in general, but this is a good first attempt
                    auto [sqliteProjection, remainingAfterProjection] = remainingFilter ? make_tuple (nullopt, projection) : Partition_ (projection);

                    MyPreparedStatement_ statement{
                        fConnectionRep_->fDB_,
                        "select id,{} from {} {};"_f(sqliteProjection == nullopt ? "json"_k : get<String> (*sqliteProjection), fTableName_,
                                                     sqliteWhereClause == nullopt ? "" : ("where "_k + *sqliteWhereClause))};

                    ThrowSQLiteErrorIfNotOK_ (::sqlite3_reset (statement), fConnectionRep_->fDB_);
                    int rc;
                    while ((rc = ::sqlite3_step (statement)) == SQLITE_ROW) {
                        String             id   = String::FromUTF8 (reinterpret_cast<const char*> (::sqlite3_column_text (statement, 0)));
                        Document::Document vDoc = ExtractRowValueAfterStep_ (statement, 1, id, sqliteProjection, remainingAfterProjection);
                        if (remainingFilter == nullopt or remainingFilter->Matches (vDoc)) {
                            if (remainingAfterProjection) {
                                vDoc = remainingAfterProjection->Apply (vDoc); // some attributes need to be projected after filter cuz maybe used in filtering
                            }
                            result += vDoc;
                        }
                    }
                    if (rc != SQLITE_DONE) [[unlikely]] {
                        ThrowSQLiteErrorIfNotOK_ (rc, fConnectionRep_->fDB_);
                    }
                }
                return result;
            }
            virtual void Update (const IDType& id, const Document::Document& newV, const optional<Set<String>>& onlyTheseFields) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"SQLite::CollectionRep_::Update()"};
#endif
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySynchronizedMutex_};
                Document::Document                                     uploadDoc = newV;
                if (onlyTheseFields) {
                    uploadDoc.RetainAll (*onlyTheseFields);
                }
                // POOR IMPLEMENTATION - should use sql update - but tricky for this case, so KISS, and get functionally working so
                // I can integrate this code in regtests
                Document::Document d2Update = onlyTheseFields ? Memory::ValueOfOrThrow (this->GetOne (id, nullopt)) : uploadDoc;
                // any fields listed in onlyTheseFields, but not present in newV need to be removed
                if (onlyTheseFields) {
                    d2Update.AddAll (uploadDoc);
                    Set<String> removeMe = *onlyTheseFields - newV.Keys ();
                    d2Update.RemoveAll (removeMe);
                }

                if (fUpdateStatement_ == nullptr) [[unlikely]] {
                    fUpdateStatement_ = MyPreparedStatement_{fConnectionRep_->fDB_, "update {} SET json=? where id=?;"_f(fTableName_)};
                }
                string r = Variant::JSON::Writer{}.WriteAsString (VariantValue{d2Update}).AsUTF8<string> ();
                ThrowSQLiteErrorIfNotOK_ (::sqlite3_reset (fUpdateStatement_), fConnectionRep_->fDB_);
                string idText = id.AsUTF8<string> ();
                ThrowSQLiteErrorIfNotOK_ (::sqlite3_bind_text (fUpdateStatement_, 1, r.c_str (), static_cast<int> (r.length ()), SQLITE_TRANSIENT),
                                          fConnectionRep_->fDB_);
                ThrowSQLiteErrorIfNotOK_ (::sqlite3_bind_text (fUpdateStatement_, 2, idText.c_str (), static_cast<int> (idText.length ()), SQLITE_TRANSIENT),
                                          fConnectionRep_->fDB_);
                int rc = ::sqlite3_step (fUpdateStatement_);
                if (rc != SQLITE_DONE) {
                    ThrowSQLiteErrorIfNotOK_ (rc, fConnectionRep_->fDB_);
                }
            }
            virtual void Remove (const IDType& id) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"SQLite::CollectionRep_::Remove()"};
#endif
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySynchronizedMutex_};
                if (fRemoveStatement_ == nullptr) [[unlikely]] {
                    fRemoveStatement_ = MyPreparedStatement_{fConnectionRep_->fDB_, "delete from {} where id=?;"_f(fTableName_)};
                }
                ThrowSQLiteErrorIfNotOK_ (::sqlite3_reset (fRemoveStatement_), fConnectionRep_->fDB_);
                string idText = id.AsUTF8<string> ();
                ThrowSQLiteErrorIfNotOK_ (::sqlite3_bind_text (fRemoveStatement_, 1, idText.c_str (), static_cast<int> (idText.length ()), SQLITE_TRANSIENT),
                                          fConnectionRep_->fDB_);
                int rc = ::sqlite3_step (fRemoveStatement_);
                if (rc != SQLITE_DONE) {
                    ThrowSQLiteErrorIfNotOK_ (rc, fConnectionRep_->fDB_);
                }
            }
        };

        ConnectionRep_ (const Options& options)
        {
            TraceContextBumper ctx{"SQLite::Connection::ConnectionRep_::ConnectionRep_"};

            int flags = 0;
            // https://www.sqlite.org/threadsafe.html explains the thread-safety stuff. Not sure I have it right, but hopefully --LGP 2023-09-13
            switch (options.fThreadingMode.value_or (Options::kDefault_ThreadingMode)) {
                case Options::ThreadingMode::eSingleThread:
                    break;
                case Options::ThreadingMode::eMultiThread:
                    Require (CompiledOptions::kThe.THREADSAFE);
                    Require (::sqlite3_threadsafe ());
                    flags |= SQLITE_OPEN_NOMUTEX;
                    break;
                case Options::ThreadingMode::eSerialized:
                    Require (CompiledOptions::kThe.THREADSAFE);
                    Require (::sqlite3_threadsafe ());
                    flags |= SQLITE_OPEN_FULLMUTEX;
                    break;
            }

            if (options.fImmutable) {
                // NYI cuz requires uri syntax
                WeakAssertNotImplemented ();
                Require (options.fReadOnly);
            }
            flags |= options.fReadOnly ? SQLITE_OPEN_READONLY : SQLITE_OPEN_READWRITE;

            string uriArg;
            if constexpr (qStroika_Foundation_Debug_AssertionsChecked) {
                [[maybe_unused]] int n{};
                if (options.fDBPath) {
                    ++n;
                }
                if (options.fTemporaryDB) {
                    ++n;
                }
                if (options.fInMemoryDB) {
                    ++n;
                }
                Require (n == 1); // exactly one of fDBPath, fTemporaryDB, fInMemoryDB must be provided
            }
            if (options.fDBPath) {
                uriArg = options.fDBPath->generic_string ();
                if (uriArg[0] == ':') {
                    uriArg = "./" + uriArg; // sqlite docs warn to do this, to avoid issues with :memory or other extensions
                }
            }
            if (options.fTemporaryDB) {
                uriArg = string{};
                // According to https://sqlite.org/inmemorydb.html, temporary DBs appear to require empty name
                // @todo MAYBE fix to find a way to do named temporary DB? - or adjust API so no string name provided.
                Require (not options.fTemporaryDB->empty ());
            }
            if (options.fInMemoryDB) {
                // Not super clear why SQLITE_OPEN_URI needed, but the example in docs uses URI, and tracing through the sqlite open code
                // it appears to require a URI format, but not really documented as near as I can tell...--LGP 2025-03-31
                flags |= SQLITE_OPEN_MEMORY;
                flags |= SQLITE_OPEN_URI;
                flags |= SQLITE_OPEN_SHAREDCACHE;
                Require (not options.fReadOnly);
                Require (options.fCreateDBPathIfDoesNotExist);
                uriArg = options.fInMemoryDB->AsNarrowSDKString (); // often empty string
                if (uriArg.empty ()) {
                    uriArg = ":memory";
                }
                else {
                    u8string safeCharURI = IO::Network::UniformResourceIdentification::PCTEncode (u8string{uriArg.begin (), uriArg.end ()}, {});
                    uriArg = "file:" + string{safeCharURI.begin (), safeCharURI.end ()} + "?mode=memory&cache=shared";
                }
                // For now, it appears we ALWAYS create memory DBS when opening (so cannot find a way to open shared) - so always set created flag
            }

            int e;
            if ((e = ::sqlite3_open_v2 (uriArg.c_str (), &fDB_, flags, options.fVFS ? options.fVFS->AsNarrowSDKString ().c_str () : nullptr)) == SQLITE_CANTOPEN) {
                if (options.fCreateDBPathIfDoesNotExist) {
                    if (fDB_ != nullptr) {
                        Verify (::sqlite3_close (fDB_) == SQLITE_OK);
                        fDB_ = nullptr;
                    }
                    if ((e = ::sqlite3_open_v2 (uriArg.c_str (), &fDB_, SQLITE_OPEN_CREATE | flags,
                                                options.fVFS ? options.fVFS->AsNarrowSDKString ().c_str () : nullptr)) == SQLITE_OK) {
                        ; // if?
                    }
                }
            }
            if (e != SQLITE_OK) [[unlikely]] {
                [[maybe_unused]] auto&& cleanup = Finally ([this] () noexcept {
                    if (fDB_ != nullptr) {
                        Verify (::sqlite3_close (fDB_) == SQLITE_OK);
                    }
                });
                ThrowSQLiteError_ (e, fDB_);
            }
            if (options.fBusyTimeout) {
                SetBusyTimeout (*options.fBusyTimeout);
            }
            if (options.fJournalMode) {
                SetJournalMode (*options.fJournalMode);
            }
            EnsureNotNull (fDB_);
        }
        ~ConnectionRep_ ()
        {
            AssertNotNull (fDB_);
            Verify (::sqlite3_close (fDB_) == SQLITE_OK);
        }
        virtual shared_ptr<const EngineProperties> GetEngineProperties () const override
        {
            struct MyEngineProperties_ final : EngineProperties {
                virtual String GetEngineName () const override
                {
                    return "SQLite"sv;
                }
            };
            static const shared_ptr<const EngineProperties> kProps_ = make_shared<const MyEngineProperties_> ();
            return kProps_;
        }
        virtual Set<String> GetCollections () override
        {
            // treat named all tables as collections (maybe just count those with two columns id/json?).
            Set<String> results;
            auto        callback = SQLiteCallback_{[&] ([[maybe_unused]] int argc, char** argv, [[maybe_unused]] char** azColName) {
                Assert (argc == 1);
                results.Add (String::FromUTF8 (argv[0]));
                return SQLITE_OK;
            }};
            //ThrowSQLiteErrorIfNotOK_ (::sqlite3_exec (fDB_, ".tables", callback, &results, nullptr)); not sure why this doesn't work
            ThrowSQLiteErrorIfNotOK_ (::sqlite3_exec (fDB_, "SELECT name FROM sqlite_master WHERE type='table';",
                                                      callback.GetStaticFunction (), callback.GetData (), nullptr),
                                      fDB_);
            return results;
        }
        virtual void CreateCollection (const String& name) override
        {
            Exec ("create table if not exists {} (id INTEGER PRIMARY KEY, json NOT NULL);"_f(name));
        }
        virtual void DropCollection (const String& name) override
        {
            Exec ("drop table {};"_f(name));
        }
        virtual Document::Collection::Ptr GetCollection (const String& name) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySynchronizedMutex_};
            return Document::Collection::Ptr{
                make_shared<CollectionRep_> (Debug::UncheckedDynamicPointerCast<ConnectionRep_> (shared_from_this ()), name)};
        }
        virtual Document::Transaction mkTransaction () override
        {
            Connection::Ptr conn = Connection::Ptr{Debug::UncheckedDynamicPointerCast<Connection::IRep> (shared_from_this ())};
            return Database::Document::SQLite::Transaction{conn};
        }
        virtual void Exec (const String& sql) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySynchronizedMutex_};
            int e = ::sqlite3_exec (fDB_, sql.AsUTF8<string> ().c_str (), nullptr, nullptr, nullptr);
            if (e != SQLITE_OK) [[unlikely]] {
                ThrowSQLiteErrorIfNotOK_ (e, fDB_);
            }
        }
        virtual ::sqlite3* Peek () override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySynchronizedMutex_}; // not super helpful, but could catch errors - reason not very helpful is we lose lock long before we stop using ptr
            return fDB_;
        }
        virtual Duration GetBusyTimeout () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fAssertExternallySynchronizedMutex_};
            optional<int>                                         d;
            auto callback = SQLiteCallback_{[&] ([[maybe_unused]] int argc, char** argv, [[maybe_unused]] char** azColName) {
                Assert (argc == 1);
                Assert (::strcmp (azColName[0], "timeout") == 0);
                int val = ::atoi (argv[0]);
                Assert (val >= 0);
                d = val;
                return SQLITE_OK;
            }};
            ThrowSQLiteErrorIfNotOK_ (::sqlite3_exec (fDB_, "pragma busy_timeout;", callback.GetStaticFunction (), callback.GetData (), nullptr), fDB_);
            Assert (d);
            return Duration{double (*d) / 1000.0};
        }
        virtual void SetBusyTimeout (const Duration& timeout) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySynchronizedMutex_};
            ThrowSQLiteErrorIfNotOK_ (::sqlite3_busy_timeout (fDB_, (int)(timeout.As<float> () * 1000)), fDB_);
        }
        virtual JournalModeType GetJournalMode () const override
        {
            optional<string> d;
            auto             callback = SQLiteCallback_{[&] ([[maybe_unused]] int argc, char** argv, [[maybe_unused]] char** azColName) {
                Assert (argc == 1);
                Assert (::strcmp (azColName[0], "journal_mode") == 0);
                d = argv[0];
                return SQLITE_OK;
            }};
            ThrowSQLiteErrorIfNotOK_ (::sqlite3_exec (fDB_, "pragma journal_mode;", callback.GetStaticFunction (), callback.GetData (), nullptr), fDB_);
            Assert (d);
            if (d == "delete"sv) {
                return JournalModeType::eDelete;
            }
            if (d == "truncate"sv) {
                return JournalModeType::eTruncate;
            }
            if (d == "persist"sv) {
                return JournalModeType::ePersist;
            }
            if (d == "memory"sv) {
                return JournalModeType::eMemory;
            }
            if (d == "wal"sv) {
                return JournalModeType::eWAL;
            }
            if (d == "wal2"sv) {
                return JournalModeType::eWAL2;
            }
            if (d == "off"sv) {
                return JournalModeType::eOff;
            }
            AssertNotReached ();
            return JournalModeType::eDelete;
        }
        virtual void SetJournalMode (JournalModeType journalMode) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySynchronizedMutex_};
            switch (journalMode) {
                case JournalModeType::eDelete:
                    ThrowSQLiteErrorIfNotOK_ (::sqlite3_exec (fDB_, "pragma journal_mode = 'delete';", nullptr, 0, nullptr), fDB_);
                    break;
                case JournalModeType::eTruncate:
                    ThrowSQLiteErrorIfNotOK_ (::sqlite3_exec (fDB_, "pragma journal_mode = 'truncate';", nullptr, 0, nullptr), fDB_);
                    break;
                case JournalModeType::ePersist:
                    ThrowSQLiteErrorIfNotOK_ (::sqlite3_exec (fDB_, "pragma journal_mode = 'persist';", nullptr, 0, nullptr), fDB_);
                    break;
                case JournalModeType::eMemory:
                    ThrowSQLiteErrorIfNotOK_ (::sqlite3_exec (fDB_, "pragma journal_mode = 'memory';", nullptr, 0, nullptr), fDB_);
                    break;
                case JournalModeType::eWAL:
                    ThrowSQLiteErrorIfNotOK_ (::sqlite3_exec (fDB_, "pragma journal_mode = 'wal';", nullptr, 0, nullptr), fDB_);
                    break;
                case JournalModeType::eWAL2:
                    if (GetJournalMode () == JournalModeType::eWAL) {
                        ThrowSQLiteErrorIfNotOK_ (::sqlite3_exec (fDB_, "pragma journal_mode = 'delete';", nullptr, 0, nullptr), fDB_);
                    }
                    ThrowSQLiteErrorIfNotOK_ (::sqlite3_exec (fDB_, "pragma journal_mode = 'wal2';", nullptr, 0, nullptr), fDB_);
                    break;
                case JournalModeType::eOff:
                    ThrowSQLiteErrorIfNotOK_ (::sqlite3_exec (fDB_, "pragma journal_mode = 'off';", nullptr, 0, nullptr), fDB_);
                    break;
            }
        }

        ::sqlite3* fDB_{};
    };
}

/*
 ********************************************************************************
 *********************** SQL::SQLite::Connection::Ptr ***************************
 ********************************************************************************
 */
Document::SQLite::Connection::Ptr::Ptr (const shared_ptr<IRep>& src)
    : inherited{src}
    , busyTimeout{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
                      const Ptr* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Ptr::busyTimeout);
                      RequireNotNull (thisObj->operator->());
                      return thisObj->operator->()->GetBusyTimeout ();
                  },
                  [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, auto timeout) {
                      Ptr* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Ptr::busyTimeout);
                      RequireNotNull (thisObj->operator->());
                      thisObj->operator->()->SetBusyTimeout (timeout);
                  }}
    , journalMode{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
                      const Ptr* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Ptr::journalMode);
                      RequireNotNull (thisObj->operator->());
                      return thisObj->operator->()->GetJournalMode ();
                  },
                  [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, auto journalMode) {
                      Ptr* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Ptr::journalMode);
                      RequireNotNull (thisObj->operator->());
                      thisObj->operator->()->SetJournalMode (journalMode);
                  }}
{
#if qStroika_Foundation_Debug_AssertExternallySynchronizedMutex_Enabled
    if (src != nullptr) {
        // _fAssertExternallySynchronizedMutex.SetAssertExternallySynchronizedMutexContext (src->_fAssertExternallySynchronizedMutex.GetSharedContext ());
    }
#endif
}

/*
 ********************************************************************************
 ************************** SQL::SQLite::Connection *****************************
 ********************************************************************************
 */
auto Document::SQLite::Connection::New (const Options& options) -> Ptr
{
    return Ptr{make_shared<ConnectionRep_> (options)};
}

/*
 ********************************************************************************
 ******************************* SQLite::Transaction ****************************
 ********************************************************************************
 */
struct Transaction::MyRep_ : public Database::Document::Transaction::IRep {
    MyRep_ (const Connection::Ptr& db, Flag f)
        : fConnectionPtr_{db}
    {
        switch (f) {
            case Flag::eDeferred:
                db->Exec ("BEGIN DEFERRED TRANSACTION;"sv);
                break;
            case Flag::eExclusive:
                db->Exec ("BEGIN EXCLUSIVE TRANSACTION;"sv);
                break;
            case Flag::eImmediate:
                db->Exec ("BEGIN IMMEDIATE TRANSACTION;"sv);
                break;
            default:
                RequireNotReached ();
        }
    }
    virtual void Commit () override
    {
        Require (not fCompleted_);
        fCompleted_ = true;
        fConnectionPtr_->Exec ("COMMIT TRANSACTION;"sv);
    }
    virtual void Rollback () override
    {
        Require (not fCompleted_);
        fCompleted_ = true;
        fConnectionPtr_->Exec ("ROLLBACK TRANSACTION;"sv);
    }
    virtual Disposition GetDisposition () const override
    {
        // @todo record more info so we can report finer grained status ; try/catch in rollback/commit and dbgtraces
        return fCompleted_ ? Disposition::eCompleted : Disposition::eNone;
    }
    Connection::Ptr fConnectionPtr_;
    bool            fCompleted_{false};
};
Transaction::Transaction (const Connection::Ptr& db, Flag f)
    : inherited{make_unique<MyRep_> (db, f)}
{
}
#endif
