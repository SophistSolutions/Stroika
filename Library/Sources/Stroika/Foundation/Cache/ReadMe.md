# Stroika Library [Foundation](../ReadMe.md)::Cache

This folder contains all the Stroika Library [Foundation](../ReadMe.md)::Cache source code.

Cache objects are used to store data that can be otherwise reconstructed. The
purpose of a cache is to provide quick access to data - to avoid having to
recompute (or fetch) it. The difference between the different kinds of caches
revolve around what strategy they employ to throw items away, and to identify
what items are cached.

Cache Types:

- BloomFilter - [BloomFilter.h](BloomFilter.h)

  a Bloom filter is a probablistic set

- CallerStalenessCache

  Track when something is added to the cache, and then the lookup function can avoid a costly call to compute something if its been recently enough added. Like TimedCache, except there is no global TTL on data, but the TTL is specified on each call to lookup.

- LRUCache - [LRUCache.h](LRUCache.h)

  This cache stores a fixed amount of data, and as you access the data from the cache throws away old data when its not been accessed in a while.

- TimedCache - [TimedCache.h](TimedCache.h)

  Keeps track of all items - indexed by Key - but throws away items which are any more stale than given by the TIMEOUT. Staleness is defined as time since item was added.

Other:

- Memoizer - [Memoizer.h](Memoizer.h)

  Cache (expensive) computation results transparently, using one of the Stroika cache types as a backend/plugin.

TODO:

- PROBABLY add a front-end to memcached, redis!!! Maybe it look like a stroika cache object!
