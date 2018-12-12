/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_TimedCache_h_
#define _Stroika_Foundation_Cache_TimedCache_h_ 1

#include "../StroikaPreComp.h"

#include <map>
#include <mutex>
#include <optional>

#include "../Common/Compare.h"
#include "../Configuration/Common.h"
#include "../Configuration/TypeHints.h"
#include "../Debug/AssertExternallySynchronizedLock.h"
#include "../Debug/Assertions.h"
#include "../Time/Realtime.h"

#include "Statistics.h"

/**
 *      \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *      @todo   fNextAutoclearAt is HORRIBLE mechnism to figure out if we need to walk list and
 *              clear. Use a time value (max age), and time last checked or something like that).
 *
 *      @todo   This class is logically a map. But you may want to have individual values with timed cache!
 *              Basically - KEY=RESULT? And then the arg to add/lookup don't take key? Maybe key is void?
 *
 *              That maybe best. Template specialization where KEY=void?
 *
 *              THEN - maybe reverse order of template params? VALUE/KEY - so then we can have KEY=void as default
 *              arg?
 *
 *      @todo   Improve Regression Tests And Docs (quite weak)
 *
 *      @todo   Use Concepts or other such constraint on T/ELEMENT declarations (and docs)
 *
 *      @todo   Perhaps use Stroika Mapping<> instead of std::map<> - and in that way - we can use aribtrary externally
 *              specified map impl - so can use HASHING or BTREE, based on passed in arg. So we don't ahve problem with
 *              creating the default, specify default type to create in the TRAITS object (so for example, if using Hash,
 *              we don't force having operator< for BTREE map).
 *
 *      @todo   add bool option to TRAITS for 'accessDoesBookkeeping' - if false, then this ASSUMES/REQUIRES
 *              caller respponsability to arragen for periodic call of bookkeeping task.
 *
 *              Maybe have ENUM for bookkeeping strategy - caller repsonsabilty, on access, or threaded, wtih
 *              on access what we do now (and probably the default since seems lowest overhead).
 *
 *  Implementation Note:
 *
 *      This module uses stl:map<> instead of a Stroika Mapping since we are comfortable with
 *      the current implementation using btree's, and to avoid any dependencies between
 *      Caching and Containers. We may want to re-think that, and just  use Mapping here.
 */

namespace Stroika::Foundation::Cache {

    namespace TimedCacheSupport {
        /**
         * The DefaultTraits<> is a simple default traits implementation for building an TimedCache<>.
         */
        template <typename KEY, typename VALUE, typename STRICT_INORDER_COMPARER = less<KEY>, bool TRACK_READ_ACCESS = false>
        struct DefaultTraits {
            using KeyType    = KEY;
            using ResultType = VALUE;

            using StatsType = Statistics::StatsType_DEFAULT;

            /**
             * This may make sense to be true for things where you are combining a staleness of data cache with an LRU-style cache
             * - to track objects where you want to keep the most recent around. This is like LRUCache + explicit timeout.
             */
            static constexpr bool kTrackReadAccess = TRACK_READ_ACCESS;

            /**
             */
            using InOrderComparerType = STRICT_INORDER_COMPARER;
        };
    }

    /**
     *  Keeps track of all items - indexed by Key - but throws away items which are any more
     *  stale than given by the TIMEOUT. Staleness is defined as time since item was added.
     *
     *  \note   Note - this class doesn't employ a thread to throw away old items, so if you count on that
     *          happening (e.g. because the VALUE object DTOR has a side-effect like closing a file), then
     *          you may call DoBookkeeping () peridocially.
     *
     *          This does check the staleness on lookup however, to assure proper staleness semantics.
     *
     *  \par Example Usage
     *      Use TimedCache to avoid needlessly redundant lookups
     *
     *      Assume 'LookupDiskStats_' returns DiskSpaceUsageType, but its expensive, and the results change only slowly...
     *
     *      \code
     *          struct DiskSpaceUsageType {
     *              int size;
     *          };
     *          // do the actual lookup part which maybe slow
     *          auto LookupDiskStats_ ([[maybe_unused]] const String& filename) -> DiskSpaceUsageType { return DiskSpaceUsageType{33}; };
     *
     *          Cache::TimedCache<String, DiskSpaceUsageType> sDiskUsageCache_{5.0};
     *
     *          // explicitly caller maintaining the cache
     *          optional<DiskSpaceUsageType> LookupDiskStats_Try1 (String diskName)
     *          {
     *              optional<DiskSpaceUsageType> o = sDiskUsageCache_.Lookup (diskName);
     *              if (not o.has_value ()) {
     *                  o = LookupDiskStats_ (diskName);
     *                  if (o) {
     *                      sDiskUsageCache_.Add (diskName, *o);
     *                  }
     *              }
     *              return o;
     *          }
     *
     *          // more automatic maintainance of that update pattern
     *          DiskSpaceUsageType LookupDiskStats_Try2 (String diskName)
     *          {
     *              return sDiskUsageCache_.Lookup (diskName,
     *                                              [](String diskName) -> DiskSpaceUsageType {
     *                                                  return LookupDiskStats_ (diskName);
     *                                              });
     *          }
     *
     *          // or still simpler
     *          DiskSpaceUsageType LookupDiskStats_Try3 (String diskName)
     *          {
     *              return sDiskUsageCache_.Lookup (diskName, LookupDiskStats_);
     *          }
     *          void DoIt ()
     *          {
     *              // example usage
     *              VerifyTestResult (Memory::ValueOrDefault (LookupDiskStats_Try1 (L"xx")).size == 33);
     *              VerifyTestResult (LookupDiskStats_Try2 (L"xx").size == 33);
     *              VerifyTestResult (LookupDiskStats_Try3 (L"xx").size == 33);
     *          }
     *      \endcode
     *
     *  \note   Only calls to @Add cause the time (used for throwing away old items) to be updated,
     *          unless you specify kTrackReadAccess in the TRAITS object. In that case, Lookup OR
     *          Add () causes the last-accessed time to be updated.
     *
     *          For most use cases (when caching something) - the default behavior of only updating
     *          the last-access time on Add makes sense. But for the case where this class is used
     *          to OWN an object (see shared_ptr example below) - then specifying kTrackReadAccess
     *          true can be helpful.
     *
     *
     *  \par Example Usage
     *      To use TimedCache<> to 'own' a set of objects (say a set caches where we are the only
     *      possible updater) - you can make the 'VALUE' type a shared_ptr<X>, and specify
     *      kTrackReadAccess = true through the TRAITS object.
     *
     *      In this example, there is a set of files on disk in a folder, which is complex to analyze
     *      but once analyzed, lots of calls come in at once to read (and maybe update) the set of files
     *      and once nobody has asked for a while, we throw that cache away, and rebuild it as needed.
     *
     *      This example ALSO shows how to wrap a cache object in 'Syncrhonized' for thread safety.
     *
     *      \code
     *          using ScanFolderKey_ = String;
     *          static constexpr DurationSecondsType kAgeForScanPersistenceCache_{5 * 60.0};
     *          struct FolderDetails_ {
     *              int size; // ...info to cache about a folder
     *          };
     *          Synchronized<Cache::TimedCache<
     *              ScanFolderKey_,
     *              shared_ptr<FolderDetails_>,
     *              TimedCacheSupport::DefaultTraits<ScanFolderKey_, shared_ptr<FolderDetails_>, less<ScanFolderKey_>, true>>>
     *              sCachedScanFoldersDetails_{kAgeForScanPersistenceCache_};
     *
     *          shared_ptr<FolderDetails_> AccessFolder_ (const ScanFolderKey_& folder)
     *          {
     *              auto lockedCache = sCachedScanFoldersDetails_.rwget ();
     *              if (optional<shared_ptr<FolderDetails_>> o = lockedCache->Lookup (folder)) {
     *                  return *o;
     *              }
     *              else {
     *                  shared_ptr<FolderDetails_> fd = make_shared<FolderDetails_> (); // and fill in default values looking at disk
     *                  lockedCache->Add (folder, fd);
     *                  return fd;
     *              }
     *          }
     *
     *          void DoIt ()
     *          {
     *              auto f1 = AccessFolder_ (L"folder1");
     *              auto f2 = AccessFolder_ (L"folder2");
     *              auto f1again = AccessFolder_ (L"folder1");  // if you trace through the debug code you'll see this is a cache hit
     *          }
     *      \endcode
     *
     *  @todo   ANOTHER EXAMPLE - USE DNS CACHE... - or current use for LDAP lookups
     *
     *  \note   This cache will keep using more and more memory until the cached items become
     *          out of date. For a cache that limits the max number of entries, use the @see LRUCache.
     *
     *  \note   This cache assumes one timeout for all items. To have timeouts vary by item,
     *          @see CallerStatenessCache.
     *
     *  \note   \em Thread-Safety   <a href="thread_safety.html#ExternallySynchronized">ExternallySynchronized</a>
     *
     *  @see SyncrhonizedTimedCache<> - for internally ynchonized implementation
     *
     *  \note   Implementation Note: inherit from TRAITS::StatsType to take advantage of zero-sized base object rule.
     *
     *  @see CallerStatenessCache
     *  @see LRUCache
     */
    template <typename KEY, typename VALUE, typename TRAITS = TimedCacheSupport::DefaultTraits<KEY, VALUE>>
    class TimedCache : private Debug::AssertExternallySynchronizedLock, private TRAITS::StatsType {
    public:
        using TraitsType = TRAITS;

    public:
        static_assert (Common::IsStrictInOrderComparer<typename TraitsType::InOrderComparerType> (), "TraitsType::InOrderComparerType - comparer not valid IsStrictInOrderComparer- see ComparisonRelationDeclaration<Common::ComparisonRelationType::eStrictInOrder, function<bool(T, T)>");

    public:
        /**
         */
        TimedCache (Time::DurationSecondsType timeoutInSeconds);
        TimedCache (const TimedCache&) = default;

    public:
        nonvirtual TimedCache& operator= (const TimedCache&) = default;

    public:
        /**
         */
        nonvirtual void SetTimeout (Time::DurationSecondsType timeoutInSeconds);

    public:
        /**
         *  Usually one will use this as (cacheFiller overload):
         *      \code
         *          VALUE v = cache.Lookup (key, ts, [this] () -> VALUE {return this->realLookup(key); });
         *      \endcode
         *
         *  However, the overload returing an optional is occasionally useful, if you don't want to fill the cache
         *  but just see if a value is present.
         *
         *  The overload with cacheFiller, will update the 'time stored' for the argument key if a new value is fetched.
         *
         *  \note   if TraitsType::kTrackReadAccess is true (defaults false), this will also update the last-accessed date
         */
        nonvirtual optional<VALUE> Lookup (typename Configuration::ArgByValueType<KEY> key);
        nonvirtual VALUE Lookup (typename Configuration::ArgByValueType<KEY> key, const function<VALUE (typename Configuration::ArgByValueType<KEY>)>& cacheFiller);

    public:
        /**
         *  Updates/adds the given value associated with key, and updates the last-access date to now.
         */
        nonvirtual void Add (typename Configuration::ArgByValueType<KEY> key, typename Configuration::ArgByValueType<VALUE> result);

    public:
        /**
         */
        nonvirtual void Remove (typename Configuration::ArgByValueType<KEY> key);

    public:
        /**
         *  Remove everything from the cache
         */
        nonvirtual void clear ();

    public:
        /**
         *  May be called occasionally to free resources used by cached items that are out of date.
         *  Not necessary to call, as done internally during access.
         */
        nonvirtual void DoBookkeeping (); // optional - need not be called

    private:
        Time::DurationSecondsType fTimeout_;
        Time::DurationSecondsType fNextAutoClearAt_;

    private:
        nonvirtual void ClearIfNeeded_ ();
        nonvirtual void ClearOld_ ();

    private:
        struct MyResult_ {
            MyResult_ (const VALUE& r)
                : fResult (r)
                , fLastAccessedAt (Time::GetTickCount ())
            {
            }
            VALUE                     fResult;
            Time::DurationSecondsType fLastAccessedAt;
        };

    private:
        using MyMapType_ = map<KEY, MyResult_, typename TRAITS::InOrderComparerType>;
        MyMapType_ fMap_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TimedCache.inl"

#endif /*_Stroika_Foundation_Cache_TimedCache_h_*/
