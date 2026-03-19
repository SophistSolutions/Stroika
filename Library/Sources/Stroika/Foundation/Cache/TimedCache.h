/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_TimedCache_h_
#define _Stroika_Foundation_Cache_TimedCache_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <map>
#include <mutex>
#include <optional>

#include "Stroika/Foundation/Cache/Statistics.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Common/TypeHints.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Time/Duration.h"
#include "Stroika/Foundation/Time/Realtime.h"

/**
 *      \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
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

    /**
     * TimedCacheSupport mostly for defining TRAITS object that configures the cache behavior.
     */
    namespace TimedCacheSupport {

        using Execution::InternallySynchronized;

        /**
         * @brief @todo maybe allow void KEY - but some work todo this!
         * 
         */
        // template <typename T>
        // concept IKey = same_as<T,void> or copyable<T>;

        /**
         * @brief any copyable type can use used as the value, with 'void' being a special sentinal type, used to indicate we are just caching presence/absense of the KEY in the cache (and its expiration date).
         */
        template <typename T>
        concept IValue = same_as<T, void> or copyable<T>;

        /**
         * @brief  see TimedCache<>::TraitsType::kAutomaticPurgeFrequency - disable automatic purging
         * 
         *  \note would be nice to declare as of type Time::DurationSeconds, but then won't work as template parameter
         */
        constexpr float kNoAutomaticPurgeSentinal = -1.0f;

        /**
         * @brief  see TimedCache<>::TraitsType::kAutomaticPurgeFrequency - default to purging every 30 seconds
         * 
         *  \note would be nice to declare as of type Time::DurationSeconds, but then won't work as template parameter
         */
#if qCompilerAndStdLib_FloatNonTypeTemplateArgument_Buggy
        constexpr int kDefaultAutomaticPurgeFrequency = 30;
#else
        constexpr float kDefaultAutomaticPurgeFrequency = 30.0f;
#endif

        /**
         * @brief Check if argument TRAITS is a valid TRAITS object for TimedCache<>
         * 
         *  \note   ONE of (but for now not both) - kTrackFreshness or kTrackExpiration
         *  \note   kTrackExpiration not compatible with kAutomaticallyMarkDataAsRefreshedEachTimeAccessed
         */
        template <typename TRAITS, typename KEY, typename VALUE>
        concept ITraits = copyable<KEY> and IValue<VALUE> and
                          requires (TRAITS) {
                              typename TRAITS::KeyType;
                              typename TRAITS::ResultType;
                              typename TRAITS::StatsType;
                              { TRAITS::kInternallySynchronized } -> convertible_to<InternallySynchronized>;
                              { TRAITS::kAutomaticPurgeFrequency } -> convertible_to<Time::DurationSeconds>;
                              { TRAITS::kAutomaticallyMarkDataAsRefreshedEachTimeAccessed } -> convertible_to<bool>;
                              { TRAITS::kTrackFreshness } -> convertible_to<bool>;
                              { TRAITS::kTrackExpiration } -> convertible_to<bool>;
                          } and same_as<typename TRAITS::KeyType, KEY> and same_as<typename TRAITS::ResultType, VALUE> and
                          TRAITS::kTrackFreshness != TRAITS::kTrackExpiration and
                          (not TRAITS::kAutomaticallyMarkDataAsRefreshedEachTimeAccessed or not TRAITS::kTrackExpiration) and
                          Common::IInOrderComparer<typename TRAITS::InOrderComparerType, typename TRAITS::KeyType> and
                          Cache::Statistics::IStatsType<typename TRAITS::StatsType>;

        /**
         * The DefaultTraits<> is a simple default traits implementation for building an TimedCache<>.
         * 
         *  \note This class was incompatibly changed in Stroika 3.0d1. It used to have a TRACK_READ_ACCESS parameter.
         *        Since Stroika 3.0d1, instead, if you wish to set that true, call Lookup (..., eTreatFoundThroughLookupAsRefreshed) instead
         *        of Lookup ()
         * 
         *  \note this class was incompatibly changed in Stroika 3.0d23. It used to have the STRICT_INORDER_COPARER as third arugment
         *        but InternallySynchronized added as new third pushing comarer to fourth.
         * 
         *  \see ITraits<> above
         */
        template <typename KEY, IValue VALUE, InternallySynchronized INTERNALLY_SYNCHRONIZED = InternallySynchronized::eNotKnownInternallySynchronized,
                  Common::IInOrderComparer<KEY> STRICT_INORDER_COMPARER = less<KEY>, bool TRACK_FRESHNESS = true,
                  bool TRACK_EXPIRATION = false, Cache::Statistics::IStatsType STATS_TYPE = Statistics::StatsType_DEFAULT,
#if qCompilerAndStdLib_FloatNonTypeTemplateArgument_Buggy
                  int
#else
                  float
#endif
                       AUTOMATIC_PURGE_FREQUENCY_SECONDS                   = kDefaultAutomaticPurgeFrequency,
                  bool AUTO_MARK_DATA_AS_REFRESHED_ON_EACH_WRITABLE_ACCESS = false>
        struct ExplicitTraits {
            using KeyType    = KEY;
            using ResultType = VALUE;

            /**
             */
            static constexpr inline InternallySynchronized kInternallySynchronized{INTERNALLY_SYNCHRONIZED};

            /**
             * SO FAR ALL WE HAVE IMPLEMENTED
             * @brief freshness means when last added/updated (or if kAutomaticallyMarkDataAsRefreshedEachTimeAccessed) then last accessed too)
             */
            static constexpr inline bool kTrackFreshness{TRACK_FRESHNESS};

            /**
             * SO FAR NOT IMPLEMENTED
             * @brief Track on a per-item when it expires. If not tracked, we use expiresAt as whenAdded + minFreshness
             */
            static constexpr inline bool kTrackExpiration{TRACK_EXPIRATION};

            /**
             */
            using InOrderComparerType = STRICT_INORDER_COMPARER;

            /**
             * @brief Internally synchronized 'Stats' collector type (Cache::Statistics::IStatsType). Often null stats collector.
             */
            using StatsType = STATS_TYPE;

            /**
             *  How often TimedCache-modifying operations will automatically trigger a call to PurgeExpiredData ()
             * 
             *  This defaults to kDefaultAutomaticPurgeFrequency (but can be set to NEVER (kNoAutomaticPurgeSentinal)).
             * 
             *  \note - NOT triggered asynchronously, but from modifying APIs, like Add, or non-const Lookup()
             */
            static constexpr Time::DurationSeconds kAutomaticPurgeFrequency{AUTOMATIC_PURGE_FREQUENCY_SECONDS};

            /*
             *  This is useful for behavior like an LRU cache, where you express INTEREST in an item by using it.
             *  Use this if the data doesn't truely expire, but you want to keep intresting / recently used data around (though you maybe should
             *  just use LRUCache in that case).
             * 
             *  This is off by default.
             * 
             *  \note this only applies to NON-CONST methods, like the non-const Lookup() overload, and LookupValue() methods.
             * 
             *  \note Before Stroika v3.0d23, this was expressed via the optional argument to Lookup/LookupValue of type 
             *        LookupMarksDataAsRefreshed  (value eTreatFoundThroughLookupAsRefreshed).
             *  \note   Before Stroika 3.0d1, this used to support TraitsType::kTrackReadAccess, and if it was true did the same
             *          as the newer Lookup (..., eTreatFoundThroughLookupAsRefreshed)
             */
            static constexpr inline bool kAutomaticallyMarkDataAsRefreshedEachTimeAccessed = AUTO_MARK_DATA_AS_REFRESHED_ON_EACH_WRITABLE_ACCESS;
        };

        /**
         */
        template <typename KEY, IValue VALUE>
        using DefaultTraits = ExplicitTraits<KEY, VALUE>;

        /**
         * @brief InternallySynchronizedTraits same as argument traits, but resetting the kInternallySynchronized to eInternallySynchronized
         * 
         * @tparam TRAITS 
         */
        template <typename TRAITS>
            requires (TimedCacheSupport::ITraits<TRAITS, typename TRAITS::KeyType, typename TRAITS::ResultType>)
        struct InternallySynchronizedTraits : TRAITS {
            static constexpr inline Execution::InternallySynchronized kInternallySynchronized{Execution::InternallySynchronized::eInternallySynchronized};
        };

        /**
         * @brief take argument TRAITS, and set to track-expires-at mode.
         * 
         * @tparam TRAITS 
         */
        template <typename TRAITS>
            requires (TimedCacheSupport::ITraits<TRAITS, typename TRAITS::KeyType, typename TRAITS::ResultType>)
        struct TrackExpirationTraits : TRAITS {
            static constexpr inline bool kTrackFreshness{false};
            static constexpr inline bool kTrackExpiration{true};
            static constexpr inline bool kAutomaticallyMarkDataAsRefreshedEachTimeAccessed{false}; // doesn't work with expiration based cache
        };

        enum class [[deprecated ("Since Stroika 3.0d23 use TRAITS kAutomaticPurgeFrequency")]] PurgeSpoiledDataFlagType {
            eAutomaticallyPurgeSpoiledData,
            eDontAutomaticallyPurgeSpoiledData
        };
        enum class [[deprecated (
            "Since Stroika 3.0d23 use TRAITS kAutomaticallyMarkDataAsRefreshedEachTimeAccessed")]] LookupMarksDataAsRefreshed {
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
     *      \code
     *          optional<String> ReverseDNSLookup_ (const InternetAddress& inetAddr)
     *          {
     *              const Time::Duration                                        kCacheTTL_{5min};
     *              static Cache::TimedCache<InternetAddress, optional<String>> sCache_{kCacheTTL_};    // not threadsafe (not internally synchronized) by default - but checked with Debug::AssertExternallySyncrhonizedMutex
     *              return sCache_.LookupValue (inetAddr, [] (const InternetAddress& inetAddr) {
     *                  return DNS::kThe.ReverseLookup (inetAddr);
     *              });
     *          }
     *      \endcode
     *
     *  \par Example Usage
     *      Same as above, but adding internal syncrhonization (automatic thread safety)
     *      \code
     *          optional<String> ReverseDNSLookup_ThreadSafe_ (const InternetAddress& inetAddr)
     *          {
     *              const Time::Duration                                        kCacheTTL_{5min};
     *              struct CACHE_TRAITS_ : Cache::TimedCacheSupport::DefaultTraits<InternetAddress, optional<String>> {
     *                  static constexpr inline InternallySynchronized kInternallySynchronized{InternallySynchronized::eInternallySynchronized};
     *              };
     *              static Cache::TimedCache<InternetAddress, optional<String>, CACHE_TRAITS_> sCache_{kCacheTTL_};    // NOW the cache is threadsafe
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
     *          OR
     *              struct CACHE_TRAITS_ : Cache::TimedCacheSupport::DefaultTraits<String, DiskSpaceUsageType> {
     *                  static constexpr inline bool kAutomaticallyMarkDataAsRefreshedEachTimeAccessed = true; // to treat lookups as 'refreshing' the cache like LRU
     *              };
     *          Cache::TimedCache<String, DiskSpaceUsageType, CACHE_TRAITS_> sDiskUsageCache_{5.0_duration};
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
     *                  shared_ptr<FolderDetails_> fd = MakeSharedPtr<FolderDetails_> (); // and fill in default values looking at disk
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
     *  \par Example Usage
     *      Use 'void' as second type argument - just storing the first KEY value (like keyedCollection or just presence/absence test - like accessKeys)
     *
     *      \code
     *          Cache::TimedCache<Characters::String, void> validAccessKeyCache{30s};
     *          EXPECT_TRUE (validAccessKeyCache.GetExpiration ("fred") == nullopt);
     *          validAccessKeyCache.Add ("fred");
     *          EXPECT_TRUE (validAccessKeyCache.GetExpiration ("fred"));
     *      \endcode
     *
     *  \note   This cache will keep using more and more memory until the cached items become
     *          out of date. For a cache that limits the max number of entries, use the @see LRUCache.
     *
     *  \note   This cache assumes one timeout for all items. To have timeouts vary by item,
     *          @see CallerStalenessCache.
     *
     *  \note Satisfies Concepts:
     *      o   moveable<TimedCache<KEY,VALUE>>
     *      o   copyable<TimedCache<KEY,VALUE>>
     *
     *  \note   \em Thread-Safety   if (TRAITS::kInternallySynchronized == eInternallySynchronized)           <a href='#Internally-Synchronized-Thread-Safety'>Internally-Synchronized-Thread-Safety</a>
     *  \note   \em Thread-Safety   if (TRAITS::kInternallySynchronized == eNotKnownInternallySynchronized)   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     * 
     *  \note   we REQUIRE (without a way to enforce) - that the STATS object be internally synchronized, so that we can
     *          maintain statistics, without requiring the lookup method be non-const; this is only for tuning/debugging, anyhow...
     *
     *  @see CallerStalenessCache
     *  @see LRUCache
     */
    template <copyable KEY, TimedCacheSupport::IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS = TimedCacheSupport::DefaultTraits<KEY, VALUE>>
    class TimedCache {
    public:
        using TraitsType = TRAITS;

    public:
        /**
         * Note that TimedCache is copyable and moveable by value.
         */
        explicit TimedCache (const Time::Duration& minimumAllowedFreshness);
        TimedCache (TimedCache&& src) noexcept;
        TimedCache (const TimedCache& src);

    public:
        nonvirtual TimedCache& operator= (TimedCache&& rhs) noexcept;
        nonvirtual TimedCache& operator= (const TimedCache& rhs);

    public:
        /**
         *  When items are added to the timed cache, there is a universal (for the entire cache) minimum allowed freshness (how old item
         *  allowed to be before thrown away).
         * 
         *  \alias Note - 'allowed freshness' == 'time to live' == 'TTL'.
         * 
         *  So an item added 30 seconds ago (freshness = 30s), would be thrown away/not returned as part of the cache
         *  if the minimum allowed freshness was 5 seconds.
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
            KEY   fKey;
            VALUE fValue;

            qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE conditional_t<TRAITS::kTrackFreshness, Time::TimePointSeconds, Common::Empty> fLastRefreshedAt;
            qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE conditional_t<TRAITS::kTrackExpiration, Time::TimePointSeconds, Common::Empty> fExpiresAt;
        };

    public:
        /**
         *  \note This returns the non-expired elements of the current cache object.
         */
        nonvirtual Traversal::Iterable<CacheElement> Elements () const;

    public:
        /**
         *  \note This returns the non-expired keys of the current cache object.
         */
        nonvirtual Traversal::Iterable<KEY> Keys () const;

    public:
        /**
         *  \brief Returns the value associated with argument 'key', or nullopt, if its missing (missing same as expired). Can be used to retrieve lastRefreshedAt
         * 
         *  If lastRefreshedAt is provided, it is ignored, except if Lookup returns true, the value pointed to will contain the last time
         *  the data was refreshed.
         *  If expiresAt is provided, it is ignored, except if Lookup returns true, the value pointed to will contain expiresAt value.
         * 
         *  \note that the non-const overload of Lookup respects TRAITS::kAutomaticallyMarkDataAsRefreshedEachTimeAccessed, and will
         *        auto-refresh the item (similar to LRUCache) if found.
         * 
         *  Occasionally, a caller might want to ASSURE it gets data, and just use the cached value if fresh enuf, and specify
         *  a lookup lambda to fetch the actual data if its not fresh, in which case call LookupValue ().
         *
         *  \note difference between const and non-const overloads is just that some extra bookkeeping can be done and kAutomaticallyMarkDataAsRefreshedEachTimeAccessed respected in non-const overload.
         */
        nonvirtual optional<VALUE> Lookup (typename Common::ArgByValueType<KEY> key, Time::TimePointSeconds* lastRefreshedAt = nullptr) const
            requires (TRAITS::kTrackFreshness);
        nonvirtual optional<VALUE> Lookup (typename Common::ArgByValueType<KEY> key, Time::TimePointSeconds* lastRefreshedAt = nullptr)
            requires (TRAITS::kTrackFreshness);
        nonvirtual optional<VALUE> Lookup (typename Common::ArgByValueType<KEY> key, Time::TimePointSeconds* expiresAt = nullptr) const
            requires (TRAITS::kTrackExpiration);
        nonvirtual optional<VALUE> Lookup (typename Common::ArgByValueType<KEY> key, Time::TimePointSeconds* expiresAt = nullptr)
            requires (TRAITS::kTrackExpiration);

    public:
        /**
         * @brief Get the Expiration of object or nullopt of item expired/not in cache
         */
        nonvirtual optional<Time::TimePointSeconds> GetExpiration (typename Common::ArgByValueType<KEY> key) const;

    public:
        /**
         *  Usually one will use this as:
         *      \code
         *          VALUE v = cache.LookupValue (key, ts, [this] () -> VALUE {return this->realLookup(key); });
         *      \endcode
         * 
         *  This operates as if:
         *      if (auto ov = Lookup (key)) {
         *          return *ov;
         *      }
         *      else {
         *          VALUE r = cacheFiller();
         *          Add (r);
         *          return r;
         *      }
         *
         *  \note   This function may update the TimedCache (which is why it is non-const).
         */
        template <Common::invocable_r<VALUE, KEY> CACHE_FILLTER_T>
        nonvirtual VALUE LookupValue (typename Common::ArgByValueType<KEY> key, CACHE_FILLTER_T&& cacheFiller);

    public:
        /**
         *  Updates/adds the given value associated with key.
         *      if TRAITS::kTrackFreshness (the default)
         *          o   The new items freshness is GetTickCount (), or the value given as argument
         *      if TRAITS::kTrackExpiration
         *          o   The new item's expiration is either given by expiresAt or now+ttl, or defaults to
         *              GetMinimumFreshness()
         */
        nonvirtual void Add (typename Common::ArgByValueType<KEY> key)
            requires (same_as<VALUE, void>);
        template <typename V = VALUE>
            requires (not same_as<V, void>)
        nonvirtual void Add (typename Common::ArgByValueType<KEY> key, typename Common::ArgByValueType<V> result);
        template <typename V = VALUE>
            requires (not same_as<V, void>)
        nonvirtual void Add (typename Common::ArgByValueType<KEY> key, typename Common::ArgByValueType<V> result, Time::TimePointSeconds freshAsOf)
            requires (TRAITS::kTrackFreshness);
        template <typename V = VALUE>
            requires (not same_as<V, void>)
        nonvirtual void Add (typename Common::ArgByValueType<KEY> key, typename Common::ArgByValueType<V> result, Time::TimePointSeconds expiresAt)
            requires (TRAITS::kTrackExpiration);
        template <typename V = VALUE>
            requires (not same_as<V, void>)
        nonvirtual void Add (typename Common::ArgByValueType<KEY> key, typename Common::ArgByValueType<V> result, Time::DurationSeconds ttl)
            requires (TRAITS::kTrackExpiration);

    public:
        /**
         */
        nonvirtual void Remove (typename Common::ArgByValueType<KEY> key);

    public:
        /**
         */
        template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (predicate<typename TimedCache<KEY, VALUE, TRAITS>::CacheElement>) PREDICATE>
        nonvirtual void RemoveAll (PREDICATE&& p);

    public:
        /**
         * @brief like Mapping<>::RetainAll () - throw away all elements not in items2Keep
         */
        template <Traversal::IIterableOfTo<KEY> ITERABLE_OF_KEY_TYPE>
        nonvirtual void RetainAll (const ITERABLE_OF_KEY_TYPE& items2Keep);

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
         *  Can be triggered automatically - see TRAITS::kAutomaticPurgeFrequency
         */
        nonvirtual void PurgeExpiredData ();

    public:
        /**
         */
        nonvirtual typename TRAITS::StatsType GetStats () const;

    private:
        nonvirtual void AutomaticallyPurgeExpiredDataSometimes_ ();

    public:
        DISABLE_COMPILER_MSC_WARNING_START (4996);
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
        using LookupMarksDataAsRefreshed
            [[deprecated ("Since Stroika 3.0d23 use TRAITS kAutomaticallyMarkDataAsRefreshedEachTimeAccessed")]] =
                TimedCacheSupport::LookupMarksDataAsRefreshed;
        using PurgeSpoiledDataFlagType
            [[deprecated ("Since Stroika v3.0d23 - use TRAITS::kAutomaticallyMarkDataAsRefreshedEachTimeAccessed instead")]] =
                TimedCacheSupport::PurgeSpoiledDataFlagType;
        [[deprecated ("Since Stroika v3.0d1, use PurgeExpiredData or count on Add's purgeSpoiledData parameter)")]] nonvirtual void DoBookkeeping ()
        {
            PurgeExpiredData ();
        }
        [[deprecated ("Since Stroika 3.0d1 use GetMinimumAllowedFreshness")]] Time::Duration GetTimeout () const
        {
            return GetMinimumAllowedFreshness ();
        }
        [[deprecated ("Since Stroika 3.0d1 use GetMinimumAllowedFreshness")]] void SetTimeout (Time::Duration timeout)
        {
            SetMinimumAllowedFreshness (timeout);
        }
        [[deprecated ("Since Stroika v3.0d23 - use TRAITS::kAutomaticallyMarkDataAsRefreshedEachTimeAccessed instead")]]
        nonvirtual optional<VALUE> Lookup (typename Common::ArgByValueType<KEY> key, LookupMarksDataAsRefreshed successfulLookupRefreshesAcceesFlag)
        {
            // DEPRECATED API
            scoped_lock                   critSec{fMaybeMutex_};
            typename MyMapType_::iterator i   = fMap_.find (key);
            Time::TimePointSeconds        now = Time::GetTickCount ();
            if (i == fMap_.end ()) {
                fStats_.IncrementMisses ();
                return nullopt;
            }
            else {
                Time::TimePointSeconds lastAccessThreshold = now - fMinimumAllowedFreshness_;
                if (i->second.fLastRefreshedAt < lastAccessThreshold) {
                    /**
                     *  Before Stroika 3.0d1, we used to remove the entry from the list (an optimization). But
                     * that required Lookup to be non-const (with synchronization in mind probably a pessimization).
                     * So instead, count on PurgeUnusedData being called automatically on future adds,
                     * explicit user calls to purge unused data.
                     *
                     *      i = fMap_.erase (i);
                     */
                    fStats_.IncrementMisses ();
                    return nullopt;
                }
                if (successfulLookupRefreshesAcceesFlag == LookupMarksDataAsRefreshed::eTreatFoundThroughLookupAsRefreshed) {
                    i->second.fLastRefreshedAt = Time::GetTickCount ();
                }
                fStats_.IncrementHits ();
                return i->second.fResult;
            }
        }
        [[deprecated ("Since Stroika v3.0d23 - use kAutomaticPurgeFrequency in TRAITS instead of PurgeSpoiledDataFlagType, and "
                      "kAutomaticallyMarkDataAsRefreshedEachTimeAccessed in TRAITS instead of LookupMarksDataAsRefreshed")]]
        nonvirtual VALUE LookupValue (typename Common::ArgByValueType<KEY> key, const function<VALUE (typename Common::ArgByValueType<KEY>)>& cacheFiller,
                                      LookupMarksDataAsRefreshed successfulLookupRefreshesAcceesFlag,
                                      PurgeSpoiledDataFlagType purgeSpoiledData = PurgeSpoiledDataFlagType::eAutomaticallyPurgeSpoiledData)
        {
            auto&& readLock = shared_lock{fMaybeMutex_}; // try shared_lock for case where present, and then lose it if we need to update object
            if (optional<VALUE> o = Lookup (key, successfulLookupRefreshesAcceesFlag)) {
                return *o;
            }
            else {
                readLock.unlock ();
                constexpr bool kHoldWriteLockDuringCacheFill = false;
                if constexpr (kHoldWriteLockDuringCacheFill) {
                    [[maybe_unused]] auto&& newRWLock = scoped_lock{fMaybeMutex_};
                    VALUE                   v         = cacheFiller (key);
                    newRWLock.unlock ();
                    Add (key, v, purgeSpoiledData); // if purgeSpoiledData must be done, do while holding lock
                    return v;
                }
                else {
                    VALUE v = cacheFiller (key);
                    Add (key, v, purgeSpoiledData);
                    return v;
                }
            }
        }
        template <typename V = VALUE>
            requires (not same_as<V, void>)
        [[deprecated ("Since Stroika v3.0d23 - use kAutomaticPurgeFrequency in TRAITS instead of PurgeSpoiledDataFlagType")]]
        nonvirtual void Add (typename Common::ArgByValueType<KEY> key, typename Common::ArgByValueType<V> result, PurgeSpoiledDataFlagType purgeSpoiledData)
        {
            scoped_lock critSec{fMaybeMutex_};
            if (purgeSpoiledData == PurgeSpoiledDataFlagType::eAutomaticallyPurgeSpoiledData) {
                AutomaticallyPurgeExpiredDataSometimes_ ();
            }
            typename MyMapType_::iterator i = fMap_.find (key);
            if (i == fMap_.end ()) {
                fMap_.insert ({key, MyResult_{.fResult = result, .fLastRefreshedAt = Time::GetTickCount ()}});
            }
            else {
                i->second = MyResult_{.fResult = result, .fLastRefreshedAt = Time::GetTickCount ()}; // overwrite if its already there
            }
        }
        DISABLE_COMPILER_MSC_WARNING_END (4996);
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

    private:
        using MaybeMutexType_ =
            conditional_t<TRAITS::kInternallySynchronized == Execution::InternallySynchronized::eInternallySynchronized, shared_timed_mutex, Debug::AssertExternallySynchronizedMutex>;
        qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE MaybeMutexType_ fMaybeMutex_;

    private:
        Time::DurationSeconds  fMinimumAllowedFreshness_;
        Time::TimePointSeconds fNextAutoClearAt_;

    private:
        nonvirtual void ClearOld_ ();

    private:
        // per-key 'value' data we track - includes both the 'VALUE' in expiration/time information
        struct MyResult_ {
            qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE conditional_t<same_as<VALUE, void>, Common::Empty, VALUE> fResult;
            qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE conditional_t<TRAITS::kTrackFreshness, Time::TimePointSeconds, Common::Empty> fLastRefreshedAt;
            qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE conditional_t<TRAITS::kTrackExpiration, Time::TimePointSeconds, Common::Empty> fExpiresAt;

            nonvirtual CacheElement MakeCacheElement (const KEY& key) const;
        };

    private:
        bool Expired_ (const MyResult_& r, Time::TimePointSeconds now) const
            requires (TRAITS::kTrackFreshness);
        static bool Expired_ (const MyResult_& r, Time::TimePointSeconds now)
            requires (TRAITS::kTrackExpiration);

    private:
        // @todo could consider using Stroika Mapping<> - or TRAITS specified Mapper strategy
        using MyMapType_ = map<KEY, MyResult_, typename TRAITS::InOrderComparerType>;
        MyMapType_ fMap_;

    private:
        qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE mutable typename TRAITS::StatsType fStats_;
    };
    static_assert (movable<TimedCache<int, int>>); // see Satisfies Concepts
    static_assert (copyable<TimedCache<int, int>>);

    /**
     * @brief SynchronizedTimedCache just adds eInternallySynchronized to the argument TRAITS - handy shortcut
     * 
     * @tparam KEY 
     * @tparam VALUE 
     * @tparam TRAITS 
     */
    template <typename KEY, typename VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS = TimedCacheSupport::DefaultTraits<KEY, VALUE>>
    using SynchronizedTimedCache = TimedCache<KEY, VALUE, TimedCacheSupport::InternallySynchronizedTraits<TRAITS>>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TimedCache.inl"

#endif /*_Stroika_Foundation_Cache_TimedCache_h_*/
