#include "Common.h"

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef	_Stroika_Foundation_Containers_LRUCache_inl_
#define	_Stroika_Foundation_Containers_LRUCache_inl_	1

#include	"../Debug/Assertions.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Containers {

			
			//	class	LRUCache<ELEMENT,TRAITS>
				template	<typename	ELEMENT, typename TRAITS>
					LRUCache<ELEMENT,TRAITS>::LRUCache (size_t maxCacheSize):
							#if		qKeepLRUCacheStats
							fCachedCollected_Hits (0),
							fCachedCollected_Misses (0),
							#endif
							fCachedElts_BUF (),
							fCachedElts_First (nullptr),
							fCachedElts_fLast (nullptr)
						{
							SetMaxCacheSize (maxCacheSize);
						}
				template	<typename	ELEMENT, typename TRAITS>
					inline	size_t	LRUCache<ELEMENT,TRAITS>::GetMaxCacheSize () const
						{
							return fCachedElts_BUF.size ();
						}
				template	<typename	ELEMENT, typename TRAITS>
					void	LRUCache<ELEMENT,TRAITS>::SetMaxCacheSize (size_t maxCacheSize)
						{
							Require (maxCacheSize >= 1);
							if (maxCacheSize != fCachedElts_BUF.size ()) {
								fCachedElts_BUF.resize (maxCacheSize);
								// Initially link LRU together.
								fCachedElts_First = Containers::Start (fCachedElts_BUF);
								fCachedElts_fLast = fCachedElts_First + maxCacheSize-1;
								fCachedElts_BUF[0].fPrev = nullptr;
								for (size_t i = 0; i < maxCacheSize-1; ++i) {
									fCachedElts_BUF[i].fNext = fCachedElts_First + (i+1);
									fCachedElts_BUF[i+1].fPrev = fCachedElts_First + (i);
								}
								fCachedElts_BUF[maxCacheSize-1].fNext = nullptr;
							}
						}
				template	<typename	ELEMENT, typename TRAITS>
					inline	void	LRUCache<ELEMENT,TRAITS>::ShuffleToHead_ (CacheElement* b)
						{
							AssertNotNull (b);
							if (b == fCachedElts_First) {
								Assert (b->fPrev == nullptr);
								return;	// already at head
							}
							CacheElement*	prev	=	b->fPrev;
							AssertNotNull (prev);					// don't call this if already at head
							// patch following and preceeding blocks to point to each other
							prev->fNext = b->fNext;
							if (b->fNext == nullptr) {
								Assert (b == fCachedElts_fLast);
								fCachedElts_fLast = b->fPrev;
							}
							else {
								b->fNext->fPrev = prev;
							}

							// Now patch us into the head of the list
							CacheElement*	oldFirst	=	fCachedElts_First;
							AssertNotNull (oldFirst);
							b->fNext = oldFirst;
							oldFirst->fPrev = b;
							b->fPrev = nullptr;
							fCachedElts_First = b;

							Ensure (fCachedElts_fLast != nullptr and fCachedElts_fLast->fNext == nullptr);
							Ensure (fCachedElts_First != nullptr and fCachedElts_First == b and fCachedElts_First->fPrev == nullptr and fCachedElts_First->fNext != nullptr);
						}
				template	<typename	ELEMENT, typename TRAITS>
					inline	void	LRUCache<ELEMENT,TRAITS>::ClearCache ()
						{
							for (CacheElement* cur = fCachedElts_First; cur != nullptr; cur = cur->fNext) {
								cur->fElement.Clear ();
							}
						}
				template	<typename	ELEMENT, typename TRAITS>
					/*
					@METHOD:		LRUCache<ELEMENT>::LookupElement
					@DESCRIPTION:	<p>Check and see if the given element is in the cache. Return that element if its tehre, and nullptr otherwise.
								Note that this routine re-orders the cache so that the most recently looked up element is first, and because
								of this re-ordering, its illegal to do a Lookup while a @'LRUCache<ELEMENT>::CacheIterator' exists
								for this LRUCache.</p>
					*/
					inline	ELEMENT*	LRUCache<ELEMENT,TRAITS>::LookupElement (const COMPARE_ITEM& item)
						{
							for (CacheElement* cur = fCachedElts_First; cur != nullptr; cur = cur->fNext) {
								if (ELEMENT::Equal (cur->fElement, item)) {
									ShuffleToHead_ (cur);
									#if		qKeepLRUCacheStats
										fCachedCollected_Hits++;
									#endif
									return &fCachedElts_First->fElement;
								}
							}
							#if		qKeepLRUCacheStats
								fCachedCollected_Misses++;
							#endif
							return nullptr;
						}
				template	<typename	ELEMENT, typename TRAITS>
					/*
					@METHOD:		LRUCache<ELEMENT>::AddNew
					@DESCRIPTION:	<p>Create a new LRUCache element (potentially bumping some old element out of the cache). This new element
								will be considered most recently used. Note that this routine re-orders the cache so that the most recently looked
								up element is first, and because of this re-ordering, its illegal to do a Lookup while 
								a @'LRUCache<ELEMENT>::CacheIterator' exists for this LRUCache.</p>
					*/
					inline	ELEMENT*	LRUCache<ELEMENT,TRAITS>::AddNew ()
						{
							ShuffleToHead_ (fCachedElts_fLast);
							return &fCachedElts_First->fElement;
						}

		}
	}
}
#endif	/*_Stroika_Foundation_Containers_LRUCache_inl_*/
