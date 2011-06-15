/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Containers_LRUCache_h_
#define	_Stroika_Foundation_Containers_LRUCache_h_	1

#include	"../StroikaPreComp.h"

#include	<algorithm>
#include	<vector>

#include	"../Configuration/Basics.h"



/// THIS MODULE SB OBSOLETE ONCE WE GET STROIKA CONTAINERS WORKING


namespace	Stroika {	
	namespace	Foundation {
		namespace	Containers {


/*
@CONFIGVAR:		qKeepLRUCacheStats
@DESCRIPTION:	<p>Defines whether or not we capture statistics (for debugging purposes) in @'LRUCache<ELEMENT>'.
			This should be ON by default - iff @'qDebug' is true.</p>
 */
#ifndef	qKeepLRUCacheStats
#define	qKeepLRUCacheStats		defined (_DEBUG)
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
									fNext (NULL),
									fPrev (NULL),
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
									RequireNotNil (fCur);
									fCur = fCur->fNext;
									return *this;
								}
							ELEMENT& operator* ()
								{
									RequireNotNil (fCur);
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
						nonvirtual	CacheIterator	end ()		{ return NULL; }

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








/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace	Stroika {	
	namespace	Foundation {
		namespace	Containers {
//	class	LRUCache<ELEMENT>
	template	<typename	ELEMENT>
		LRUCache<ELEMENT>::LRUCache (size_t maxCacheSize):
				#if		qKeepLRUCacheStats
				fCachedCollected_Hits (0),
				fCachedCollected_Misses (0),
				#endif
				fCachedElts_BUF (),
				fCachedElts_First (NULL),
				fCachedElts_fLast (NULL)
			{
				SetMaxCacheSize (maxCacheSize);
			}
	template	<typename	ELEMENT>
		inline	size_t	LRUCache<ELEMENT>::GetMaxCacheSize () const
			{
				return fCachedElts_BUF.size ();
			}
	template	<typename	ELEMENT>
		void	LRUCache<ELEMENT>::SetMaxCacheSize (size_t maxCacheSize)
			{
				Require (maxCacheSize >= 1);
				if (maxCacheSize != fCachedElts_BUF.size ()) {
					fCachedElts_BUF.resize (maxCacheSize);
					// Initially link LRU together.
					fCachedElts_First = Containers::Start (fCachedElts_BUF);
					fCachedElts_fLast = fCachedElts_First + maxCacheSize-1;
					fCachedElts_BUF[0].fPrev = NULL;
					for (size_t i = 0; i < maxCacheSize-1; ++i) {
						fCachedElts_BUF[i].fNext = fCachedElts_First + (i+1);
						fCachedElts_BUF[i+1].fPrev = fCachedElts_First + (i);
					}
					fCachedElts_BUF[maxCacheSize-1].fNext = NULL;
				}
			}
	template	<typename	ELEMENT>
		inline	void	LRUCache<ELEMENT>::ShuffleToHead (CacheElement* b)
			{
				AssertNotNil (b);
				if (b == fCachedElts_First) {
					Assert (b->fPrev == NULL);
					return;	// already at head
				}
				CacheElement*	prev	=	b->fPrev;
				AssertNotNil (prev);					// don't call this if already at head
				// patch following and preceeding blocks to point to each other
				prev->fNext = b->fNext;
				if (b->fNext == NULL) {
					Assert (b == fCachedElts_fLast);
					fCachedElts_fLast = b->fPrev;
				}
				else {
					b->fNext->fPrev = prev;
				}

				// Now patch us into the head of the list
				CacheElement*	oldFirst	=	fCachedElts_First;
				AssertNotNil (oldFirst);
				b->fNext = oldFirst;
				oldFirst->fPrev = b;
				b->fPrev = NULL;
				fCachedElts_First = b;

				Ensure (fCachedElts_fLast != NULL and fCachedElts_fLast->fNext == NULL);
				Ensure (fCachedElts_First != NULL and fCachedElts_First == b and fCachedElts_First->fPrev == NULL and fCachedElts_First->fNext != NULL);
			}
	template	<typename	ELEMENT>
		inline	void	LRUCache<ELEMENT>::ClearCache ()
			{
				for (CacheElement* cur = fCachedElts_First; cur != NULL; cur = cur->fNext) {
					cur->fElement.Clear ();
				}
			}
	template	<typename	ELEMENT>
		/*
		@METHOD:		LRUCache<ELEMENT>::LookupElement
		@DESCRIPTION:	<p>Check and see if the given element is in the cache. Return that element if its tehre, and NULL otherwise.
					Note that this routine re-orders the cache so that the most recently looked up element is first, and because
					of this re-ordering, its illegal to do a Lookup while a @'LRUCache<ELEMENT>::CacheIterator' exists
					for this LRUCache.</p>
		*/
		inline	ELEMENT*	LRUCache<ELEMENT>::LookupElement (const COMPARE_ITEM& item)
			{
				for (CacheElement* cur = fCachedElts_First; cur != NULL; cur = cur->fNext) {
					if (ELEMENT::Equal (cur->fElement, item)) {
						ShuffleToHead (cur);
						#if		qKeepLRUCacheStats
							fCachedCollected_Hits++;
						#endif
						return &fCachedElts_First->fElement;
					}
				}
				#if		qKeepLRUCacheStats
					fCachedCollected_Misses++;
				#endif
				return NULL;
			}
	template	<typename	ELEMENT>
		/*
		@METHOD:		LRUCache<ELEMENT>::AddNew
		@DESCRIPTION:	<p>Create a new LRUCache element (potentially bumping some old element out of the cache). This new element
					will be considered most recently used. Note that this routine re-orders the cache so that the most recently looked
					up element is first, and because of this re-ordering, its illegal to do a Lookup while 
					a @'LRUCache<ELEMENT>::CacheIterator' exists for this LRUCache.</p>
		*/
		inline	ELEMENT*	LRUCache<ELEMENT>::AddNew ()
			{
				ShuffleToHead (fCachedElts_fLast);
				return &fCachedElts_First->fElement;
			}

		}
	}
}



#endif	/*_Stroika_Foundation_Containers_LRUCache_h_*/
