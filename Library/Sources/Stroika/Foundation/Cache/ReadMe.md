# Stroika Library [Foundation](../)::Cache

This folder contains all the Stroika Library [Foundation](../)::Cache source code.

Cache objects are used to store data that can be otherwise reconstructed. The
purpose of a cache is to provide quick access to data - to avoid having to
recompute (or fetch) it. The difference between the different kinds of caches
revolve around what strategy they employ to throw items away, and to identify
what items are cached.

Cache Types:

- BloomFilter - [BloomFilter.h](BloomFilter.h)

  a Bloom filter is a probablistic set, which returns either "probably in set" or "definitely not in set"

- LRUCache - [LRUCache.h](LRUCache.h)

  This cache stores a fixed (capped) amount of data, and as you access the data from the cache throws away old data when its not been accessed in a while.

- TimedCache - [TimedCache.h](TimedCache.h)

  Keeps track of all items - indexed by Key - but throws away items which are any more stale than given by the minimumAllowedFreshness (associated with the cache, not passed in to each lookup call). Staleness is defined as time since item was added.

  Also supports tracking 'expires' (instead of freshness).

  Also implements older CallerStalenessCache (global TTL on data, but the TTL is specified on each call to lookup) via same template (slight changes).

Other:

- Memoizer - [Memoizer.h](Memoizer.h)

  Cache (expensive) computation results transparently, using one of the Stroika cache types as a backend/plugin.

- [Statistics.h](Statistics.h)
- [SynchronizedLRUCache.h](SynchronizedLRUCache.h)

TODO:

- PROBABLY add a front-end to memcached, redis!!! Maybe it look like a stroika cache object!
