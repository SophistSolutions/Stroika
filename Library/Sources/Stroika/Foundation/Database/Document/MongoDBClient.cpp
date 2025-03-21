/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_HasComponent_mongocxxdriver
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/value.hpp>
#include <bsoncxx/types/bson_value/view.hpp>
#include <bsoncxx/view_or_value.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/uri.hpp>
#endif

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Reader.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Writer.h"
#include "Stroika/Foundation/Database/Exception.h"
#include "Stroika/Foundation/Debug/Main.h"

#include "MongoDBClient.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Database;
using namespace Stroika::Foundation::Database::Document::MongoDBClient;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;

using Database::Document::EngineProperties;
using Database::Document::Filter;
using Database::Document::IDType;
using Database::Document::Projection;

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
    String bson_value_to_string_ (const bsoncxx::types::bson_value::view& value)
    {
        switch (value.type ()) {
            case bsoncxx::type::k_string:
                return String::FromUTF8 (span<const char8_t>{reinterpret_cast<const char8_t*> (value.get_string ().value.data ()),
                                                             value.get_string ().value.size ()});
            case bsoncxx::type::k_oid:
                return String{value.get_oid ().value.to_string ()};
            case bsoncxx::type::k_bool:
                return value.get_bool () ? "true"_k : "false"_k;
            case bsoncxx::type::k_int32:
                return "{}"_f(value.get_int32 ().value);
            case bsoncxx::type::k_int64:
                return "{}"_f(value.get_int64 ().value);
            case bsoncxx::type::k_double:
                return "{}"_f(value.get_double ().value);
            default:
                throw std::invalid_argument{"Unsupported BSON type for string conversion"};
        }
    }
    Document::Document FromBSON_ (const bsoncxx::document::view_or_value& b)
    {
        // @todo - this is a ROUGH approximation - but deal with 'extended json' and make more efficient - especially BLOBS
        Mapping<String, VariantValue> result =
            Variant::JSON::Reader{}.Read (String::FromUTF8 (bsoncxx::to_json (b.view ()))).As<Mapping<String, VariantValue>> ();
        if (result.ContainsKey (kMongoID_)) {
            // patch 'id' <-> '_id' and value
            VariantValue idValue = result[kMongoID_]; // {id: {$oid -> 67da17b30c4265ac0302f483}}
            idValue              = idValue.As<Mapping<String, VariantValue>> ()["$oid"];
            result.Remove (kMongoID_);
            result.Add (Database::Document::kID, idValue);
        }
        return result;
    }
    bsoncxx::document::value ToBSON_ (const Document::Document& vv)
    {
        // @todo - this is a ROUGH approximation - but deal with 'extended json' and make more efficient - especially BLOBS
        if (vv.ContainsKey (Database::Document::kID)) {
            // patch 'id' <-> '_id' and value
            Document::Document vvv     = vv;
            auto               idValue = vv[Database::Document::kID];
            vvv.Remove (Database::Document::kID);
            vvv.Add (kMongoID_, VariantValue{Mapping<String, VariantValue>{{"$oid", idValue}}});
            return bsoncxx::from_json (Variant::JSON::Writer{}.WriteAsString (VariantValue{vvv}).AsUTF8<string> ());
        }
        else {
            return bsoncxx::from_json (Variant::JSON::Writer{}.WriteAsString (VariantValue{vv}).AsUTF8<string> ());
        }
    }
}

namespace {
    /**
     * Break the given Stroika filter into parts that can be remoted to MongoDB, and parts that must be handled locally
     * 
     * @param filter 
     * @return tuple<bsoncxx::document,Filter> 
     */
    tuple<optional<bsoncxx::document::value>, optional<Filter>> Parse_ (const optional<Filter>& filter)
    {
        if (filter) {
            // NYI - just return empty for now
            // make_document ()
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
     * 
     * SEE https://stackoverflow.com/questions/62704615/mongodb-projection-on-c
     */
    tuple<optional<bsoncxx::document::value>, optional<Projection>> Parse_ (const optional<Projection>& p)
    {
        // NYI - just return empty for now
        if (p) {
            // @todo support mongoProjection - {{a: 1, b:0}} etc...
            return make_tuple (nullopt, p);
        }
        return make_tuple (nullopt, nullopt);
    }
}

namespace {
    struct AdminRep_ final : Stroika::Foundation::Database::Document::MongoDBClient::AdminConnection::IRep {
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fAssertExternallySynchronizedMutex_;
        mongocxx::client                                               fClient_;

        AdminRep_ (const AdminConnection::Options& options)
            : fClient_{mongocxx::uri{options.fConnectionString.AsUTF8<string> ()}} // @todo not sure about charset to map to?
        {
            TraceContextBumper ctx{"Document::MongoDBClient::AdminConnection::Rep_::CTOR"};
        }
        ~AdminRep_ () = default;

        // MongoDBClient::Connection::IRep overrides
    public:
        virtual mongocxx::client& GetClientRef () override
        {
            return fClient_;
        }
        virtual Document::Document run_command (const Document::Document& v) override
        {
            mongocxx::database adminDB_;
            return FromBSON_ (fClient_.database ("admin").run_command (ToBSON_ (v)));
        }
        virtual Set<String> GetDatabases () override
        {
            vector<string> n = fClient_.list_database_names ();
            return Iterable<string>{n}.Map<Set<String>> ([] (string i) { return String{i}; });
        }
        virtual void DropDatabase (const String& dbName) override
        {
            mongocxx::database{fClient_.database (dbName.AsUTF8<string> ())}.drop ();
        }
        virtual void CreateDatabase (const String& dbName) override
        {
            // doesn't appear to be anything todo to create the database except maybe  writing to it
            mongocxx::database d{fClient_.database (dbName.AsUTF8<string> ())};
            d.create_collection ("_junk_");
            d.collection ("_junk_").drop ();
        }
    };
}

namespace {
    struct ConnectionRep_ final : Stroika::Foundation::Database::Document::MongoDBClient::Connection::IRep {
        struct CollectionRep_ final : Stroika::Foundation::Database::Document::Collection::IRep {
            [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fAssertExternallySynchronizedMutex_; // @todo these fAssert... guys all muyst be linked togetoher since have internal pointers into each other
            mongocxx::collection fCollection_;

            CollectionRep_ (ConnectionRep_& connectionRep, const String& collectionName)
                : fCollection_{connectionRep.fDatabase_.collection (collectionName.AsUTF8<string> ())}
            {
#if qStroika_Foundation_Debug_AssertExternallySynchronizedMutex_Enabled
                fAssertExternallySynchronizedMutex_.SetAssertExternallySynchronizedMutexContext (
                    connectionRep.fAssertExternallySynchronizedMutex_.GetSharedContext ());
#endif
            }
            virtual IDType AddDocument (const Document::Document& v) override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySynchronizedMutex_};
                // auto insert_one_result = fCollection_.insert_one(make_document(kvp("i", 0)));
                if (auto insert_one_result = fCollection_.insert_one (ToBSON_ (v))) {
                    return bson_value_to_string_ (insert_one_result->inserted_id ());
                }
                Throw (RuntimeErrorException{"failed to add doc"});
            }
            virtual optional<Document::Document> GetDocument (const IDType& id, const optional<Projection>& projection) override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySynchronizedMutex_};
                bsoncxx::builder::basic::document                      filter_doc;
                filter_doc.append (kvp ("_id", bsoncxx::oid{id.AsUTF8<string> ()})); //kMongoID
                auto [mongoProjection, myProjection] = Parse_ (projection);
                // @todo support mongoProjection - {{a: 1, b:0}} etc...
                auto result = fCollection_.find_one (filter_doc.view ());
                if (result) {
                    auto rr = FromBSON_ (bsoncxx::document::view_or_value{*result});
                    if (myProjection) {
                        rr = myProjection->Apply (rr);
                    }
                    return rr;
                }
                return nullopt;
            }
            virtual Sequence<Document::Document> GetDocuments (const optional<Filter>& filter, const optional<Projection>& projection) override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySynchronizedMutex_};
                auto [mongoFilter, myFilter]         = Parse_ (filter);
                auto [mongoProjection, myProjection] = Parse_ (projection);
                //bsoncxx::builder::basic::document filter_doc;
                //filter_doc.append (kvp (kMongoID_, bsoncxx::oid{id.AsUTF8<string> ()}));
                //filter_doc.append (kvp (kMongoID_, [&] (sub_document subdoc) { subdoc.append(kvp ("$oid", id.AsUTF8<string> ())); }));
                Sequence<Document::Document> result;
                //auto                         cursor = fCollection_.find (filter_doc.view ());
                auto cursor = fCollection_.find (mongoFilter ? mongoFilter->view () : bsoncxx::builder::basic::document{}.view ());
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
            virtual void UpdateDocument (const IDType& id, const Document::Document& newV, const optional<Set<String>>& onlyTheseFields) override
            {
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
            virtual void DeleteDocument (const IDType& id) override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySynchronizedMutex_};
                bsoncxx::builder::basic::document                      filter_doc;
                filter_doc.append (kvp ("_id", bsoncxx::oid{id.AsUTF8<string> ()})); // kMongoID_
                auto result = fCollection_.delete_one (filter_doc.view ());
                if (result && result->deleted_count () == 0) {
                    Throw (RuntimeErrorException{"failed to delete doc"});
                }
            }
        };

        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fAssertExternallySynchronizedMutex_;
        mongocxx::client                                               fClient_;
        mongocxx::database                                             fDatabase_;

        ConnectionRep_ (const Connection::Options& options)
            : fClient_{mongocxx::uri{options.fConnectionString.AsUTF8<string> ()}}
            , fDatabase_{fClient_.database (options.fDatabase.AsUTF8<string> ())}
        {
            TraceContextBumper ctx{"Document::MongoDBClient::Connection::ConnectionRep_::CTOR"};
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
                // a specific error here - check for that - and throw others... no such database gets mapped to empty collection list
                return {};
            }
        }
        virtual void CreateCollection (const String& name) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySynchronizedMutex_};
            fDatabase_.create_collection (name.AsUTF8<string> ());
        }
        virtual void DropCollection (const String& name) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySynchronizedMutex_};
            fDatabase_.collection (name.AsUTF8<string> ()).drop ();
        }
        virtual Document::Collection::Ptr GetCollection (const String& name) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fAssertExternallySynchronizedMutex_};
            return Document::Collection::Ptr{make_shared<CollectionRep_> (*this, name)};
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
            return fClient_;
        }
    };
}

/*
 ********************************************************************************
 ********************* Document::MongoDBClient::Activator ***********************
 ********************************************************************************
 */
Document::MongoDBClient::Activator::Activator ()
    : fAllowReactivation_{false}
{
    Require (Debug::AppearsDuringMainLifetime ());
    if (sActivatorLiveCnt_.fetch_add (1) == 0) {
        if (not sMongoInstance_) {
            sMongoInstance_ = mongocxx::instance{};
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
