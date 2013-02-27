This folder contains all the Stroika Library Foundation::Cache source code.

Cache objects are used to store data that can be otherwise reconstructed. The
purpose of a cache is to provide quick access to data - to avoid having to
recompute (or fetch) it. The difference between the different kinds of caches
revolve around what strategy they employ to throw items away, and to identify
what items are cached.

Cache Types:

	>	BloomFilter
		(NYI)

	>	LRUCache
		This cache stores a fixed amount of data, and as you access the data from the cache
		throws away old data when its not been accessed in a while.

		This is well implemented, but the API will likely evolve before release (more Iterator<T> like
		etc).

	>	TimedCache
		...This is implemented and used, but again, very imature at this point -- LGP 2013-02-26


TODO:
	>	Add some sort of 'memoize' module
		Think about memcached. To some extent, however, LRUCache and TimedCache
		already replicate this functionality.
