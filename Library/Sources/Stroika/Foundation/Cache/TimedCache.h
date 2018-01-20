/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_TimedCache_h_
#define _Stroika_Foundation_Cache_TimedCache_h_ 1

#include "../StroikaPreComp.h"

#include <map>
#include <mutex>

#include "../Characters/SDKChar.h"
#include "../Common/Compare.h"
#include "../Configuration/Common.h"
#include "../Configuration/TypeHints.h"
#include "../Debug/AssertExternallySynchronizedLock.h"
#include "../Debug/Assertions.h"
#include "../Memory/Optional.h"
#include "../Time/Realtime.h"

/**
 *      \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *
 *      @todo   fNextAutoclearAt is HORRIBLE mechnism to figure out if we need to walk list and
 *              clear. Use a time value (max age), and time last checked or something like that).
 *
 *      @todo   This class is logically a map. But you may want to have individual values with timed cache!
 *              Basically - KEY=RESULT? And then the arg to add/lookup dont take key? Maybe key is void?
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
 *              specified map impl - so can use HASHING or BTREE, based on passed in arg. So we dont ahve problem with
 *              creating the default, specify default type to create in the TRAITS object (so for example, if using Hash,
 *              we dont force having operator< for BTREE map).
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

namespace Stroika {
    namespace Foundation {
        namespace Cache {

            using Stroika::Foundation::Characters::SDKChar;

            namespace TimedCacheSupport {

                /**
                 *  Helper detail class for analyzing and tuning cache statistics.
                 */
                struct Stats_Basic {
                    Stats_Basic () = default;
                    size_t fCachedCollected_Hits{};
                    size_t fCachedCollected_Misses{};

                    nonvirtual void IncrementHits ();
                    nonvirtual void IncrementMisses ();
                    nonvirtual void DbgTraceStats (const Characters::SDKChar* label) const;
                };

                /**
                 *  Helper for DefaultTraits - when not collecting stats.
                 */
                struct Stats_Null {
                    nonvirtual void IncrementHits ();
                    nonvirtual void IncrementMisses ();
                    nonvirtual void DbgTraceStats (const Characters::SDKChar* label) const;
                };

                /**
                 * The DefaultTraits<> is a simple default traits implementation for building an TimedCache<>.
                 */
                template <typename KEY, typename VALUE, typename WELL_ORDER_COMPARER = Common::ComparerWithWellOrder<KEY>, bool TRACK_READ_ACCESS = false>
                struct DefaultTraits {
                    using KeyType    = KEY;
                    using ResultType = VALUE;

#if qDebug
                    using StatsType = Stats_Basic;
#else
                    using StatsType = Stats_Null;
#endif
                    static constexpr bool kTrackReadAccess = TRACK_READ_ACCESS;

                    /**
                     */
                    using WellOrderCompareFunctionType = WELL_ORDER_COMPARER;
                };
            }

            /**
             *  Keeps track of all items - indexed by Key - but throws away items which are any more
             *  stale than given by the TIMEOUT
             *
             *  \note   Note - this class doesn't employ a thread to throw away old items, so if you count on that
             *          happening (e.g. because the VALUE object DTOR has a side-effect like closing a file), then
             *          you may call DoBookkeeping () peridocially.
             *
             *  \par Example Usage
             *      Use TimedCache to avoid needlessly redundant lookups
             *
             *      Assume 'LookupDiskStats_' returns DiskSpaceUsageType, but its expensive, and the results change only slowly...
             *
             *      \code
             *      Cache::TimedCache<String, DiskSpaceUsageType>   sDiskUsageCache_ { 5.0 };
             *
             *      DiskSpaceUsageType LookupDiskStats (String diskName)
             *      {
             *          Optional<DiskSpaceUsageType>    o   =   sDiskUsageCache_.Lookup (diskName);
             *          if (o.IsMissing ()) {
             *              o = LookupDiskStats_ ();
             *              sDiskUsageCache_.Add (diskName, *o);
             *          }
             *          return o.Value ();
             *      }
             *      \endcode
             *
             *  or better yet:
             *      \code
             *      DiskSpaceUsageType LookupDiskStats2 (String diskName)
             *      {
             *          return sDiskUsageCache_.Lookup (diskName,
             *              [] (String diskName) -> DiskSpaceUsageType {
             *                  return LookupDiskStats_ (diskName);
             *              }
             *          );
             *      }
             *      \endcode
             *
             *  or still better (if no context needed for lookup function):
             *      \code
             *      DiskSpaceUsageType LookupDiskStats3 (String diskName)
             *      {
             *          return sDiskUsageCache_.Lookup (diskName, LookupDiskStats_);
             *      }
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
             *      \code
             *      using   ScanFolderKey_      =   String;
             *      static  constexpr   DurationSecondsType kAgeForScanPersistenceCache_ { 5 * 60.0 };
             *      struct FolderDetails_ {
             *          int size;       // ...info to cache about a folder
             *      };
             *      Synchronized<Cache::TimedCache<
             *          ScanFolderKey_,
             *          shared_ptr<FolderDetails_>,
             *          TimedCachedSupport::DefaultTraits<ScanFolderKey_,shared_ptr<FolderDetails_>,Common::ComparerWithWellOrder<ScanFolderKey_>,true>
             *          >
             *          >
             *          sCachedScanFoldersDetails_ {kAgeForScanPersistenceCache_ }
             *          ;
             *
             *      shared_ptr<FolderDetails_> AccessFolder_ (const ScanFolderKey_& folder) const
             *      {
             *           auto lockedCache = sCachedScanFoldersDetails_.rwget ();
             *           if (Optional<FolderDetails_> o  = lockedCache->Lookup (folder)) {
             *                  return *o;
             *           }
             *           else {
             *              FolderDetails_  fd = make_shared<Folder_Details_> ();   // and fill in default values looking at disk
             *              lockedCache->Add (folder, fd);
             *              return move (fd);
             *          }
             *      }
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
                 *  Usually one will use this as
                 *      VALUE v = cache.Lookup (key, ts, [this] () -> VALUE {return this->realLookup(key); });
                 *
                 *  However, the overload returing an optional is occasionally useful, if you dont want to fill the cache
                 *  but just see if a value is present.
                 *
                 *  Both the overload with cacheFiller, and defaultValue will update the 'time stored' for the argument key.
                 *
                 *  \note   if TraitsType::kTrackReadAccess is true (defaults false), this will also update the last-accessed date
                 */
                nonvirtual Memory::Optional<VALUE> Lookup (typename Configuration::ArgByValueType<KEY> key);
                nonvirtual VALUE Lookup (typename Configuration::ArgByValueType<KEY> key, const std::function<VALUE (typename Configuration::ArgByValueType<KEY>)>& cacheFiller);
                nonvirtual VALUE Lookup (typename Configuration::ArgByValueType<KEY> key, const VALUE& defaultValue);

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
                using MyMapType_ = map<KEY, MyResult_, Common::STL::less<KEY, typename TRAITS::WellOrderCompareFunctionType>>;
                MyMapType_ fMap_;
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TimedCache.inl"

#endif /*_Stroika_Foundation_Cache_TimedCache_h_*/
