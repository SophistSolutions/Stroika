/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_TimedCache_h_
#define _Stroika_Foundation_Cache_TimedCache_h_    1

#include    "../StroikaPreComp.h"

#include    <map>
#include    <mutex>

#include    "../Configuration/Common.h"
#include    "../Configuration/TypeHints.h"
#include    "../Characters/SDKChar.h"
#include    "../Debug/Assertions.h"
#include    "../Debug/AssertExternallySynchronizedLock.h"
#include    "../Memory/Optional.h"
#include    "../Time/Realtime.h"



/**
 *      \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 * TODO:
 *
 *      @todo   Add compare template param so can check key by non-standard compare
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



namespace   Stroika {
    namespace   Foundation {
        namespace   Cache {

            using Stroika::Foundation::Characters::SDKChar;


            namespace   TimedCacheSupport {


                /**
                 *  Helper detail class for analyzing and tuning cache statistics.
                 */
                struct  Stats_Basic {
                    Stats_Basic ();
                    size_t      fCachedCollected_Hits;
                    size_t      fCachedCollected_Misses;

                    nonvirtual  void    IncrementHits ();
                    nonvirtual  void    IncrementMisses ();
                    nonvirtual  void    DbgTraceStats (const Characters::SDKChar* label) const;
                };


                /**
                 *  Helper for DefaultTraits - when not collecting stats.
                 */
                struct  Stats_Null {
                    nonvirtual  void    IncrementHits ();
                    nonvirtual  void    IncrementMisses ();
                    nonvirtual  void    DbgTraceStats (const Characters::SDKChar* label) const;
                };


                /**
                 * The DefaultTraits<> is a simple default traits implementation for building an TimedCache<>.
                 */
                template    <typename   KEY, typename VALUE, bool TRACK_READ_ACCESS = false>
                struct  DefaultTraits {
                    using   KeyType     =   KEY;
                    using   ResultType  =   VALUE;

#if     qDebug
                    using   StatsType   =   Stats_Basic;
#else
                    using   StatsType   =   Stats_Null;
#endif
                    DEFINE_CONSTEXPR_CONSTANT(bool, kTrackReadAccess, TRACK_READ_ACCESS)
                };


            }


            /**
             *  Keeps track of all items - indexed by Key - but throws away items which are any more
             *  stale than given by the TIMEOUT
             *
             *  <TEMPORARILY>
             *      The only constraint on KEY is that it must have an operator== and operator<, and the only constraint on
             *      both KEY and VALUE is that they must be copyable objects.
             *  </TEMPORARILY>
             *
             *  \note   Note - this class doesn't employ a thread to throw away old items, so if you count on that
             *          happening (e.g. because the VALUE object DTOR has a side-effect like closing a file), then
             *          you may call DoBookkeeping () peridocially.
             *
             *
             *  \par Example Usage
             *      Use TimedCache to avoid needlessly redundant lookups
             *
             *      Assume 'LookupDiskStats_' returns DiskSpaceUsageType, but its expensive, and the results change only slowly...
             *
             *      \code
             *      Cache::TimedCache<String, DiskSpaceUsageType>   sDiskUsageCache_ { false, 5.0 };
             *
             *      Optional<DiskSpaceUsageType> LookupDiskStats (String diskName)
             *      {
             *          Optional<DiskSpaceUsageType>    o   =   sDiskUsageCache_.AccessElement (diskName);
             *          if (o.IsMissing ()) {
             *              o = LookupDiskStats_ ();
             *              if (o) {
             *                  sDiskUsageCache_.Add (diskName, *o);
             *              }
             *          }
             *          return o;
             *      }
             *      \endcode
             *
             *
             *  EXAMPLE
             *

             &&& not sure we want this example
                    struct  ScanFolderKey_ {
                        String                Volume;
                        String                      Folder;
                        bool operator== (const ScanFolderKey_& rhs) const { return Volume == rhs.Volume and Folder == rhs.Folder; }
                        bool operator< (const ScanFolderKey_& rhs) const { if (Volume == rhs.Volume) { return  Folder < rhs.Folder;  } else return Volume < rhs.Volume; }
                    };
                    static  constexpr   DurationSecondsType kAgeForScanPersistence_ { 5 * 60.0 };

                    struct FolderDetails_ {
                        int size;       // ...info to cache about a folder
                    };
                    mutable Synchronized<Cache::TimedCache<ScanFolderKey_, FolderDetails_>>   fScanFolderCache_ { true, kAgeForScanPersistence_ };

                    FolderDetails_ AccessFolder_ (const String& volume, const String& folder) const
                    {
                        auto lockedCache = fScanFolderCache_.GetReference ();
                        if (Optional<FolderDetails_> o  = lockedCache->AccessElement (ScanFolderKey_ { volume, folder })) {
                            return *o;
                        }
                        else {
                            FolderDetails_  constructed_details {}; // fill in details
                            lockedCache->Add (ScanFolderKey_ { volume, folder }, constructed_details);
                            return move (constructed_details);
                        }
                    }


             *  EXAMPLE - USE DNS CACHE... - or current use for LDAP lookups
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#ExternallySynchronized">ExternallySynchronized</a>
             *
             *  \note   Implementation Note: inherit from TRAITS::StatsType to take advantage of zero-sized base object rule.
             *
             */
            template    <typename   KEY, typename VALUE, typename TRAITS = TimedCacheSupport::DefaultTraits<KEY, VALUE>>
            class   TimedCache : private Debug::AssertExternallySynchronizedLock, private TRAITS::StatsType {
            public:
                using   TraitsType  =   TRAITS;

            public:
                /**
                 */
                TimedCache (Time::DurationSecondsType timeoutInSeconds);
                TimedCache (bool accessFreshensDate, Time::DurationSecondsType timeoutInSeconds);   /// DEPRECATED
                TimedCache (const TimedCache&) = default;

            public:
                nonvirtual  TimedCache& operator= (const TimedCache&) = default;

            public:
                /**
                 */
                nonvirtual  void    SetTimeout (Time::DurationSecondsType timeoutInSeconds);

            public:
                /**
                 *  Lookup the given value and return it if its in the Cache.
                 */
                nonvirtual  Memory::Optional<VALUE>     _DeprecatedFunction2_ (AccessElement, "Use Lookup") (typename Configuration::ArgByValueType<KEY> key);
                nonvirtual  bool                        _DeprecatedFunction2_ (AccessElement, "Use Lookup") (typename Configuration::ArgByValueType<KEY> key, VALUE* result);

            public:
                /**
                 *  Usually one will use this as
                 *      VALUE v = cache.Lookup (key, ts, [this] () -> VALUE {return this->realLookup(key); });
                 *
                 *  However, the overload returing an optional is occasionally useful, if you dont want to fill the cache
                 *  but just see if a value is present.
                 *
                 *  Both the overload with cacheFiller, and defaultValue will update the 'time stored' for the argument key.
                 */
                nonvirtual  Memory::Optional<VALUE> Lookup (typename Configuration::ArgByValueType<KEY> key);
                nonvirtual  VALUE                   Lookup (typename Configuration::ArgByValueType<KEY> key, const std::function<VALUE()>& cacheFiller);
                nonvirtual  VALUE                   Lookup (typename Configuration::ArgByValueType<KEY> key, const VALUE& defaultValue);

            public:
                /**
                 */
                nonvirtual  void    Add (typename Configuration::ArgByValueType<KEY> key, typename Configuration::ArgByValueType<VALUE> result);

            public:
                /**
                 */
                nonvirtual  void    _DeprecatedFunction2_ (AddElement, "Add") (typename Configuration::ArgByValueType<KEY> key, typename Configuration::ArgByValueType<VALUE> result)
                {
                    Add (key, result);
                }

            public:
                /**
                 */
                nonvirtual  void    Remove (typename Configuration::ArgByValueType<KEY> key);

            public:
                /**
                 */
                nonvirtual  void    DoBookkeeping ();   // optional - need not be called

            private:
                Time::DurationSecondsType   fTimeout_;
                Time::DurationSecondsType   fNextAutoClearAt_;

            private:
                nonvirtual  void    ClearIfNeeded_ ();
                nonvirtual  void    ClearOld_ ();

            private:
                struct  MyResult_ {
                    MyResult_ (const VALUE& r)
                        : fResult (r)
                        , fLastAccessedAt (Time::GetTickCount ())
                    {
                    }
                    VALUE                       fResult;
                    Time::DurationSecondsType   fLastAccessedAt;
                };
                map<KEY, MyResult_>   fMap_;
            };


        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "TimedCache.inl"

#endif  /*_Stroika_Foundation_Cache_TimedCache_h_*/
