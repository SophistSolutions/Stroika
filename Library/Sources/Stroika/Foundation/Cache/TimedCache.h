/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
#include "../Debug/AssertExternallySynchronizedMutex.h"
#include "../Debug/Assertions.h"
#include "../Time/Duration.h"
#include "../Time/Realtime.h"

#include "Statistics.h"

/**
 *      \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *
 *      @todo   This class is logically a map. But you may want to have individual values with timed cache!
 *              Basically - KEY=RESULT? And then the arg to add/lookup don't take key? Maybe key is void?
 *
 *              That maybe best. Template specialization where KEY=void?
 *
 *              THEN - maybe reverse order of template params? VALUE/KEY - so then we can have KEY=void as default
 *              arg?
 *
 *              NOTE - I DID this already for CallerStalenessCache, so pretty easy, but for the case where KEY=void, you can
 *              really use either so LOW PRIORITY.
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
         * 
         *  \note This class was incompatibly changed in Stroika 3.0d1. It used to have a TRACK_READ_ACCESS parameter.
         *        Since Stroika 3.0d1, instead, if you wish to set that true, call Lookup (..., eTreatFoundThroughLookupAsRefreshed) instead
         *        of Lookup ()
         */
        template <typename KEY, typename VALUE, typename STRICT_INORDER_COMPARER = less<KEY>>
        struct DefaultTraits {
            using KeyType    = KEY;
            using ResultType = VALUE;

            using StatsType = Statistics::StatsType_DEFAULT;

            /**
             */
            using InOrderComparerType = STRICT_INORDER_COMPARER;
        };

        /**
         *  Flag to facilitate automatic cleanup of internal data structures as data tracked becomes uneeded.
         */
        enum class PurgeSpoiledDataFlagType {
            eAutomaticallyPurgeSpoiledData,
            eDontAutomaticallyPurgeSpoiledData
        };

        /**
         *  
         */
        enum class LookupMarksDataAsRefreshed {
            eTreatFoundThroughLookupAsRefreshed,
            eDontTreatFoundThroughLookupAsRefreshed
        };
    }

    /**
     *  \brief Keep track of a bunch of objects, each with an associated 'freshness' which meet a TimedCache-associated minimal reshness limit.
     * 
     *  We define 'fresheness' somewhat arbitrarily, but by default, this means since the item was added. However, the TimedCache
     *  also provides other apis to update the 'freshness' of a stored object, depending on application needs.
     *
     *  Keeps track of all items - indexed by Key - but throws away items which are any more
     *  stale than given by the staleness limit.
     *
     *  \note Comparison with LRUCache
     *        The main difference beweeen an LRUCache and TimedCache has to do with when an element is evicted from the Cache.
     *        With a TimedCache, its evicted only when its overly aged. With an LRUCache, its more random, and depends a
     *        bit on luck (when using hashing) and how recently an item was last accessed.
     *
     *  \note   Principal difference between CallerStalenessCache and TimedCache lies in where you specify the
     *          max-age for an item: with CallerStalenessCache, its specified on each lookup call (ie with the caller), and with
     *          TimedCache, the expiry is stored with each cached item.
     *
     *          Because of this, when you use either of these caches with a KEY=void (essentially to cache a single thing)
     *          they become indistinguishable.
     *
     *          N.B. the KEY=void functionality is NYI for TimedCache, so best to use CallerStalenessCache for that, at least for
     *          now.
     *
     *  \par Example Usage
     *      Use TimedCache to avoid needlessly redundant lookups
     *
     *      \code
     *          optional<String> ReverseDNSLookup_ (const InternetAddress& inetAddr)
     *          {
     *              const Time::Duration                                        kCacheTTL_{5min};
     *              static Cache::TimedCache<InternetAddress, optional<String>> sCache_{kCacheTTL_};
     *              return sCache_.LookupValue (inetAddr, [] (const InternetAddress& inetAddr) {
     *                  return DNS::kThe.ReverseLookup (inetAddr);
     *              });
     *          }
     *      \endcode
     *
     *  \par Example Usage
     *      Assume 'LookupDiskStats_' returns DiskSpaceUsageType, but its expensive, and the results change only slowly...
     *
     *      \code
     *          struct DiskSpaceUsageType {
     *              int size;
     *          };
     *          // do the actual lookup part which maybe slow
     *          auto LookupDiskStats_ ([[maybe_unused]] const String& filename) -> DiskSpaceUsageType { return DiskSpaceUsageType{33}; };
     *
     *          Cache::TimedCache<String, DiskSpaceUsageType> sDiskUsageCache_{5.0_duration};
     *
     *          // explicitly caller maintaining the cache
     *          optional<DiskSpaceUsageType> LookupDiskStats_Try1 (String diskName)
     *          {
     *              optional<DiskSpaceUsageType> o = sDiskUsageCache_.Lookup (diskName);    // maybe use eTreatFoundThroughLookupAsRefreshed depending on your application
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
     *              // maybe use eTreatFoundThroughLookupAsRefreshed depending on your application
     *              return sDiskUsageCache_.LookupValue (diskName,
     *                                              [](String diskName) -> DiskSpaceUsageType {
     *                                                  return LookupDiskStats_ (diskName);
     *                                              });
     *          }
     *
     *          // or still simpler
     *          DiskSpaceUsageType LookupDiskStats_Try3 (String diskName)
     *          {
     *              // maybe use eTreatFoundThroughLookupAsRefreshed depending on your application
     *              return sDiskUsageCache_.LookupValue (diskName, LookupDiskStats_);
     *          }
     *          void DoIt ()
     *          {
     *              // example usage
     *              EXPECT_TRUE (Memory::NullCoalesce (LookupDiskStats_Try1 ("xx")).size == 33);
     *              EXPECT_TRUE (LookupDiskStats_Try2 ("xx").size == 33);
     *              EXPECT_TRUE (LookupDiskStats_Try3 ("xx").size == 33);
     *          }
     *      \endcode
     *
     *  \note   Only calls to @Add, @Lookup (...,eTreatFoundThroughLookupAsRefreshed), and @LookupValue (on a cache miss when updating) update the
     *          lastRefreshed time.
     *
     *          For most use cases (when caching something) - the default behavior of only updating
     *          the last-access time on Add makes sense. But for the case where this class is used
     *          to OWN an object (see shared_ptr example below) - then treating a read asccess as a refresh can be helpful.
     * 
     *          Before Stroika 3.0d1, there was a template parameter which allowed treating Lookup () this way. But that
     *          has significant downsides (making lookup non-const which has threading implications). Instead now, we have
     *          non-const methods you can use todo this instead of Lookup, and then there is no need for special
     *          template paremeters, or non-cost Lookup.
     *
     *  \par Example Usage
     *      To use TimedCache<> to 'own' a set of objects (say a set caches where we are the only
     *      possible updater) - you can make the 'VALUE' type a shared_ptr<X>, and  use Lookup (...,eTreatFoundThroughLookupAsRefreshed) instead 
     *      of Lookup ().
     *
     *      In this example, there is a set of files on disk in a folder, which is complex to analyze
     *      but once analyzed, lots of calls come in at once to read (and maybe update) the set of files
     *      and once nobody has asked for a while, we throw that cache away, and rebuild it as needed.
     *
     *      This example ALSO shows how to wrap a cache object in 'Synchronized' for thread safety.
     *
     *      \code
     *          using ScanFolderKey_ = String;
     *          static constexpr Time::DurationSeconds kAgeForScanPersistenceCache_{5 * 60.0s};
     *          struct FolderDetails_ {
     *              int size; // ...info to cache about a folder
     *          };
     *          Synchronized<Cache::TimedCache<
     *              ScanFolderKey_,
     *              shared_ptr<FolderDetails_>,
     *              shared_ptr<FolderDetails_>>>
     *              sCachedScanFoldersDetails_{kAgeForScanPersistenceCache_};
     *
     *          shared_ptr<FolderDetails_> AccessFolder_ (const ScanFolderKey_& folder)
     *          {
     *              auto lockedCache = sCachedScanFoldersDetails_.rwget ();
     *              if (optional<shared_ptr<FolderDetails_>> o = lockedCache->Lookup (folder, eTreatFoundThroughLookupAsRefreshed)) {
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
     *              auto f1 = AccessFolder_ ("folder1"_k);
     *              auto f2 = AccessFolder_ ("folder2"_k);
     *              auto f1again = AccessFolder_ ("folder1");  // if you trace through the debug code you'll see this is a cache hit
     *          }
     *      \endcode
     *
     *  \note   This cache will keep using more and more memory until the cached items become
     *          out of date. For a cache that limits the max number of entries, use the @see LRUCache.
     *
     *  \note   This cache assumes one timeout for all items. To have timeouts vary by item,
     *          @see CallerStalenessCache.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     * 
     *  \note   we REQUIRE (without a way to enforce) - that the STATS object be internally synchronized, so that we can
     *          maintain statistics, without requiring the lookup method be non-const; this is only for tuning/debugging, anyhow...
     *
     *  @see SynchronizedTimedCache<> - for internally synchonized implementation
     *
     *  @see SyncrhonizedCallerStalenessCache
     *  @see CallerStalenessCache
     *  @see LRUCache
     */
    template <typename KEY, typename VALUE, typename TRAITS = TimedCacheSupport::DefaultTraits<KEY, VALUE>>
    class TimedCache {
    public:
        using TraitsType = TRAITS;

    public:
        static_assert (Common::IInOrderComparer<typename TraitsType::InOrderComparerType, KEY>,
                       "TraitsType::InOrderComparerType - comparer not valid IInOrderComparer- see "
                       "ComparisonRelationDeclaration<Common::ComparisonRelationType::eStrictInOrder, function<bool(T, T)>");

    public:
        using LookupMarksDataAsRefreshed = TimedCacheSupport::LookupMarksDataAsRefreshed;

    public:
        using PurgeSpoiledDataFlagType = TimedCacheSupport::PurgeSpoiledDataFlagType;

    public:
        /**
         */
        explicit TimedCache (const Time::Duration& minimumAllowedFreshness);
        TimedCache (TimedCache&&)      = default;
        TimedCache (const TimedCache&) = default;

    public:
        nonvirtual TimedCache& operator= (TimedCache&&)      = default;
        nonvirtual TimedCache& operator= (const TimedCache&) = default;

    public:
        /**
         *  When items are added to the timed cache, there is a universal (for the entire cache) minimum allowed freshness (how old item
         *  allowed to be before thrown away).
         */
        nonvirtual Time::Duration GetMinimumAllowedFreshness () const;

    public:
        /**
         *  @see GetMinimumAllowedFreshness ()
         */
        nonvirtual void SetMinimumAllowedFreshness (Time::Duration minimumAllowedFreshness);

    public:
        /**
         */
        struct CacheElement {
            KEY                    fKey;
            VALUE                  fValue;
            Time::TimePointSeconds fLastRefreshedAt;
        };

    public:
        /**
         *  \note This returns the non-expired elements of the current cache object.
         */
        nonvirtual Traversal::Iterable<CacheElement> Elements () const;

    public:
        /**
         *  \brief Returns the value associated with argument 'key', or nullopt, if its missing (missing same as expired). Can be used to retrieve lastRefreshedAt
         * 
         *  If lastRefreshedAt is provided, it is ignored, except if Lookup returns true, the value pointed to will contain the last time
         *  the data was refreshed.
         * 
         *  Occasionally, a caller might want to have the ACT of doing a lookup mark the item as fresh, in which case call
         *  Lookup (..., eTreatFoundThroughLookupAsRefreshed) instead.
         * 
         *  Occasionally, a caller might want to ASSURE it gets data, and just use the cached value if fresh enuf, and specify
         *  a lookup lambda to fetch the actual data if its not fresh, in which case call LookupValue ().
         *
         *  \note   Before Stroika 3.0d1, this used to support TraitsType::kTrackReadAccess, and if it was true did the same
         *          as the newer Lookup (..., eTreatFoundThroughLookupAsRefreshed)
         */
        nonvirtual optional<VALUE> Lookup (typename Configuration::ArgByValueType<KEY> key, Time::TimePointSeconds* lastRefreshedAt = nullptr) const;
        nonvirtual optional<VALUE> Lookup (typename Configuration::ArgByValueType<KEY> key, LookupMarksDataAsRefreshed successfulLookupRefreshesAcceesFlag);

    public:
        /**
         *  Usually one will use this as (cacheFiller overload):
         *      \code
         *          VALUE v = cache.Lookup (key, ts, [this] () -> VALUE {return this->realLookup(key); });
         *      \endcode
         *
         *  However, the method (Lookup) returing an optional is occasionally useful, if you don't want to fill the cache
         *  but just see if a value is present.
         *
         *  The overload with cacheFiller, will update the 'time stored' for the argument key if a new value is fetched.
         *
         *  \note   This function may update the TimedCache (which is why it is non-const).
         */
        nonvirtual VALUE LookupValue (typename Configuration::ArgByValueType<KEY>                          key,
                                      const function<VALUE (typename Configuration::ArgByValueType<KEY>)>& cacheFiller,
                                      LookupMarksDataAsRefreshed successfulLookupRefreshesAcceesFlag = LookupMarksDataAsRefreshed::eDontTreatFoundThroughLookupAsRefreshed,
                                      PurgeSpoiledDataFlagType purgeSpoiledData = PurgeSpoiledDataFlagType::eAutomaticallyPurgeSpoiledData);

    public:
        /**
         *  Updates/adds the given value associated with key, and updates the last-access date to now (or argument freshAsOf).
         * 
         *  The parameter PurgeSpoiledData defaults to eAutomaticallyPurgeSpoiledData; this allows the accumulated data
         *  to automatically be purged as it becomes irrelevant (@see PurgeSpoiledData). But for performance sake,
         *  callers may call Add (..., eDontAutomaticallyPurgeSpoiledData)
         */
        nonvirtual void Add (typename Configuration::ArgByValueType<KEY> key, typename Configuration::ArgByValueType<VALUE> result,
                             PurgeSpoiledDataFlagType purgeSpoiledData = PurgeSpoiledDataFlagType::eAutomaticallyPurgeSpoiledData);
        nonvirtual void Add (typename Configuration::ArgByValueType<KEY> key, typename Configuration::ArgByValueType<VALUE> result,
                             Time::TimePointSeconds freshAsOf);

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
         *  Not necessary to call - but can save memory.
         * 
         *  Can be triggered automatically (so not explicitly) by passing eAutomaticallyPurgeSpoiledData to Add ()
         */
        nonvirtual void PurgeSpoiledData ();

    public:
        [[deprecated ("Since Stroika v3.0d1, use PurgeSpoiledData or count on Add's purgeSpoiledData parameter)")]] nonvirtual void DoBookkeeping ()
        {
            PurgeSpoiledData ();
        }
        [[deprecated ("Since Stroika 3.0d1 use GetMinimumAllowedFreshness")]] Time::Duration GetTimeout () const
        {
            return GetMinimumAllowedFreshness ();
        }
        [[deprecated ("Since Stroika 3.0d1 use GetMinimumAllowedFreshness")]] void SetTimeout (Time::Duration timeout)
        {
            SetMinimumAllowedFreshness (timeout);
        }

    private:
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fAssertExternallySyncrhonized_;

    private:
        Time::DurationSeconds  fMinimumAllowedFreshness_;
        Time::TimePointSeconds fNextAutoClearAt_;

    private:
        nonvirtual void ClearIfNeeded_ ();
        nonvirtual void ClearOld_ ();

    private:
        struct MyResult_ {
            VALUE                  fResult;
            Time::TimePointSeconds fLastRefreshedAt{Time::GetTickCount ()};
        };

    private:
        using MyMapType_ = map<KEY, MyResult_, typename TRAITS::InOrderComparerType>;
        MyMapType_ fMap_;

    private:
        [[no_unique_address]] mutable typename TRAITS::StatsType fStats_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TimedCache.inl"

#endif /*_Stroika_Foundation_Cache_TimedCache_h_*/
