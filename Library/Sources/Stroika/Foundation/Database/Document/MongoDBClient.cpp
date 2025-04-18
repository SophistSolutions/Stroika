/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_HasComponent_mongocxxdriver
DISABLE_COMPILER_MSC_WARNING_START (4166) // avoid warning of buggy usage cdecl on CTOR/DTOR- only shows up on x86 MSVC compilers
DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-literal-operator\""); // ...bsoncxx/v_noabi/bsoncxx/json.hpp:85:54: warning: identifier '_bson' preceded by whitespace in a literal operator declaration is deprecated [-Wdeprecated-literal-operator]
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/value.hpp>
#include <bsoncxx/types/bson_value/view.hpp>
#include <bsoncxx/view_or_value.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>
DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-literal-operator\"");
DISABLE_COMPILER_MSC_WARNING_END (4166)
#endif

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Containers/Concrete/Mapping_HashTable.h"
#include "Stroika/Foundation/Containers/Concrete/Sequence_stdvector.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Reader.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Writer.h"
#include "Stroika/Foundation/Database/Exception.h"
#include "Stroika/Foundation/Debug/Main.h"
#include "Stroika/Foundation/Memory/Common.h"

#include "MongoDBClient.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Database;
using namespace Stroika::Foundation::Database::Document::MongoDBClient;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;

using Containers::Concrete::Mapping_HashTable;
using Containers::Concrete::Sequence_stdvector;

using Database::Document::EngineProperties;
using Database::Document::Filter;
using Database::Document::IDType;
using Database::Document::Projection;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/**
 *  Character set Docx
 *      Haven't found clear docs about characterset (apis all take 'string') - but it appears (search through docs for
 *      characterset comes up with nothing but UTF-8) that they use UTF-8.
 */

#if qStroika_HasComponent_mongocxxdriver

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::sub_array;
using bsoncxx::builder::basic::sub_document;

namespace {
    const String kMongoID_ = "_id"sv;
}

namespace {
    atomic<unsigned int> sActivatorLiveCnt_{0};
}

namespace {
    auto ConnectionString2MongoURI_ (const String& connectionString)
    {
        return mongocxx::uri{connectionString.AsUTF8<string> ()};
    }
}

namespace {
    String ID_2_string_ (const bsoncxx::types::bson_value::view& value)
    {
        switch (value.type ()) {
            case bsoncxx::type::k_oid:
                return String{value.get_oid ().value.to_string ()};
            default:
                AssertNotReached ();
                return String{};
        }
    }
    template <Common::IAnyOf<bsoncxx::types::bson_value::view, bsoncxx::document::element, bsoncxx::array::element> T>
    VariantValue BSON2VV_ (const T& value)
    {
        switch (value.type ()) {
            case bsoncxx::type::k_double:
                return value.get_double ().value;
            case bsoncxx::type::k_string:
                return String::FromUTF8 (SpanBytesCast<span<const char8_t>> (span{value.get_string ().value}));
            case bsoncxx::type::k_document: {
                Mapping_HashTable<String, VariantValue>::DEFAULT_HASHTABLE<> vvResult; // performance tweak, add in STL, avoiding virtual calls for each add, and then move to Stroika mapping
                const bsoncxx::types::b_document& thisDoc = value.get_document ();
                for (auto di : thisDoc.value) {
                    vvResult.Add (String::FromUTF8 (span{di.key ()}), BSON2VV_ (di));
                }
                return VariantValue{Mapping_HashTable<String, VariantValue>{move (vvResult)}};
            }
            case bsoncxx::type::k_array: {
                vector<VariantValue>           vvResult;
                const bsoncxx::types::b_array& thisArray = value.get_array ();
                vvResult.reserve (distance (thisArray.value.begin (), thisArray.value.end ()));
                for (auto ai : thisArray.value) {
                    vvResult.push_back (BSON2VV_ (ai));
                }
                return VariantValue{Sequence_stdvector<VariantValue>{move (vvResult)}};
            }
            case bsoncxx::type::k_binary:
                return Memory::BLOB{span{value.get_binary ().bytes, static_cast<size_t> (value.get_binary ().size)}};
            case bsoncxx::type::k_undefined:
                return VariantValue{nullptr}; // Stroika VariantValue doesn't distinguish between null and undefined
            case bsoncxx::type::k_oid:
                return String{value.get_oid ().value.to_string ()};
            case bsoncxx::type::k_bool:
                return static_cast<bool> (value.get_bool ());
            case bsoncxx::type::k_date:
                // Note - STROIKA doesn't usually (ever) generate these, because of problems documented in VV2BSONV_
                return Time::DateTime{chrono::time_point<chrono::system_clock>{value.get_date ().value}}; // UTC datetime.
            case bsoncxx::type::k_null:
                return VariantValue{nullptr};
            case bsoncxx::type::k_regex:
                return String::FromUTF8 (SpanBytesCast<span<const char8_t>> (span{value.get_string ().value}));
            case bsoncxx::type::k_dbpointer:
                WeakAssertNotReached (); ///< DBPointer. @deprecated
                return VariantValue{};
            case bsoncxx::type::k_code:
                WeakAssertNotReached ();
                return VariantValue{};
            case bsoncxx::type::k_symbol:
                WeakAssertNotReached (); ///< Symbol. @deprecated
                return VariantValue{};
            case bsoncxx::type::k_codewscope:
                WeakAssertNotReached ();
                return VariantValue{};
            case bsoncxx::type::k_int32:
                return value.get_int32 ().value;
            case bsoncxx::type::k_timestamp:
                WeakAssertNotReached (); // not sure how to translate/interpret
                return VariantValue{};
            case bsoncxx::type::k_int64:
                return value.get_int64 ().value;
            case bsoncxx::type::k_decimal128:
                WeakAssertNotReached (); // ///< 128-bit decimal floating point. == not sure what todo
                return VariantValue{};
            case bsoncxx::type::k_maxkey:
            case bsoncxx::type::k_minkey:
                WeakAssertNotReached (); // not sure what todo
                return VariantValue{};
            default:
                AssertNotReached (); // all cases covered
                return VariantValue{};
        }
    }
    bsoncxx::types::bson_value::value VV2BSONV_ (const VariantValue& vv)
    {
        using namespace std::chrono;
        // @todo adequate first draft, but not 100% right conversions --LGP 2025-03-24
        switch (vv.GetType ()) {
            case VariantValue::Type::eNull:
                return bsoncxx::types::bson_value::value{nullptr};
            case VariantValue::Type::eBLOB:
                return bsoncxx::types::bson_value::value{vv.As<Memory::BLOB> ().As<vector<uint8_t>> ()};
            case VariantValue::Type::eBoolean:
                return bsoncxx::types::bson_value::value{vv.As<bool> ()};
            case VariantValue::Type::eInteger:
                return bsoncxx::types::bson_value::value{vv.As<int64_t> ()};
            case VariantValue::Type::eUnsignedInteger:
                return bsoncxx::types::bson_value::value{static_cast<int64_t> (vv.As<uint64_t> ())}; // @todo tweak - not quite right
            case VariantValue::Type::eFloat:
                return bsoncxx::types::bson_value::value{vv.As<double> ()};
            case VariantValue::Type::eDate:
                // MongoDB doesn't support dates before 1970, so store as string
                // Also, because mongocxx api uses system_clock, which is neither a steady_clock nor monotonic_clock, its really not suitable for roundtripping.
                // So just store dates as strings in the database (and our read code will convert them back to DateTime as appropriate - really VariantValue)
                //      if (Date dt = vv.As<Date> (); dt > Date{1970y / January / 1d}) {
                //          return bsoncxx::types::bson_value::value{bsoncxx::types::b_date{dt.As<std::chrono::system_clock::time_point> ()}};
                //      }
                return vv.As<Date> ().Format (Date::kISO8601Format).AsUTF8<string> ();
            case VariantValue::Type::eDateTime:
                // MongoDB doesn't support dates before 1970, so store as string
                // only makes sense to store UTC dates in database (so convert to UTC before ISO8601)
                // Also, because mongocxx api uses system_clock, which is neither a steady_clock nor monotonic_clock, its really not suitable for roundtripping.
                // So just store dates as strings in the database (and our read code will convert them back to DateTime as appropriate - really VariantValue)
                //      if (DateTime dt = vv.As<DateTime> ().AsUTC (); dt.GetDate () > Date{1970y / January / 1d}) {
                //          return bsoncxx::types::bson_value::value{bsoncxx::types::b_date{dt.As<std::chrono::system_clock::time_point> ()}};
                //      }
                return vv.As<DateTime> ().AsUTC ().Format (DateTime::kISO8601Format).AsUTF8<string> ();
            case VariantValue::Type::eString:
                return bsoncxx::types::bson_value::value{vv.As<String> ().AsUTF8<string> ()};
            case VariantValue::Type::eArray: {
                bsoncxx::builder::basic::array bsonArr;
                for (const auto& ai : vv.As<Sequence<VariantValue>> ()) {
                    bsonArr.append (VV2BSONV_ (ai));
                }
                return bsoncxx::types::bson_value::value{bsonArr};
            }
            case VariantValue::Type::eMap: {
                bsoncxx::builder::basic::document bsonDoc;
                for (const KeyValuePair<String, VariantValue>& ai : vv.As<Mapping<String, VariantValue>> ()) {
                    bsonDoc.append (kvp (ai.fKey.AsUTF8<string> (), VV2BSONV_ (ai.fValue)));
                }
                return bsoncxx::types::bson_value::value{bsonDoc};
            }
            default:
                AssertNotReached (); // all cases covered
                return bsoncxx::types::bson_value::value{nullptr};
        }
    }
    Document::Document FromBSON_ (const bsoncxx::document::view_or_value& b)
    {
        Mapping<String, VariantValue> result;
        for (const bsoncxx::document::element& di : b.view ()) {
            result.Add (String::FromUTF8 (span{di.key ()}), BSON2VV_ (di));
        }
        if (result.ContainsKey (kMongoID_)) {
            // patch '_id':oid => 'id':string
            VariantValue idValue = result[kMongoID_]; // {id: {$oid -> 67da17b30c4265ac0302f483}}
            result.Remove (kMongoID_);
            result.Add (Database::Document::kID, idValue);
        }
        return result;
    }
    bsoncxx::document::value ToBSON_ (const Document::Document& vv)
    {
        // more complex, but more performant version of
        //      bsoncxx::from_json (Variant::JSON::Writer{}.WriteAsString (VariantValue{vvv}).AsUTF8<string> ());
        Document::Document newDoc = vv;
        if (vv.ContainsKey (Database::Document::kID)) {
            // patch 'id':string => '_id':oid
            auto idValue = vv[Database::Document::kID];
            newDoc.Remove (Database::Document::kID);
            newDoc.Add (kMongoID_, VariantValue{Mapping<String, VariantValue>{{"$oid", idValue}}});
        }
        bsoncxx::builder::basic::document bsonDoc;
        for (const KeyValuePair<String, VariantValue>& ai : newDoc) {
            bsonDoc.append (kvp (ai.fKey.AsUTF8<string> (), VV2BSONV_ (ai.fValue)));
        }
        return bsonDoc.extract ();
    }
}

namespace {
    /**
     * Break the given Stroika filter into parts that can be remoted to MongoDB, and parts that must be handled locally
     * 
     * @param filter 
     * @return tuple<bsoncxx::document,Filter> 
     */
    tuple<optional<bsoncxx::document::value>, optional<Filter>> Partition_ (const optional<Filter>& filter)
    {
        if (filter) {
            /*
             *  For now just look for FIELD EQUALS VALUE expressions in the top level conjunction. These can be done
             *  server or client side transparently, and moving them server side is more efficient.
             * 
             *  Much more could be done, but this is a good cost/benefit start.
             */
            Sequence<Document::FilterElements::Operation> clientSideOps;
            bsoncxx::builder::basic::document             filterDoc;
            bool                                          anyTransfers = false;
            for (Document::FilterElements::Operation op : filter->GetConjunctionOperations ()) {
                bool transfered = false;
                if (const Document::FilterElements::Equals* eqOp = get_if<Document::FilterElements::Equals> (&op)) {
                    String useFieldName = eqOp->fLHS == Database::Document::kID ? kMongoID_ : eqOp->fLHS;
                    if (const Document::FilterElements::Value* rhsValue = get_if<Document::FilterElements::Value> (&eqOp->fRHS)) {
                        // move to server side
                        filterDoc.append (kvp (useFieldName.AsUTF8<string> (), VV2BSONV_ (*rhsValue)));
                        transfered   = true;
                        anyTransfers = true;
                    }
                }
                if (not transfered) {
                    clientSideOps += op; // keep for client side
                }
            }
            if (anyTransfers) {
                // if we moved any to server side, then return the filterDoc and the client side ops
                return make_tuple (filterDoc.extract (), clientSideOps.empty () ? optional<Filter>{} : make_optional (Filter{clientSideOps}));
            }
            // else no change
            return make_tuple (nullopt, filter);
        }
        return make_tuple (nullopt, nullopt);
    }
}

namespace {
    /**
     * Break the given Stroika filter into parts that can be remoted to MongoDB, and parts that must be handled locally
     * 
     * @param filter 
     * @return tuple<optional<bsoncxx::document::value>, optional<Projection>> 
     * 
     * SEE https://stackoverflow.com/questions/62704615/mongodb-projection-on-c
     */
    tuple<optional<bsoncxx::document::value>, optional<Projection>> Partition_ (const optional<Projection>& p)
    {
        if (p) {
            /*
             *  support mongoProjection - e.g. {{a: 1, b:0}} etc...
             */
            tuple<Document::Projection::Flag, Set<String>> fields = p->GetFields ();
            Require (get<1> (fields).size () >= 1); // cannot (usefully) project to null-space
            bsoncxx::builder::basic::document projectionDoc;
            for (String f : get<1> (fields)) {
                String mongoFieldName = f;
                if (mongoFieldName == Database::Document::kID) {
                    mongoFieldName = kMongoID_;
                }
                projectionDoc.append (kvp (mongoFieldName.AsUTF8<string> (), get<0> (fields) == Document::Projection::Flag::eInclude ? 1 : 0));
            }
            return make_tuple (projectionDoc.extract (), nullopt);
        }
        return make_tuple (nullopt, nullopt);
    }
}

namespace {
    struct AdminRep_ final : Stroika::Foundation::Database::Document::MongoDBClient::AdminConnection::IRep {
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fAssertExternallySynchronizedMutex_;
        variant<mongocxx::client, mongocxx::pool::entry>               fClientStorage_;
        mongocxx::client*                                              fClientPtr_;

        AdminRep_ (const AdminRep_&) = delete;
        AdminRep_ (const AdminConnection::Options& options)
        {
            TraceContextBumper ctx{"MongoDBClient:::AdminConnection::Rep_::CTOR"};
            if (auto os = get_if<String> (&options.fConnectionTarget)) {
                fClientStorage_ = mongocxx::client{ConnectionString2MongoURI_ (*os)};
                fClientPtr_     = get_if<mongocxx::client> (&fClientStorage_);
            }
            else if (auto op = get_if<ConnectionPool> (&options.fConnectionTarget)) {
                fClientStorage_ = op->PeekPool ().acquire ();
                fClientPtr_     = get<mongocxx::pool::entry> (fClientStorage_).operator->();
            }
            EnsureNotNull (fClientPtr_);
        }
        ~AdminRep_ () = default;

        // MongoDBClient::Connection::IRep overrides
    public:
        virtual mongocxx::client& GetClientRef () override
        {
            return *fClientPtr_;
        }
        virtual Document::Document run_command (const Document::Document& v) override
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            TraceContextBumper ctx{"MongoDBClient::AdminRep_::run_command"};
#endif
            mongocxx::database adminDB_;
            return FromBSON_ (fClientPtr_->database ("admin").run_command (ToBSON_ (v)));
        }
        virtual Set<String> GetDatabases () override
        {
            vector<string> n = fClientPtr_->list_database_names ();
            return Iterable<string>{n}.Map<Set<String>> ([] (string i) { return String::FromUTF8 (i); });
        }
        virtual void DropDatabase (const String& dbName) override
        {
            mongocxx::database{fClientPtr_->database (dbName.AsUTF8<string> ())}.drop ();
        }
        virtual void CreateDatabase (const String& dbName) override
        {
            // doesn't appear to be anything todo to create the database except maybe  writing to it
            mongocxx::database d{fClientPtr_->database (dbName.AsUTF8<string> ())};
            d.create_collection ("_junk_");
            d.collection ("_junk_").drop ();
        }
    };
}

namespace {
    struct ConnectionRep_ final : Stroika::Foundation::Database::Document::MongoDBClient::Connection::IRep {
        struct CollectionRep_ final : Stroika::Foundation::Database::Document::Collection::IRep {
            [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fAssertExternallySynchronizedMutex_; // since shares unsyncrhonized connection, share its context
            shared_ptr<ConnectionRep_> fConnectionRep_; // save to bump reference count
            mongocxx::collection       fCollection_;

            CollectionRep_ (const shared_ptr<ConnectionRep_>& connectionRep, const String& collectionName)
                : fConnectionRep_{connectionRep}
                , fCollection_{connectionRep->fDatabase_.collection (collectionName.AsUTF8<string> ())}
            {
#if qStroika_Foundation_Debug_AssertExternallySynchronizedMutex_Enabled
                fAssertExternallySynchronizedMutex_.SetAssertExternallySynchronizedMutexContext (
                    connectionRep->fAssertExternallySynchronizedMutex_.GetSharedContext ());
#endif
            }
            virtual IDType Add (const Document::Document& v) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"MongoDBClient::CollectionRep_::Add()"};
#endif
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySynchronizedMutex_};
                // auto insert_one_result = fCollection_.insert_one(make_document(kvp("i", 0)));
                if (auto insert_one_result = fCollection_.insert_one (ToBSON_ (v))) {
                    return ID_2_string_ (insert_one_result->inserted_id ());
                }
                Throw (RuntimeErrorException{"failed to add doc"});
            }
            virtual optional<Document::Document> GetOne (const IDType& id, const optional<Projection>& projection) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"MongoDBClient::CollectionRep_::GetOne()"};
#endif
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySynchronizedMutex_};
                bsoncxx::builder::basic::document                      filterDoc;
                filterDoc.append (kvp ("_id", bsoncxx::oid{id.AsUTF8<string> ()})); //kMongoID
                auto [mongoProjection, myProjection] = Partition_ (projection);
                mongocxx::options::find o;
                if (mongoProjection) {
                    o.projection (mongoProjection->view ());
                }
                auto result = fCollection_.find_one (filterDoc.view (), o);
                if (result) {
                    auto rr = FromBSON_ (bsoncxx::document::view_or_value{*result});
                    if (myProjection) {
                        rr = myProjection->Apply (rr);
                    }
                    return rr;
                }
                return nullopt;
            }
            virtual Sequence<Document::Document> GetAll (const optional<Filter>& filter, const optional<Projection>& projection) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"MongoDBClient::CollectionRep_::GetAll()"};
#endif
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySynchronizedMutex_};
                auto [mongoFilter, myFilter]         = Partition_ (filter);
                auto [mongoProjection, myProjection] = Partition_ (projection);
                Sequence<Document::Document> result;
                mongocxx::options::find      o;
                if (mongoProjection) {
                    o.projection (mongoProjection->view ());
                }
                auto cursor = fCollection_.find (mongoFilter ? mongoFilter->view () : bsoncxx::builder::basic::document{}.view (), o);
                for (auto&& doc : cursor) {
                    auto rr = FromBSON_ (doc);
                    if (myProjection) {
                        rr = myProjection->Apply (rr);
                    }
                    if (not filter or filter->Matches (rr)) {
                        result += rr;
                    }
                }
                return result;
            }
            virtual void Update (const IDType& id, const Document::Document& newV, const optional<Set<String>>& onlyTheseFields) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"MongoDBClient::CollectionRep_::Update()"};
#endif
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySynchronizedMutex_};
                // incomplete... - not sure how to handle partial update vs full update - must read mongo docs more carefully
                Document::Document uploadDoc = newV;
                if (onlyTheseFields) {
                    uploadDoc.RetainAll (*onlyTheseFields);
                }
                uploadDoc.RemoveIf ("id");
                bsoncxx::document::value bsonDoc = ToBSON_ (uploadDoc);
                if (onlyTheseFields) {
                    if (auto o = fCollection_.update_one (make_document (kvp ("_id", bsoncxx::oid{id.AsUTF8<string> ()})),
                                                          make_document (kvp ("$set", bsonDoc.view ())))) {
                        if (o->modified_count () == 0) {
                            Throw (RuntimeErrorException{"failed to update doc - not modified"});
                        }
                    }
                    else {
                        Throw (RuntimeErrorException{"failed to update doc"});
                    }
                }
                else {
                    if (auto o = fCollection_.replace_one (make_document (kvp ("_id", bsoncxx::oid{id.AsUTF8<string> ()})), bsonDoc.view ())) {
                        if (o->modified_count () == 0) {
                            Throw (RuntimeErrorException{"failed to replace doc - not modified"});
                        }
                    }
                    else {
                        Throw (RuntimeErrorException{"failed to replace doc"});
                    }
                }
            }
            virtual void Remove (const IDType& id) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"MongoDBClient::CollectionRep_::Remove()"};
#endif
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySynchronizedMutex_};
                bsoncxx::builder::basic::document                      filterDoc;
                filterDoc.append (kvp ("_id", bsoncxx::oid{id.AsUTF8<string> ()})); // kMongoID_
                auto result = fCollection_.delete_one (filterDoc.view ());
                if (result && result->deleted_count () == 0) {
                    Throw (RuntimeErrorException{"failed to delete doc"});
                }
            }
        };

        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fAssertExternallySynchronizedMutex_;
        variant<mongocxx::client, mongocxx::pool::entry> fClientStorage_; // not directly used, but needed to free the resource when this connection obj goes away - and implicitly stored in database
        mongocxx::database fDatabase_;

        ConnectionRep_ (const Connection::Options& options)
        {
            TraceContextBumper ctx{"MongoDBClient::ConnectionRep_::CTOR"};
            if (auto os = get_if<String> (&options.fConnectionTarget)) {
                fClientStorage_ = mongocxx::client{ConnectionString2MongoURI_ (*os)};
                fDatabase_      = get<mongocxx::client> (fClientStorage_).database (options.fDatabase.AsUTF8<string> ());
            }
            else if (auto op = get_if<ConnectionPool> (&options.fConnectionTarget)) {
                fClientStorage_ = op->PeekPool ().acquire ();
                fDatabase_      = get<mongocxx::pool::entry> (fClientStorage_)->database (options.fDatabase.AsUTF8<string> ());
            }
            Ensure (fDatabase_); // properly constructed
        }
        ~ConnectionRep_ () = default;

        // Document::Connection::IRep overrides
    public:
        virtual shared_ptr<const EngineProperties> GetEngineProperties () const override
        {
            struct MyEngineProperties_ final : EngineProperties {
                virtual String GetEngineName () const override
                {
                    return "mongo-cxx-driver"sv; // must indirect to connection to get more info (from dns at least? not clear)
                }
            };
            return make_shared<const MyEngineProperties_> (); // dynamic info based on connection/dsn
        }
        virtual Set<String> GetCollections () override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySynchronizedMutex_};
            try {
                vector<string> n = fDatabase_.list_collection_names ();
                return Iterable<string>{n}.Map<Set<String>> ([] (string i) { return String{i}; });
            }
            catch (const mongocxx::v_noabi::operation_exception& e) {
                DbgTrace ("e={}"_f, e);
                if (e.raw_server_error ()) {
                    DbgTrace ("e.raw={}"_f, FromBSON_ (e.raw_server_error ()->view ()));
                }
                // a specific error here - check for that - and throw others... no such database gets mapped to empty collection list
                return {};
            }
        }
        virtual void CreateCollection (const String& name) override
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            TraceContextBumper ctx{"mongocxx::CreateCollection()"};
#endif
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySynchronizedMutex_};
            fDatabase_.create_collection (name.AsUTF8<string> ());
        }
        virtual void DropCollection (const String& name) override
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            TraceContextBumper ctx{"mongocxx::DropCollection()"};
#endif
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySynchronizedMutex_};
            fDatabase_.collection (name.AsUTF8<string> ()).drop ();
        }
        virtual Document::Collection::Ptr GetCollection (const String& name) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySynchronizedMutex_};
            return Document::Collection::Ptr{
                make_shared<CollectionRep_> (Debug::UncheckedDynamicPointerCast<ConnectionRep_> (shared_from_this ()), name)};
        }
        virtual Document::Transaction mkTransaction () override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySynchronizedMutex_};
            Connection::Ptr conn = Connection::Ptr{Debug::UncheckedDynamicPointerCast<Connection::IRep> (shared_from_this ())};
            return Transaction{conn};
        }

        // MongoDBClient::Connection::IRep overrides
    public:
        virtual mongocxx::client& GetClientRef () override
        {
            if (auto oc = get_if<mongocxx::client> (&fClientStorage_)) {
                return *oc;
            }
            else if (auto oe = get_if<mongocxx::pool::entry> (&fClientStorage_)) {
                return *oe->operator->();
            }
            else {
                AssertNotReached ();
                static mongocxx::client x;
                return x;
            }
        }
    };
}

/*
 ********************************************************************************
 ********************* Document::MongoDBClient::Activator ***********************
 ********************************************************************************
 */
unique_ptr<mongocxx::instance> Document::MongoDBClient::Activator::sMongoInstance_;

Document::MongoDBClient::Activator::Activator ()
    : fAllowReactivation_{false}
{
    Require (Debug::AppearsDuringMainLifetime ());
    if (sActivatorLiveCnt_.fetch_add (1) == 0) {
        if (not sMongoInstance_) {
            sMongoInstance_ = make_unique<mongocxx::instance> ();
        }
    }
}
Document::MongoDBClient::Activator::Activator (AllowReactivateFlag)
    : Activator{}
{
    fAllowReactivation_ = true;
}

Document::MongoDBClient::Activator::~Activator ()
{
    Require (Debug::AppearsDuringMainLifetime ());
    Require (sActivatorLiveCnt_ > 0);
    if (sActivatorLiveCnt_.fetch_sub (1) == 0 and not fAllowReactivation_) {
        sMongoInstance_.reset ();
    }
}

/*
 ********************************************************************************
 ******************* Document::MongoDBClient::ConnectionPool ********************
 ********************************************************************************
 */
ConnectionPool::ConnectionPool (shared_ptr<mongocxx::pool>&& poolRep)
    : fPool_{move (poolRep)}
{
}
ConnectionPool::ConnectionPool (const String& connectionString)
    : ConnectionPool{make_shared<mongocxx::pool> (ConnectionString2MongoURI_ (connectionString))}
{
}

mongocxx::pool& ConnectionPool::PeekPool () const
{
    AssertNotNull (fPool_);
    return *fPool_;
}

/*
 ********************************************************************************
 ****************** Document::MongoDBClient::Connection::Ptr ********************
 ********************************************************************************
 */
Document::MongoDBClient::Connection::Ptr::Ptr (const shared_ptr<IRep>& src)
    : inherited{src}
{
}

/*
 ********************************************************************************
 **************** Document::MongoDBClient::AdminConnection::New *****************
 ********************************************************************************
 */
auto Document::MongoDBClient::AdminConnection::New (const AdminConnection::Options& options) -> Ptr
{
#if qStroika_Foundation_Debug_AssertionsChecked
    Require (sActivatorLiveCnt_ > 0);
#endif
    return Ptr{make_shared<AdminRep_> (options)};
}

/*
 ********************************************************************************
 ******************* Document::MongoDBClient::Connection::New *******************
 ********************************************************************************
 */
auto Document::MongoDBClient::Connection::New (const Connection::Options& options) -> Ptr
{
#if qStroika_Foundation_Debug_AssertionsChecked
    Require (sActivatorLiveCnt_ > 0);
#endif
    return Ptr{make_shared<ConnectionRep_> (options)};
}

/*
 ********************************************************************************
 ******************************* SQLite::Transaction ****************************
 ********************************************************************************
 */
struct Transaction::MyRep_ : public MongoDBClient::Transaction::IRep {
    MyRep_ (const Connection::Ptr& db)
        : fConnectionPtr_{db}
    {
        AssertNotImplemented ();
    }
    virtual void Commit () override
    {
        Require (not fCompleted_);
        fCompleted_ = true;
        // fConnectionPtr_->Exec ("COMMIT;"sv);
    }
    virtual void Rollback () override
    {
        Require (not fCompleted_);
        fCompleted_ = true;
        // fConnectionPtr_->Exec ("ROLLBACK;"sv);
    }
    virtual Disposition GetDisposition () const override
    {
        // @todo record more info so we can report finer grained status ; try/catch in rollback/commit and dbgtraces
        return fCompleted_ ? Disposition::eCompleted : Disposition::eNone;
    }
    Connection::Ptr fConnectionPtr_;
    bool            fCompleted_{false};
};
Transaction::Transaction (const Connection::Ptr& db)
    : inherited{make_unique<MyRep_> (db)}
{
}
#endif
