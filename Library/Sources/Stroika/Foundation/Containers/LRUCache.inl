/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Containers_LRUCache_inl_
#define	_Stroika_Foundation_Containers_LRUCache_inl_	1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"../Debug/Assertions.h"

#include	"Common.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Containers {


			//	class	LRUCacheDefaultTraits<ELEMENT,TRAITS>
			template	<typename	ELEMENT, typename KEY>
				inline	typename	LRUCacheDefaultTraits<ELEMENT,KEY>::KeyType	LRUCacheDefaultTraits<ELEMENT,KEY>::ExtractKey (const ElementType& e)
					{
						return e;
					}
			template	<typename	ELEMENT, typename KEY>
				inline	size_t	LRUCacheDefaultTraits<ELEMENT,KEY>::Hash (const KeyType& e)
						{
							return 0;
						}
			template	<typename	ELEMENT, typename KEY>
				inline	void	LRUCacheDefaultTraits<ELEMENT,KEY>::Clear (ElementType* element)
					{
						(*element) = ElementType ();
					}
			template	<typename	ELEMENT, typename KEY>
				inline	bool	LRUCacheDefaultTraits<ELEMENT,KEY>::Equal (const KeyType& lhs, const KeyType& rhs)
					{
						return lhs == rhs;
					}




			//	class	LRUCache<ELEMENT,TRAITS>::CacheElement
			template	<typename	ELEMENT, typename TRAITS>
				inline	LRUCache<ELEMENT,TRAITS>::CacheElement::CacheElement ()
						: fNext (nullptr)
						, fPrev (nullptr)
						, fElement ()
					{
					}



			//	class	LRUCache<ELEMENT,TRAITS>::CacheIterator
			template	<typename	ELEMENT, typename TRAITS>
				inline	LRUCache<ELEMENT,TRAITS>::CacheIterator::CacheIterator (CacheElement* c)
					: fCur (c)
					{
					}
			template	<typename	ELEMENT, typename TRAITS>
				inline	typename	LRUCache<ELEMENT,TRAITS>::CacheIterator&	LRUCache<ELEMENT,TRAITS>::CacheIterator::operator++ ()
					{
						RequireNotNull (fCur);
						fCur = fCur->fNext;
						return *this;
					}
			template	<typename	ELEMENT, typename TRAITS>
				inline	ELEMENT&	LRUCache<ELEMENT,TRAITS>::CacheIterator::operator* ()
					{
						RequireNotNull (fCur);
						return fCur->fElement;
					}
			template	<typename	ELEMENT, typename TRAITS>
				inline	bool LRUCache<ELEMENT,TRAITS>::CacheIterator::operator== (CacheIterator rhs)
					{
						return fCur == rhs.fCur;
					}
			template	<typename	ELEMENT, typename TRAITS>
				inline	bool LRUCache<ELEMENT,TRAITS>::CacheIterator::operator!= (CacheIterator rhs)
					{
						return fCur != rhs.fCur;
					}


			//	class	LRUCache<ELEMENT,TRAITS>
				template	<typename	ELEMENT, typename TRAITS>
					LRUCache<ELEMENT,TRAITS>::LRUCache (size_t maxCacheSize)
							: fCachedElts_BUF_ ()
							, fCachedElts_First_ (nullptr)
							, fCachedElts_fLast_ (nullptr)
							#if		qKeepLRUCacheStats
							, fCachedCollected_Hits (0)
							, fCachedCollected_Misses (0)
							#endif
						{
							SetMaxCacheSize (maxCacheSize);
						}
				template	<typename	ELEMENT, typename TRAITS>
					inline	size_t	LRUCache<ELEMENT,TRAITS>::GetMaxCacheSize () const
						{
							return fCachedElts_BUF_.size ();
						}
				template	<typename	ELEMENT, typename TRAITS>
					void	LRUCache<ELEMENT,TRAITS>::SetMaxCacheSize (size_t maxCacheSize)
						{
							Require (maxCacheSize >= 1);
							if (maxCacheSize != fCachedElts_BUF_.size ()) {
								fCachedElts_BUF_.resize (maxCacheSize);
								// Initially link LRU together.
								fCachedElts_First_ = Containers::Start (fCachedElts_BUF_);
								fCachedElts_fLast_ = fCachedElts_First_ + maxCacheSize-1;
								fCachedElts_BUF_[0].fPrev = nullptr;
								for (size_t i = 0; i < maxCacheSize-1; ++i) {
									fCachedElts_BUF_[i].fNext = fCachedElts_First_ + (i+1);
									fCachedElts_BUF_[i+1].fPrev = fCachedElts_First_ + (i);
								}
								fCachedElts_BUF_[maxCacheSize-1].fNext = nullptr;
							}
						}
				template	<typename	ELEMENT, typename TRAITS>
					inline	typename	LRUCache<ELEMENT,TRAITS>::CacheIterator	LRUCache<ELEMENT,TRAITS>::begin ()
						{
							return fCachedElts_First_;
						}
				template	<typename	ELEMENT, typename TRAITS>
					inline	typename	LRUCache<ELEMENT,TRAITS>::CacheIterator	LRUCache<ELEMENT,TRAITS>::end ()
						{
							return nullptr;
						}
				template	<typename	ELEMENT, typename TRAITS>
					inline	void	LRUCache<ELEMENT,TRAITS>::ShuffleToHead_ (CacheElement* b)
						{
							AssertNotNull (b);
							if (b == fCachedElts_First_) {
								Assert (b->fPrev == nullptr);
								return;	// already at head
							}
							CacheElement*	prev	=	b->fPrev;
							AssertNotNull (prev);					// don't call this if already at head
							// patch following and preceeding blocks to point to each other
							prev->fNext = b->fNext;
							if (b->fNext == nullptr) {
								Assert (b == fCachedElts_fLast_);
								fCachedElts_fLast_ = b->fPrev;
							}
							else {
								b->fNext->fPrev = prev;
							}

							// Now patch us into the head of the list
							CacheElement*	oldFirst	=	fCachedElts_First_;
							AssertNotNull (oldFirst);
							b->fNext = oldFirst;
							oldFirst->fPrev = b;
							b->fPrev = nullptr;
							fCachedElts_First_ = b;

							Ensure (fCachedElts_fLast_ != nullptr and fCachedElts_fLast_->fNext == nullptr);
							Ensure (fCachedElts_First_ != nullptr and fCachedElts_First_ == b and fCachedElts_First_->fPrev == nullptr and fCachedElts_First_->fNext != nullptr);
						}
				template	<typename	ELEMENT, typename TRAITS>
					inline	void	LRUCache<ELEMENT,TRAITS>::ClearCache ()
						{
							for (CacheElement* cur = fCachedElts_First_; cur != nullptr; cur = cur->fNext) {
								TRAITS::Clear (&cur->fElement);
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
					inline	ELEMENT*	LRUCache<ELEMENT,TRAITS>::LookupElement (const KeyType& item)
						{
							for (CacheElement* cur = fCachedElts_First_; cur != nullptr; cur = cur->fNext) {
								if (TRAITS::Equal (TRAITS::ExtractKey (cur->fElement), item)) {
									ShuffleToHead_ (cur);
									#if		qKeepLRUCacheStats
										fCachedCollected_Hits++;
									#endif
									return &fCachedElts_First_->fElement;
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
							ShuffleToHead_ (fCachedElts_fLast_);
							return &fCachedElts_First_->fElement;
						}

		}
	}
}
#endif	/*_Stroika_Foundation_Containers_LRUCache_inl_*/
