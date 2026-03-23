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
 *      @todo   Perhaps use Stroika Mapping<> instead of std::map<> - and in that way - we can use aribtrary externally
 *              specified map impl - so can use HASHING or BTREE, based on passed in arg. So we don't ahve problem with
 *              creating the default, specify default type to create in the TRAITS object (so for example, if using Hash,
 *              we don't force having operator< for BTREE map).
 * 
 *              AND RELATED cleanup to spec using InOrderComparerType - as opposed to EQUALS COMPARER or other stuff like HashFunctions!
 *              MAYBE LOSE inorder comparer arg - and INSTEAD have template / template for creating container (for now its map<>??)
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
         * @brief A KEY is any copyable value (or the sentinal type void - indicating a keyless - single valued - cache)
         */
        template <typename T>
        concept IKey = same_as<T, void> or copyable<T>;

        /**
         * @brief any copyable type can use used as the value, with 'void' being a special sentinal type, used to indicate we are just caching presence/absense of the KEY in the cache (and its expiration date).
         */
        template <typename T>
        concept IValue = same_as<T, void> or copyable<T>;

        /**
         * @brief does this cache have a KEY type (overwhelming YES, but sometimes handy to have 'singleton' cache, where you cache something, but just one of them)
         * 
         *  \par Example Usage:
         *      \code
         *          using Cache::SynchronizedCallerStalenessCache;
         *          // one cache of network interfaces - but dont recompute it periodically
         *          static SynchronizedCallerStalenessCache<void, Collection<NetworkInterface>> sCache_;
         *          results = sCache_.LookupValue (allowedStaleness.value_or (kDefaultItemCacheLifetime_),
         *                                  [] () -> Collection<NetworkInterface> { return CollectAllNetworkInterfaces_ (); });
         *      \endcode
         */
        template <typename KEY>
        static constexpr bool IKeyedCache = not same_as<KEY, void>;

        /**
         * @brief  see TimedCache<>::TraitsType::kAutomaticPurgeFrequency - disable automatic purging
         * 
         *  \note would be nice to declare as of type Time::DurationSeconds, but then won't work as template parameter
         * 
         *  \note TimeStampDifferenceType{TimedCacheSupport::kNoAutomaticPurgeSentinal} all over place due to qCompilerAndStdLib_FloatNonTypeTemplateArgument_Buggy.
         *        if we can stop supporting compilers with this issue, we can probably redeclare this as TimeStampDifferenceType --LGP 2026-03-23
         */
        constexpr float kNoAutomaticPurgeSentinal = -1.0f;

        /**
         * @brief The default 'TTL'/Max-Age of items added to a TimedCache, if not specified/overriden anyplace else (1 minute)
         * 
         *  \note float due to qCompilerAndStdLib_FloatNonTypeTemplateArgument_Buggy.
         *        if we can stop supporting compilers with this issue, we can probably redeclare this as TimeStampDifferenceType --LGP 2026-03-23
         */
        constexpr float kDefaultMaxAge = 60.0;

        /**
         * @brief  see TimedCache<>::TraitsType::kAutomaticPurgeFrequency - default to purging every 5 minutes
         * 
         *  \note float due to qCompilerAndStdLib_FloatNonTypeTemplateArgument_Buggy.
         *        if we can stop supporting compilers with this issue, we can probably redeclare this as TimeStampDifferenceType --LGP 2026-03-23
         */
#if qCompilerAndStdLib_FloatNonTypeTemplateArgument_Buggy
        constexpr int kDefaultAutomaticPurgeFrequency = 5 * 60;
#else
        constexpr float kDefaultAutomaticPurgeFrequency = 5 * 60.0f;
#endif

        /**
         * @brief Check if argument TRAITS is a valid TRAITS object for TimedCache<>
         * 
         *  \note   ONE of (but not both) - kTrackFreshness or kTrackExpiration
         *  \note   kTrackExpiration not compatible with kAutomaticallyMarkDataAsRefreshedEachTimeAccessed
         *  \note   valid meaningful TRAITS::kDefaultMaxAge or TRAITS::kPerCacheMaxAge
         */
        template <typename TRAITS, typename KEY, typename VALUE>
        concept ITraits = IKey<KEY> and IValue<VALUE> and
                          requires (TRAITS) {
                              typename TRAITS::KeyType;
                              typename TRAITS::ResultType;
                              typename TRAITS::StatsType;
                              typename TRAITS::TimeStampType;
                              typename TRAITS::TimeStampDifferenceType;
                              { TRAITS::kInternallySynchronized } -> convertible_to<InternallySynchronized>;
                              { TRAITS::kAutomaticPurgeFrequency } -> convertible_to<typename TRAITS::TimeStampDifferenceType>;
                              { TRAITS::kAutomaticallyMarkDataAsRefreshedEachTimeAccessed } -> convertible_to<bool>;
                              { TRAITS::kTrackFreshness } -> convertible_to<bool>;
                              { TRAITS::kTrackExpiration } -> convertible_to<bool>;
                              { TRAITS::GetCurrentTimestamp () } -> convertible_to<typename TRAITS::TimeStampType>;
                              {
                                  declval<typename TRAITS::TimeStampType> () - declval<typename TRAITS::TimeStampType> ()
                              } -> convertible_to<typename TRAITS::TimeStampDifferenceType>;
                              {
                                  declval<typename TRAITS::TimeStampType> () + declval<typename TRAITS::TimeStampDifferenceType> ()
                              } -> convertible_to<typename TRAITS::TimeStampType>;
                              { TRAITS::kPerCacheMaxAge } -> convertible_to<bool>;
                          } and same_as<typename TRAITS::KeyType, KEY> and same_as<typename TRAITS::ResultType, VALUE> and
                          TRAITS::kTrackFreshness != TRAITS::kTrackExpiration and totally_ordered<typename TRAITS::TimeStampType> and
                          (not TRAITS::kAutomaticallyMarkDataAsRefreshedEachTimeAccessed or not TRAITS::kTrackExpiration) and
                          (not IKeyedCache<KEY> or Common::IInOrderComparer<typename TRAITS::InOrderComparerType, typename TRAITS::KeyType>) and
                          (not std::is_empty_v<decltype (TRAITS::kDefaultMaxAge)> or TRAITS::kPerCacheMaxAge) and
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
         *  \note Use of this directly IS allowed, but its fragile, as there isn't a good way to overload or evolve definition over time
         *        so code using this directly will be more likely to not be backward compatible in the future. Better to use adapters like InternallySynchronizedTraits
         * 
         *  \see ITraits<> above
         */
        template <IKey KEY, IValue VALUE, InternallySynchronized INTERNALLY_SYNCHRONIZED, typename STRICT_INORDER_COMPARER, bool TRACK_FRESHNESS, bool TRACK_EXPIRATION,
                  Cache::Statistics::IStatsType STATS_TYPE, typename TIMESTAMP_TYPE, typename TIMESTAMP_DIFFERENCE_TYPE, TIMESTAMP_TYPE (*GET_CURRENT_TIMESTAMP) (),
// IDEALLY if I can - TIMESTAMP_DIFFERENCE_TYPE DEFAULT_MAX_AGE;
#if qCompilerAndStdLib_FloatNonTypeTemplateArgument_Buggy
                  int DEFAULT_MAX_AGE,
#else
                  float DEFAULT_MAX_AGE,
#endif
                  bool PER_CACHE_MAX_AGE,
#if qCompilerAndStdLib_FloatNonTypeTemplateArgument_Buggy
                  int AUTOMATIC_PURGE_FREQUENCY_SECONDS,
#else
                  float AUTOMATIC_PURGE_FREQUENCY_SECONDS,
#endif
                  bool AUTO_MARK_DATA_AS_REFRESHED_ON_EACH_WRITABLE_ACCESS>
        struct ExplicitTraits {
            using KeyType    = KEY;
            using ResultType = VALUE;

            /**
             * @brief  Typically Time::TimePointSeconds
             */
            using TimeStampType = TIMESTAMP_TYPE;

            /**
             * @brief  Typically Time::DurationSeconds
             */
            using TimeStampDifferenceType = TIMESTAMP_DIFFERENCE_TYPE;

            /**
             * @brief Get the Current Timestamp object - defaults to Time::GetTickCount ()
             */
            static constexpr auto GetCurrentTimestamp{GET_CURRENT_TIMESTAMP};

            /**
             * @brief specifies a default MAX_AGE (can be UNDEFINED or Empty). If defined, its the default value returned by/used by
             *        the TimedCache as the max age (TTL) of an item added to the cache. It can generally be overridden (by Lookup or Add or LookupValue)
             *        functions (which depends on kTrackFreshness or TrackExpiration).
             * 
             *  \note TimedCache MAY have kDefaultMaxAge, but MUST have EITHER (inclusive or) kDefaultMaxAge or kPerCacheMaxAge.
             */
            static constexpr conditional_t<(DEFAULT_MAX_AGE < 0), Common::Empty, TimeStampDifferenceType> kDefaultMaxAge{DEFAULT_MAX_AGE};

            /**
             * @brief allow a per-cache MAX_AGE to be defined. If allowed, and the default (traits) is specified, that is the default for the cache instance.
             * 
             *  \note TimedCache MAY have kDefaultMaxAge, but MUST have EITHER (inclusive or) kDefaultMaxAge or kPerCacheMaxAge.
             */
            static constexpr bool kPerCacheMaxAge{PER_CACHE_MAX_AGE};

            /**
             * @brief This 'automatic synchronization' feature is off (eNotKnownInternallySynchronized) by default, but can easily
             *        be turned on with InternallySynchronizedTraits
             */
            static constexpr inline InternallySynchronized kInternallySynchronized{INTERNALLY_SYNCHRONIZED};

            /**
             * @brief freshness means when last added/updated (or if kAutomaticallyMarkDataAsRefreshedEachTimeAccessed) then last accessed too)
             *        This is true by default
             * 
             *  \note kTrackFreshness and kTrackExpiration are mutually exclusive
             */
            static constexpr inline bool kTrackFreshness{TRACK_FRESHNESS};

            /**
             * @brief Track on a per-item when it expires. If not tracked, we use expiresAt as whenAdded + maxAge
             *        This is false by default
             * 
             *  \note kTrackFreshness and kTrackExpiration are mutually exclusive
             */
            static constexpr inline bool kTrackExpiration{TRACK_EXPIRATION};

            /**
             * @brief if IKeyedCache, this is a how the KEY type elements are compared (for Lookup purposes).
             * 
             *  @todo - historical that we use INORDER comparer - cuz impl uses std::map<> - but could loosen this to be ANY EQUALITY_COMPARER or other - if we use
             *          different kinds of MAP (Mapping...) --LGP 2026-03-23
             */
            using InOrderComparerType = STRICT_INORDER_COMPARER;

            /**
             * @brief Internally synchronized 'Stats' collector type (Cache::Statistics::IStatsType). Often null stats collector.
             */
            using StatsType = STATS_TYPE;

            /**
             *  How often TimedCache-modifying operations will automatically trigger a call to ClearExpiredData ()
             * 
             *  This defaults to kDefaultAutomaticPurgeFrequency (but can be set to NEVER (kNoAutomaticPurgeSentinal)).
             * 
             *  \note - NOT triggered asynchronously, but from modifying APIs, like Add, or non-const Lookup()
             */
            static constexpr TimeStampDifferenceType kAutomaticPurgeFrequency{AUTOMATIC_PURGE_FREQUENCY_SECONDS};

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
         * @brief Default choices for TimedCache, if you don't specify anything else.
         * 
         * @tparam KEY 
         * @tparam VALUE 
         */
        template <IKey KEY, IValue VALUE>
        using DefaultTraits = ExplicitTraits<KEY, VALUE, InternallySynchronized::eNotKnownInternallySynchronized, less<KEY>, true, false,
                                             Statistics::StatsType_DEFAULT, Time::TimePointSeconds, Time::DurationSeconds, &Time::GetTickCount,
#if qCompilerAndStdLib_FloatNonTypeTemplateArgument_Buggy
                                             (int)
#endif
                                                 kDefaultMaxAge,
                                             true, kDefaultAutomaticPurgeFrequency, false>;

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
     *  \brief Keep track of a bunch of objects, each with an associated time used to allow data to 'expire'.
     * 
     *  This expiration time is handled PRINCIPALLY, in one of two ways:
     *      kTrackExpiration:
     *          In this case, at the time the data is ADDED, a time of expiration is captured and associated with the datum.
     * 
     *      kTrackFreshness:
     *          In this case, the time the data is Added (last-refreshed) is associated with the datum, and expiration computed later.
     *          **this is the default**
     *  
     *  More about kTrackFreshness:
     *      We define 'fresheness' somewhat arbitrarily, but by default, this means since the item was added. However, the TimedCache
     *      also provides other apis to update the 'freshness' of a stored object, depending on application needs.
     *
     *      Keeps track of all items - indexed by Key - but throws away items which are any more
     *      stale than given by the staleness limit.
     *
     *  When/Why to use?:
     *      (@todo revise these docs - next 15 lines)
     *      The idea behind this cache is to track when something is added, and that the lookup function can avoid
     *      a costly call to compute something if its been recently enough added.
     *
     *      For example, consider a system where memory is stored across a slow bus, and several components need to read data from
     *      across that bus. But the different components have different tolerance for staleness (e.g. PID loop needs fresh temperature sensor
     *      data but GUI can use more stale data).
     *
     *      This CallerStalenessCache will store when the value is updated, and let the caller either return the
     *      value from cache, or fetch it and update the cache if needed.
     *
     *      This differs from other forms of caches in that:
     *          o   It records the timestamp when a value is last-updated
     *          o   It doesn't EXPIRE the data ever (except by explicit Clear or ClearOlderThan call)
     *          o   The lookup caller specifies its tolerance for data staleness, and refreshes the data as needed.
     *
     *  \note Comparison with LRUCache
     *        The main difference beweeen an LRUCache and TimedCache has to do with when an element is evicted from the Cache.
     *        With a TimedCache, its evicted only when its overly aged. With an LRUCache, its more random, and depends a
     *        bit on luck (when using hashing) and how recently an item was last accessed.
     *
     *  \note   TimedCache (since Stroika v3.0d23) fully supports the caching model from the Stroika v2.1
     *          CallerStalenessCache (which is now obsolete). If the cache uses 'Freshness' instead of 'Expired' (the default)
     *          then the various Lookup APIs fully support specifying a (non-defaulted, caller specified) 'staleness'.
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
     *  \par Example Usage (caller specifies staleness in each context where lookup happens)
     *      \code
     *          // keyed cache
     *          optional<int> MapValue_ (int value, optional<Time::DurationSeconds> allowedStaleness = {})
     *          {
     *              static CallerStalenessCache<int, optional<int>> sCache_;
     *              try {
     *                  return sCache_.LookupValue (value, allowedStaleness.value_or (30), [=](int v) -> optional<int> {
     *                      return v;   // typically more expensive computation
     *                  });
     *              }
     *              catch (...) {
     *                  // NOTE - to NEGATIVELY CACHE failure, you could call sCache_.Add (value, nullopt);
     *                  // return null here, or Execution::ReThrow ()
     *              }
     *          }
     *          EXPECT_EQ (MapValue_ (1), 1);  // skips 'more expensive computation' if in cache
     *          EXPECT_EQ (MapValue_ (2), 2);  // ''
     *      \endcode
     * 
     *  \par Example Usage (no key, and 'caller staleness' style)
     *      \code
     *          optional<InternetAddress> LookupExternalInternetAddress_ (optional<Time::DurationSeconds> allowedStaleness = {})
     *          {
     *              static TimedCache<void, optional<InternetAddress>> sCache_;
     *              return sCache_.Lookup (allowedStaleness.value_or (30), []() -> optional<InternetAddress> {
     *                  ...
     *                  return IO::Network::InternetAddress{connection.GET ().GetDataTextInputStream ().ReadAll ().Trim ()};
     *              });
     *          }
     *          optional<InternetAddress> iaddr = LookupExternalInternetAddress_ ();    // only invoke connection logic if timed out
     *      \endcode
     *
     *  \par Example Usage
     * @todo REVIEW DOCS FROM HERE DOWN
     * 
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
     *  @see LRUCache
     */
    template <TimedCacheSupport::IKey KEY, TimedCacheSupport::IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS = TimedCacheSupport::DefaultTraits<KEY, VALUE>>
    class TimedCache {
    public:
        using TraitsType = TRAITS;

    public:
        /**
         */
        using TimeStampType = typename TRAITS::TimeStampType;

    public:
        /**
         */
        using TimeStampDifferenceType = typename TRAITS::TimeStampDifferenceType;

    public:
        /**
         * @brief Track on a per-item when it expires. If not tracked, we use expiresAt as whenAdded + maxAge
         *        This is false by default
         * 
         *  \note kTrackFreshness and kTrackExpiration are mutually exclusive
         */
        static constexpr bool kTrackExpiration = TRAITS::kTrackExpiration;

    public:
        /**
         * @brief freshness means when last added/updated (or if kAutomaticallyMarkDataAsRefreshedEachTimeAccessed) then last accessed too)
         *        This is true by default
         * 
         *  \note kTrackFreshness and kTrackExpiration are mutually exclusive
         */
        static constexpr bool kTrackFreshness = TRAITS::kTrackFreshness;

    public:
        /**
         * Note that TimedCache is copyable and moveable by value.
         * 
         *  It MAYBE default constructible (if there is a kDefaultMaxAge - defaults to true).
         */
        TimedCache ()
            requires (not is_empty_v<decltype (TRAITS::kDefaultMaxAge)>);
        explicit TimedCache (TimeStampDifferenceType maxAge)
            requires (TRAITS::kPerCacheMaxAge);
        TimedCache (TimedCache&& src) noexcept;
        TimedCache (const TimedCache& src);

    public:
        nonvirtual TimedCache& operator= (TimedCache&& rhs) noexcept;
        nonvirtual TimedCache& operator= (const TimedCache& rhs);

    private:
        // same as GetMaxAge, but doesnt do locking
        constexpr TimeStampDifferenceType GetMaxAge_ () const;

    public:
        /**
         *  When items are added to the timed cache, there is a universal (for the entire cache) minimum allowed freshness (how old item
         *  allowed to be before thrown away).
         * 
         *  \alias Note - 'allowed freshness' == 'time to live' == 'TTL'.
         *         GetMinimumFreshness, GetTimeout, TTL, GetDefaultMaxAge ().
         * 
         *  So an item added 30 seconds ago (freshness = 30s), would be thrown away/not returned as part of the cache
         *  if the minimum allowed freshness was 5 seconds.
         * 
         *  \note if kTrackFreshness, this is used in CHECKS of freshness (provides a default 'maxage' at the point of Lookup call).
         *        if kTrackExpiration, this is applied to the added item, as its default expiration. In both cases, these values
         *        can be overridden (either in Add or Lookup depending on which you are tracking).
         */
        nonvirtual TimeStampDifferenceType GetMaxAge () const;

    public:
        /**
         *  @see GetMaxAge ()
         */
        nonvirtual void SetMaxAge (TimeStampDifferenceType maxAge)
            requires (TRAITS::kPerCacheMaxAge);

    public:
        /**
         * @brief everything here is optional ;-) But typically, its fKey, fValue, fLastRefreshedAt
         */
        struct CacheElement {
            qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE conditional_t<same_as<KEY, void>, Common::Empty, KEY> fKey;
            qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE conditional_t<same_as<VALUE, void>, Common::Empty, VALUE> fValue;
            qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE conditional_t<kTrackFreshness, TimeStampType, Common::Empty> fLastRefreshedAt;
            qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE conditional_t<kTrackExpiration, TimeStampType, Common::Empty> fExpiresAt;
        };

    public:
        /**
         *  \note This returns the non-expired elements of the current cache object.
         * 
         *      @todo could use overload taking TTL argumnet (if track freshness)
         */
        nonvirtual Traversal::Iterable<CacheElement> Elements () const;

    public:
        /**
         *  \note This returns the non-expired keys of the current cache object.
         * 
         *      @todo could use overload taking TTL argumnet (if track freshness)
         */
        template <typename K = KEY>
        nonvirtual Traversal::Iterable<K> Keys () const
            requires (TimedCacheSupport::IKeyedCache<K>);

    public:
        /**
         *  \brief Returns the value associated with argument 'key', or nullopt, if its missing (missing same as expired). Can be used to retrieve lastRefreshedAt
         * 
         *  \note that the non-const overload of Lookup respects TRAITS::kAutomaticallyMarkDataAsRefreshedEachTimeAccessed, and will
         *        auto-refresh the item (similar to LRUCache) if found.
         * 
         *  Occasionally, a caller might want to ASSURE it gets data, and just use the cached value if fresh enuf, and specify
         *  a lookup lambda to fetch the actual data if its not fresh, in which case call LookupValue ().
         *
         *  \note difference between const and non-const overloads is just that some extra bookkeeping can be done and kAutomaticallyMarkDataAsRefreshedEachTimeAccessed respected in non-const overload.
         * 
         *  \note for Lookup (key)
         *        whether we track freshness or expiration, the API looks the same (though the impl differs).
         *  \note for Lookup (key,maxAge)
         *        only possible if you track freshness
         *        no non-const version of this, cuz no bookkeeping. If you are using this, the caller decides what gets cleaned up, explicitly.
         */
        template <typename K = KEY>
            requires (TimedCacheSupport::IKeyedCache<K>)
        nonvirtual optional<VALUE> Lookup (typename Common::ArgByValueType<K> key) const;
        template <typename K = KEY>
            requires (TimedCacheSupport::IKeyedCache<K>)
        nonvirtual optional<VALUE> Lookup (typename Common::ArgByValueType<K> key);
        template <typename K = KEY>
            requires (TimedCacheSupport::IKeyedCache<K> and TRAITS::kTrackFreshness)
        nonvirtual optional<VALUE> Lookup (typename Common::ArgByValueType<K> key, TimeStampDifferenceType maxAge) const;
        template <typename K = KEY>
            requires (not TimedCacheSupport::IKeyedCache<K>)
        nonvirtual optional<VALUE> Lookup () const;
        template <typename K = KEY>
            requires (not TimedCacheSupport::IKeyedCache<K>)
        nonvirtual optional<VALUE> Lookup ();
        template <typename K = KEY>
            requires (not TimedCacheSupport::IKeyedCache<K> and TRAITS::kTrackFreshness)
        nonvirtual optional<VALUE> Lookup (TimeStampDifferenceType maxAge) const;

    public:
        /**
         *  \brief Returns the value associated with argument 'key' (if IKeyedCache), or nullopt, if its missing (missing same as expired). Like Lookup but also returns expiration information.
         * 
         *  \note that the non-const overload of Lookup respects TRAITS::kAutomaticallyMarkDataAsRefreshedEachTimeAccessed, and will
         *        auto-refresh the item (similar to LRUCache) if found.
         * 
         *  Occasionally, a caller might want to ASSURE it gets data, and just use the cached value if fresh enuf, and specify
         *  a lookup lambda to fetch the actual data if its not fresh, in which case call LookupValue ().
         *
         *  \note difference between const and non-const overloads is just that some extra bookkeeping can be done and kAutomaticallyMarkDataAsRefreshedEachTimeAccessed respected in non-const overload.
         */
        template <typename K = KEY>
        nonvirtual optional<tuple<VALUE, TimeStampType>> LookupDetails (typename Common::ArgByValueType<K> key) const
            requires (kTrackExpiration and TimedCacheSupport::IKeyedCache<K>);
        template <typename K = KEY>
        nonvirtual optional<tuple<VALUE, TimeStampType>> LookupDetails (typename Common::ArgByValueType<K> key)
            requires (kTrackExpiration and TimedCacheSupport::IKeyedCache<K>);
        template <typename K = KEY>
        nonvirtual optional<tuple<VALUE, TimeStampType>> LookupDetails (typename Common::ArgByValueType<K> key) const
            requires (kTrackFreshness and TimedCacheSupport::IKeyedCache<K>);
        template <typename K = KEY>
        nonvirtual optional<tuple<VALUE, TimeStampType>> LookupDetails (typename Common::ArgByValueType<K> key)
            requires (kTrackFreshness and TimedCacheSupport::IKeyedCache<K>);
        template <typename K = KEY>
        nonvirtual optional<tuple<VALUE, TimeStampType>> LookupDetails (typename Common::ArgByValueType<K> key, TimeStampDifferenceType maxAge) const
            requires (kTrackFreshness and TimedCacheSupport::IKeyedCache<K>);
        template <typename K = KEY>
        nonvirtual optional<tuple<VALUE, TimeStampType>> LookupDetails (TimeStampDifferenceType maxAge) const
            requires (not TimedCacheSupport::IKeyedCache<K>);

    public:
        /**
         * @brief Get the Expiration of object or nullopt of item expired/not in cache
         * 
         * in the case of IKeyedCache, it returns the expiration of the given key (or nullopt if not present or expired).
         * in the case of NOT IKeyedCache, it refers to the single cached item, but otherwise acts the same.
         */
        template <typename K = KEY>
        nonvirtual optional<TimeStampType> GetExpiration () const
            requires (not TimedCacheSupport::IKeyedCache<K>);
        template <typename K = KEY>
        nonvirtual optional<TimeStampType> GetExpiration (typename Common::ArgByValueType<K> key) const
            requires (TimedCacheSupport::IKeyedCache<K>);

    public:
        /**
         * @brief Lookup value, and if missing, fetch it with argument cacheFiller (and add/return its value).
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
         *  \note **this is the PREFERRED API for adding/looking up in TimedCache**
         *
         *  Usually one will use this as:
         *      \code
         *          VALUE v = cache.LookupValue (key, ts, [this] () -> VALUE {return this->realLookup(key); });
         *      \endcode
         * 
         *  \note   This function may update the TimedCache (which is why it is non-const).
         * 
         *  \note   Any time arguments given constrain the lookup. They are not used for the Add ().
         */
        template <typename K = KEY, Common::invocable_r<VALUE, KEY> CACHE_FILLTER_T>
            requires (TimedCacheSupport::IKeyedCache<K>)
        nonvirtual VALUE LookupValue (typename Common::ArgByValueType<K> key, CACHE_FILLTER_T&& cacheFiller);
        template <typename K = KEY, Common::invocable_r<VALUE, KEY> CACHE_FILLTER_T>
            requires (TimedCacheSupport::IKeyedCache<K> and TRAITS::kTrackFreshness)
        nonvirtual VALUE LookupValue (typename Common::ArgByValueType<K> key, TimeStampDifferenceType maxAge, CACHE_FILLTER_T&& cacheFiller);
        template <typename K = KEY, Common::invocable_r<VALUE> CACHE_FILLTER_T>
            requires (not TimedCacheSupport::IKeyedCache<K>)
        nonvirtual VALUE LookupValue (CACHE_FILLTER_T&& cacheFiller);
        template <typename K = KEY, Common::invocable_r<VALUE> CACHE_FILLTER_T>
            requires (not TimedCacheSupport::IKeyedCache<K> and TRAITS::kTrackFreshness)
        nonvirtual VALUE LookupValue (TimeStampDifferenceType maxAge, CACHE_FILLTER_T&& cacheFiller);

    public:
        /**
         *  Adds/Updates the given value associated with key.
         *      if kTrackFreshness (the default)
         *          o   The new items freshness is TRAITS::GetCurrentTimestamp (), or the value given as argument
         *      if kTrackExpiration
         *          o   The new item's expiration is either given by expiresAt or now+ttl, or defaults to
         *              GetMaxAge()
         * 
         *  \note this API supports overloads of Add () where either the KEY or the VALUE is missing, depending on how
         *        the TimedCache TRAITS were declared.
         */
        template <typename K = KEY>
            requires (TimedCacheSupport::IKeyedCache<K> and same_as<VALUE, void>)
        nonvirtual void Add (typename Common::ArgByValueType<K> key);
        template <typename K = KEY, typename V = VALUE>
            requires (TimedCacheSupport::IKeyedCache<K> and not same_as<V, void>)
        nonvirtual void Add (typename Common::ArgByValueType<K> key, typename Common::ArgByValueType<V> result);
        template <typename K = KEY, typename V = VALUE>
            requires (TimedCacheSupport::IKeyedCache<K> and not same_as<V, void> and TRAITS::kTrackExpiration)
        nonvirtual void Add (typename Common::ArgByValueType<K> key, typename Common::ArgByValueType<V> result, TimeStampType expiresAt);
        template <typename K = KEY, typename V = VALUE>
            requires (TimedCacheSupport::IKeyedCache<K> and not same_as<V, void> and TRAITS::kTrackFreshness)
        nonvirtual void Add (typename Common::ArgByValueType<K> key, typename Common::ArgByValueType<V> result, TimeStampType freshAsOf);
        template <typename K = KEY, typename V = VALUE>
            requires (not TimedCacheSupport::IKeyedCache<K> and not same_as<V, void>)
        nonvirtual void Add (typename Common::ArgByValueType<V> result);
        template <typename K = KEY, typename V = VALUE>
            requires (not TimedCacheSupport::IKeyedCache<K> and not same_as<V, void> and TRAITS::kTrackFreshness)
        nonvirtual void Add (typename Common::ArgByValueType<V> result, TimeStampType freshAsOf);
        template <typename K = KEY, typename V = VALUE>
            requires (not TimedCacheSupport::IKeyedCache<K> and not same_as<V, void> and TRAITS::kTrackExpiration)
        nonvirtual void Add (typename Common::ArgByValueType<V> result, TimeStampType expiresAt);

    public:
        /**
         */
        template <typename K = KEY>
            requires (TimedCacheSupport::IKeyedCache<K>)
        nonvirtual void Remove (typename Common::ArgByValueType<K> key);

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
        nonvirtual void ClearExpiredData ();
        nonvirtual void ClearExpiredData (TimeStampDifferenceType maxAge)
            requires (kTrackFreshness);

    public:
        /**
         */
        nonvirtual typename TRAITS::StatsType GetStats () const;

    private:
        nonvirtual void AutomaticallyClearExpiredDataSometimes_ ();

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
        [[deprecated ("Since Stroika v3.0d1, use ClearExpiredData or count on Add's purgeSpoiledData parameter)")]] nonvirtual void DoBookkeeping ()
        {
            ClearExpiredData ();
        }
        [[deprecated ("Since Stroika 3.0d1 use GetMaxAge")]] Time::Duration GetTimeout () const
        {
            return GetMaxAge ();
        }
        [[deprecated ("Since Stroika 3.0d1 use GetMaxAge")]] void SetTimeout (Time::Duration timeout)
        {
            SetMaxAge (timeout);
        }
        [[deprecated ("Since Stroika 3.0d23 use GetMaxAge")]]
        nonvirtual Time::Duration GetMinimumAllowedFreshness () const
        {
            return GetMaxAge ();
        }
        [[deprecated ("Since Stroika 3.0d23 use SetMaxAge")]]
        nonvirtual void SetMinimumAllowedFreshness (Time::Duration minimumAllowedFreshness)
        {
            SetMaxAge (minimumAllowedFreshness);
        }
        template <typename K = KEY>
            requires (TimedCacheSupport::IKeyedCache<K> and TRAITS::kTrackFreshness)
        [[deprecated ("Since Stroika 3.0d23 use maxAge DURATION directly")]]
        optional<VALUE> Lookup (typename Common::ArgByValueType<K> key, TimeStampType maxAge) const
        {
            return Lookup (key, Ago (maxAge)); // IS THIS BACKWARDS
        }
        template <typename K = KEY>
        [[deprecated ("Since Stroika 3.0d23 use maxAge DURATION directly")]]
        nonvirtual optional<tuple<VALUE, TimeStampType>> LookupDetails (typename Common::ArgByValueType<K> key, TimeStampType maxAge) const
            requires (TRAITS::kTrackFreshness and TimedCacheSupport::IKeyedCache<K>)
        {
            AssertNotImplemented ();
            return nullopt;
        }
        template <typename K = KEY, Common::invocable_r<VALUE> CACHE_FILLTER_T>
            requires (not TimedCacheSupport::IKeyedCache<K> and TRAITS::kTrackFreshness)
        nonvirtual VALUE LookupValue (TimeStampType maxAge, CACHE_FILLTER_T&& cacheFiller)
        {
            return Lookup (Ago (maxAge), forward<CACHE_FILLTER_T> (cacheFiller));
        }
        template <typename K = KEY, Common::invocable_r<VALUE, KEY> CACHE_FILLTER_T>
            requires (TimedCacheSupport::IKeyedCache<K> and TRAITS::kTrackFreshness)
        [[deprecated ("Since Stroika 3.0d23 use maxAge DURATION directly")]]
        nonvirtual VALUE LookupValue (typename Common::ArgByValueType<K> key, TimeStampType maxAge, CACHE_FILLTER_T&& cacheFiller)
        {
            return Lookup (key, Ago (maxAge), forward<CACHE_FILLTER_T> (cacheFiller));
        }
        template <typename K = KEY>
        [[deprecated ("Since Stroika v3.0d23 - use TRAITS::kAutomaticallyMarkDataAsRefreshedEachTimeAccessed instead")]]
        nonvirtual optional<VALUE> Lookup (typename Common::ArgByValueType<K> key, LookupMarksDataAsRefreshed successfulLookupRefreshesAcceesFlag)
        {
            scoped_lock                   critSec{fMaybeMutex_};
            typename MyMapType_::iterator i   = fData_.find (key);
            TimeStampType                 now = TRAITS::GetCurrentTimestamp ();
            if (i == fData_.end ()) {
                fStats_.IncrementMisses ();
                return nullopt;
            }
            else {
                TimeStampType lastAccessThreshold = now - fMaxAge_;
                if (i->second.fLastRefreshedAt < lastAccessThreshold) {
                    /**
                     *  Before Stroika 3.0d1, we used to remove the entry from the list (an optimization). But
                     * that required Lookup to be non-const (with synchronization in mind probably a pessimization).
                     * So instead, count on PurgeUnusedData being called automatically on future adds,
                     * explicit user calls to purge unused data.
                     *
                     *      i = fData_.erase (i);
                     */
                    fStats_.IncrementMisses ();
                    return nullopt;
                }
                if (successfulLookupRefreshesAcceesFlag == LookupMarksDataAsRefreshed::eTreatFoundThroughLookupAsRefreshed) {
                    i->second.fLastRefreshedAt = TRAITS::GetCurrentTimestamp ();
                }
                fStats_.IncrementHits ();
                return i->second.fResult;
            }
        }
        template <typename K = KEY>
            requires (TimedCacheSupport::IKeyedCache<K>)
        [[deprecated ("Since Stroika v3.0d23 - use kAutomaticPurgeFrequency in TRAITS instead of PurgeSpoiledDataFlagType, and "
                      "kAutomaticallyMarkDataAsRefreshedEachTimeAccessed in TRAITS instead of LookupMarksDataAsRefreshed")]]
        nonvirtual VALUE LookupValue (typename Common::ArgByValueType<K> key, const function<VALUE (typename Common::ArgByValueType<K>)>& cacheFiller,
                                      LookupMarksDataAsRefreshed successfulLookupRefreshesAcceesFlag,
                                      PurgeSpoiledDataFlagType purgeSpoiledData = PurgeSpoiledDataFlagType::eAutomaticallyPurgeSpoiledData)
        {
            auto&& readLock = shared_lock{fMaybeMutex_}; // try shared_lock for case where present, and then lose it if we need to update object
            if (optional<VALUE> o = Lookup (key, successfulLookupRefreshesAcceesFlag)) {
                return *o;
            }
            else {
                readLock.unlock ();
                VALUE v = cacheFiller (key);
                Add (key, v, purgeSpoiledData);
                return v;
            }
        }
        template <typename K = KEY, typename V = VALUE>
            requires (TimedCacheSupport::IKeyedCache<K> and not same_as<V, void>)
        [[deprecated ("Since Stroika v3.0d23 - use kAutomaticPurgeFrequency in TRAITS instead of PurgeSpoiledDataFlagType")]]
        nonvirtual void Add (typename Common::ArgByValueType<K> key, typename Common::ArgByValueType<V> result, PurgeSpoiledDataFlagType purgeSpoiledData)
        {
            scoped_lock critSec{fMaybeMutex_};
            if (purgeSpoiledData == PurgeSpoiledDataFlagType::eAutomaticallyPurgeSpoiledData) {
                AutomaticallyClearExpiredDataSometimes_ ();
            }
            typename MyMapType_::iterator i = fData_.find (key);
            if (i == fData_.end ()) {
                fData_.insert ({key, MyResult_{.fResult = result, .fLastRefreshedAt = TRAITS::GetCurrentTimestamp ()}});
            }
            else {
                i->second = MyResult_{.fResult = result, .fLastRefreshedAt = TRAITS::GetCurrentTimestamp ()}; // overwrite if its already there
            }
        }
        template <typename K = KEY>
        [[deprecated ("Since Stroika v3.0d23 - use LookupDetails")]]
        nonvirtual optional<VALUE> Lookup (typename Common::ArgByValueType<K> key, TimeStampType* lastRefreshedAt) const
            requires (TRAITS::kTrackFreshness)
        {
            if (auto r = LookupDetails (key)) {
                if (lastRefreshedAt != nullptr) {
                    *lastRefreshedAt = get<2> (*r);
                }
                return get<1> (*r);
            }
        }
        template <typename K = KEY>
        [[deprecated ("Since Stroika v3.0d23 - use LookupDetails")]]
        nonvirtual optional<VALUE> Lookup (typename Common::ArgByValueType<K> key, TimeStampType* lastRefreshedAt)
            requires (TRAITS::kTrackFreshness)
        {
            if (auto r = LookupDetails (key)) {
                if (lastRefreshedAt != nullptr) {
                    *lastRefreshedAt = get<2> (*r);
                }
                return get<1> (*r);
            }
        }
        template <typename K = KEY>
        [[deprecated ("Since Stroika v3.0d23 - use LookupDetails")]]
        nonvirtual optional<VALUE> Lookup (typename Common::ArgByValueType<K> key, TimeStampType* expiresAt) const
            requires (TRAITS::kTrackExpiration)
        {
            if (auto r = LookupDetails (key)) {
                if (expiresAt != nullptr) {
                    *expiresAt = get<2> (*r);
                }
                return get<1> (*r);
            }
        }
        template <typename K = KEY>
        [[deprecated ("Since Stroika v3.0d23 - use LookupDetails")]]
        nonvirtual optional<VALUE> Lookup (typename Common::ArgByValueType<K> key, TimeStampType* expiresAt)
            requires (TRAITS::kTrackExpiration)
        {
            if (auto r = LookupDetails (key)) {
                if (expiresAt != nullptr) {
                    *expiresAt = get<2> (*r);
                }
                return get<1> (*r);
            }
        }
        DISABLE_COMPILER_MSC_WARNING_END (4996);
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

    private:
        // note if shared_mutex, it must be mutable, cuz shared locks still must be done
        using MaybeMutexType_ =
            conditional_t<TRAITS::kInternallySynchronized == Execution::InternallySynchronized::eInternallySynchronized, shared_timed_mutex, Debug::AssertExternallySynchronizedMutex>;
        qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE mutable MaybeMutexType_ fMaybeMutex_;

    private:
        TimeStampDifferenceType fMaxAge_;
        qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE
            conditional_t<TRAITS::kAutomaticPurgeFrequency == TimeStampDifferenceType{TimedCacheSupport::kNoAutomaticPurgeSentinal}, Common::Empty, TimeStampType>
                fNextAutoClearAt_;

    private:
        nonvirtual void ClearExpired_ ();
        nonvirtual void ClearExpired_ (TimeStampDifferenceType maxAge)
            requires (kTrackFreshness);

    private:
        // per-key 'value' data we track - includes both the 'VALUE' in expiration/time information
        struct MyResult_ {
            qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE conditional_t<same_as<VALUE, void>, Common::Empty, VALUE> fResult;
            qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE conditional_t<kTrackFreshness, TimeStampType, Common::Empty> fLastRefreshedAt;
            qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE conditional_t<kTrackExpiration, TimeStampType, Common::Empty> fExpiresAt;

            template <typename K = KEY>
            nonvirtual CacheElement MakeCacheElement (const K& key) const;
        };

    private:
        nonvirtual bool Expired_ (const MyResult_& r, TimeStampType now = TRAITS::GetCurrentTimestamp ()) const;

    private:
        // @todo could consider using Stroika Mapping<> - or TRAITS specified Mapper strategy
        using MyMapType_ =
            conditional_t<TimedCacheSupport::IKeyedCache<KEY>, Common::LazyType_t<map, KEY, MyResult_, typename TRAITS::InOrderComparerType>, optional<MyResult_>>;
        MyMapType_ fData_;

    private:
        // pass in readLock can can be 'upgraded' to full lock
        template <typename K = KEY, Common::invocable_r<VALUE> CACHE_FILLTER_T>
            requires (not TimedCacheSupport::IKeyedCache<K>)
        nonvirtual VALUE LookupValueAdder_ (CACHE_FILLTER_T&& cacheFiller);
        template <typename K = KEY, Common::invocable_r<VALUE, KEY> CACHE_FILLTER_T>
            requires (TimedCacheSupport::IKeyedCache<K>)
        nonvirtual VALUE LookupValueAdder_ (typename Common::ArgByValueType<K> key, shared_lock<MaybeMutexType_>* lock, CACHE_FILLTER_T&& cacheFiller);

    private:
        qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE mutable typename TRAITS::StatsType fStats_;
    };
    static_assert (movable<TimedCache<int, int>>); // see Satisfies Concepts
    static_assert (copyable<TimedCache<int, int>>);

    /**
     * @brief SynchronizedTimedCache just adds eInternallySynchronized to a regular 'TimedCache' (just short-hand).
     * 
     * @tparam KEY 
     * @tparam VALUE 
     * @tparam TRAITS 
     * 
     *  \par Example Usage
     *      Use TimedCache to avoid needlessly redundant lookups
     *      \code
     *          optional<String> ReverseDNSLookup_ (const InternetAddress& inetAddr)
     *          {
     *              static SynchronizedTimedCache<InternetAddress, optional<String>> sCache_;
     *              // Or could write like this
     *              static TimedCache<InternetAddress, optional<String>, TimedCacheSupport::InternallySynchronizedTraits<TimedCacheSupport::DefaultTraits<InternetAddress, optional<String>>>> sCache2_;
     *              return sCache_.LookupValue (inetAddr, [] (const InternetAddress& inetAddr) {
     *                  return DNS::kThe.ReverseLookup (inetAddr);
     *              });
     *          }
     *      \endcode
     * 
     *      \note this CAN be done pretty easily without defining SynchronizedTimedCache, but this is enough more terse
     *             and common enough to be useful.
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
