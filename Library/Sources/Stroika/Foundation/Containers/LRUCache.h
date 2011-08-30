/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Containers_LRUCache_h_
#define	_Stroika_Foundation_Containers_LRUCache_h_	1

#include	"../StroikaPreComp.h"

#include	<vector>

#include	"../Configuration/Common.h"

// This is temporarily needed in .h file because we do Require() stuff in .h - but move to .inl before long....
#include	"../Debug/Assertions.h"



/// THIS MODULE SB OBSOLETE ONCE WE GET STROIKA CONTAINERS WORKING



// THIS CODE NEEDS BIG CLEANUP. USE 'TRAITS' style stuff 

// ALSO - KDJ - suggestion/hint - provide MAYBE OPTIONAL HASH function (through traits). THEN - the LRUCache mechanism can 
// store more elements efficeintly. Right now - LRU cache really just works for small numbers of items
// NOTE - if user uses HASHING - then its not stricly LRU - just LRU per-hash element/value
// Discuss with KDJ since it was his idea (indirectly) Maybe have HashedLRUCache as a separate impl? Maybe generically
// do LRUCache (liek stroika style containers) and have differnt impls)


namespace	Stroika {	
	namespace	Foundation {
		namespace	Containers {


			/*
			@CONFIGVAR:		qKeepLRUCacheStats
			@DESCRIPTION:	<p>Defines whether or not we capture statistics (for debugging purposes) in @'LRUCache<ELEMENT>'.
			 */
			#if		!defined (qKeepLRUCacheStats)
				#error "qKeepLRUCacheStats should normally be defined indirectly by StroikaConfig.h"
			#endif



/*
@CLASS:			LRUCache<ELEMENT>
@DESCRIPTION:	<p>A basic LRU (least recently used) cache mechanism. You provide a class type argument 'ELEMENT' defined roughly as follows:
				<br>
				<code>
					struct	ELEMENT {
						struct	COMPARE_ITEM;
						nonvirtual	void	Clear ();
						static		bool	Equal (const ELEMENT&, const COMPARE_ITEM&);
					};
				</code>
				</p>
				<p>The <code>COMPARE_ITEM</code> is an object which defines the attributes which make the given item UNIQUE (for Lookup purposes).
			Think of it as the KEY. The <code>Clear ()</code> method must be provided to invaliate the given item (usually by setting part of the COMPARE_ITEM
			to an invalid value) so it won't get found by a Lookup. The 'Equal' method compares an element and a COMPARE_ITEM in the Lookup method.
				</p>
				<p>Note that the type 'ELEMENT' must be copyable (though its rarely copied - just in response
			to a @'LRUCache<ELEMENT>::SetMaxCacheSize' call.</p>
				<p>To iterate over the elements of the cache - use an @'LRUCache<ELEMENT>::CacheIterator'.
				<p>Note this class is NOT THREADSAFE, and must be externally locked. This is because it returns pointers
			to internal data structures (the cached elements).
			</p>
*/
			template	<typename	ELEMENT>
				class	LRUCache {
					public:
						typedef	typename ELEMENT::COMPARE_ITEM	COMPARE_ITEM;

					public:
						LRUCache (size_t maxCacheSize);

					public:
						nonvirtual	size_t	GetMaxCacheSize () const;
						nonvirtual	void	SetMaxCacheSize (size_t maxCacheSize);

					public:
						struct	CacheElement {
							public:
								CacheElement ():
									fNext (nullptr),
									fPrev (nullptr),
									fElement ()
									{
									}

							public:
								CacheElement*	fNext;
								CacheElement*	fPrev;

							public:
								ELEMENT			fElement;
						};

					public:
						/*
						@CLASS:			LRUCache<ELEMENT>::CacheIterator
						@DESCRIPTION:	<p>Used to iterate over elements of an @'LRUCache<ELEMENT>'</p>
										<p>Please note that while an CacheIterator object exists for an LRUCache - it is not
									safe to do other operations on the LRUCache - like @'LRUCache<ELEMENT>::LookupElement' or @'LRUCache<ELEMENT>::AddNew'.
									</p>
						*/
						struct	CacheIterator {
							CacheIterator (CacheElement* c): fCur (c) {}
							CacheElement*	fCur;
							CacheIterator& operator++ ()
								{
									RequireNotNull (fCur);
									fCur = fCur->fNext;
									return *this;
								}
							ELEMENT& operator* ()
								{
									RequireNotNull (fCur);
									return fCur->fElement;
								}
							bool operator== (CacheIterator rhs)
								{
									return fCur == rhs.fCur;
								}
							bool operator!= (CacheIterator rhs)
								{
									return fCur != rhs.fCur;
								}
						};
						nonvirtual	CacheIterator	begin ()	{ return fCachedElts_First; }
						nonvirtual	CacheIterator	end ()		{ return nullptr; }

					public:
						nonvirtual	void	ClearCache ();

					public:
						nonvirtual	ELEMENT*	LookupElement (const COMPARE_ITEM& item);
						nonvirtual	ELEMENT*	AddNew ();

					#if		qKeepLRUCacheStats
					public:
						size_t		fCachedCollected_Hits;
						size_t		fCachedCollected_Misses;
					#endif

					private:
						vector<CacheElement>	fCachedElts_BUF;
						CacheElement*			fCachedElts_First;
						CacheElement*			fCachedElts_fLast;

					private:
						nonvirtual	void	ShuffleToHead (CacheElement* b);
				};


		}
	}
}
#endif	/*_Stroika_Foundation_Containers_LRUCache_h_*/







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"LRUCache.inl"
