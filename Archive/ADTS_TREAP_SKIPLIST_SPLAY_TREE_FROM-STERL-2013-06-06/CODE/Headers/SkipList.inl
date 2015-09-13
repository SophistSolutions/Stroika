#pragma once

#include <math.h>

#include <random>

#include "../Shared/Headers/Utils.h"

#if qDebug
	#include <iostream>

    #include "../Shared/Headers/ContainerValidation.h"
    #include "../Shared/Headers/HashKey.h"
#endif

namespace   ADT {


template <typename KEY, typename VALUE,typename TRAITS>
SkipList<KEY,VALUE,TRAITS>::SkipList () :
	fLength (0)
  #if qKeepADTStatistics
      ,fCompares (0)
      ,fRotations (0)
  #endif
{
	GrowHeadLinksIfNeeded (1, nullptr);
}

template <typename KEY, typename VALUE,typename TRAITS>
SkipList<KEY,VALUE,TRAITS>::SkipList (const SkipList& s) :
	fLength (0)
  #if qKeepADTStatistics
      ,fCompares (0)
      ,fRotations (0)
  #endif
{
	operator= (s);
}


template <typename KEY, typename VALUE,typename TRAITS>
SkipList<KEY,VALUE,TRAITS>& SkipList<KEY,VALUE,TRAITS>::operator= (const SkipList& t)
{
	RemoveAll ();
	if (t.GetLength () != 0) {
		Node*	prev = nullptr;
		Node*	n = (t.fHead.size () == 0) ? nullptr : t.fHead[0];
		while (n != nullptr) {
			Node* newNode = new Node (n->fEntry.GetKey (), n->fEntry.GetValue ());
			if (prev == nullptr) {
				Assert (fHead.size () == 1);
				Assert (fHead[0] == nullptr);
				fHead[0] = newNode;
			}
			else {
				prev->fNext.push_back (newNode);
			}
			prev = newNode;
			n = n->fNext[0];
		}
		AssertNotNull (prev);
		Assert (prev->fNext.size () == 0);
		prev->fNext.push_back (nullptr);

		fLength = t.fLength;
		Balance ();	// this will give us a proper link structure
	}
	return *this;
}

template <typename KEY, typename VALUE,typename TRAITS>
SkipList<KEY,VALUE,TRAITS>::~SkipList ()
{
	RemoveAll ();
}


template <typename KEY, typename VALUE,typename TRAITS>
inline	size_t	SkipList<KEY,VALUE,TRAITS>::GetLinkHeightProbability ()
{
	return 25;
}


template <typename KEY, typename VALUE,typename TRAITS>
inline	size_t	SkipList<KEY,VALUE,TRAITS>::GetLength () const
{
	return fLength;
}


template <typename KEY, typename VALUE,typename TRAITS>
typename SkipList<KEY,VALUE,TRAITS>::Node*	SkipList<KEY,VALUE,TRAITS>::FindNode (const KeyType& key)  const
{
	Assert (fHead.size () > 0);

	std::vector<Node*> const* startV = &fHead;
	for (size_t linkHeight = fHead.size (); linkHeight > 0; --linkHeight) {
		Node*	n = (*startV)[linkHeight-1];

		// tweak to use pointer comparisons rather than key field compares. We know any link heigher than the current link being
		// tested must point past the key we are looking for, so we can compare our current node with that one and skip the
		// test if they are the same. In practice, seems to avoid 3-10% of all compares
		Node*	overShotNode = (startV->size () <= linkHeight) ? nullptr : (*startV)[linkHeight];
		while (n != overShotNode) {
			#if qKeepADTStatistics
				const_cast<SkipList<KEY,VALUE,TRAITS> *> (this)->fCompares++;
			#endif

			int comp =TRAITS::Comparer::Compare (n->fEntry.GetKey (), key);
			if (comp == 0) {
				return n;
			}
			else if (comp < 0) {
				startV = &n->fNext;
				n = n->fNext[linkHeight-1];
			}
			else {
				break;	// overshot
			}
		}
	}

	return nullptr;
}

template <typename KEY, typename VALUE,typename TRAITS>
bool	SkipList<KEY,VALUE,TRAITS>::Find (const KeyType& key, ValueType* val)  const
{
	Node* n = FindNode (key);
	if (n != nullptr) {
		if (val != nullptr) {
			*val = n->fEntry.GetValue ();
		}
		return true;
	}
	return false;
}

template <typename KEY, typename VALUE,typename TRAITS>
void	SkipList<KEY,VALUE,TRAITS>::Add (const KeyType& key, const ValueType& val)
{
	std::vector<Node*>	links;

	size_t     minLinkHeight = 0;
 	Node* n = FindNearest (key, links);
 	if (n != nullptr) {
 	    if (TRAITS::kPolicy & ADT::eDuplicateAddThrowException) {
 	        throw DuplicateAddException ();;
 	    }
 	    else {
 	        minLinkHeight = n->fNext.size ();   // trick to make sure new node is found first (it is already inserted first)
 	    }
 	}
	AddNode (new Node (key, val), links, minLinkHeight);
}

template <typename KEY, typename VALUE,typename TRAITS>
void	SkipList<KEY,VALUE,TRAITS>::Add (const KeyType& keyAndValue)
{
    Add (keyAndValue, keyAndValue);
}


template <typename KEY, typename VALUE,typename TRAITS>
void	SkipList<KEY,VALUE,TRAITS>::AddNode (Node* node, const std::vector<Node*>& links, size_t minLinkHeight)
{
	RequireNotNull (node);
	size_t	newLinkHeight = std::max (DetermineLinkHeight (), minLinkHeight);
	node->fNext.resize (newLinkHeight);

    size_t	linksToPatch = std::min (fHead.size (), newLinkHeight);
	for (size_t i = 0; i < linksToPatch; ++i) {
		Node*	nextL = nullptr;
		if (links[i] == nullptr) {
			nextL = fHead[i];
			fHead[i] = node;
		}
		else {
			#if qKeepADTStatistics
				const_cast<SkipList<KEY,VALUE,TRAITS> *> (this)->fRotations++;
			#endif
			Node*	oldLink = links[i];
			AssertNotNull (oldLink);
			nextL = oldLink->fNext[i];
			oldLink->fNext[i] = node;
		}

		node->fNext[i] = nextL;
	}

	GrowHeadLinksIfNeeded (newLinkHeight, node);
	++fLength;
}

template <typename KEY, typename VALUE,typename TRAITS>
void	SkipList<KEY,VALUE,TRAITS>::Remove (const KeyType& key)
{
	std::vector<Node*>	links;
	Node*	n = FindNearest (key, links);
	if (n != nullptr) {
		RemoveNode (n, links);
	}
	else {
		if (not (TRAITS::kPolicy & ADT::eInvalidRemoveIgnored)) {
		    throw InvalidRemovalException ();
		}
	}
}

template <typename KEY, typename VALUE,typename TRAITS>
void	SkipList<KEY,VALUE,TRAITS>::RemoveNode (Node* n, const std::vector<Node*>& links)
{
	for (auto it = links.begin (); it != links.end (); ++it) {
		size_t index = it - links.begin ();
		Node**	patchNode = (*it == nullptr) ? &fHead[index] : &(*it)->fNext[index];
		if (*patchNode == n) {
			#if qKeepADTStatistics
				const_cast<SkipList<KEY,VALUE,TRAITS> *> (this)->fRotations++;
			#endif
			*patchNode = n->fNext[index];
		}
		else {
			break;
		}
	}
	if (n->fNext.size () == fHead.size ()) {
		ShrinkHeadLinksIfNeeded ();
	}
	delete n;

	--fLength;
}

template <typename KEY, typename VALUE,typename TRAITS>
size_t	SkipList<KEY,VALUE,TRAITS>::DetermineLinkHeight () const
{
	enum {
		kMaxNewGrowth = 1
	};

	int	linkHeight = 1;
    int	maxHeight = std::min (fHead.size ()+kMaxNewGrowth, size_t (kMaxLinkHeight));
	while ((linkHeight < maxHeight) and (RandomSize_t (1, 100) <= GetLinkHeightProbability ())) {
		++linkHeight;
	}
	return linkHeight;
}


template <typename KEY, typename VALUE,typename TRAITS>
void	SkipList<KEY,VALUE,TRAITS>::GrowHeadLinksIfNeeded (size_t newSize, Node* nodeToPointTo)
{
	if (newSize > fHead.size ()) {
		fHead.resize (newSize, nodeToPointTo);
		Assert (fHead[newSize-1] == nodeToPointTo);
	}
}

template <typename KEY, typename VALUE,typename TRAITS>
void	SkipList<KEY,VALUE,TRAITS>::ShrinkHeadLinksIfNeeded ()
{
	if (fHead.size () <= 1) return;

	for (size_t i = fHead.size ()-1; i > 1; --i) {
		if (fHead[i] == nullptr) {
			fHead.pop_back ();
		}
	}
}

template <typename KEY, typename VALUE,typename TRAITS>
void	SkipList<KEY,VALUE,TRAITS>::RemoveAll ()
{
	Node*	link = (fHead.size () == 0) ? nullptr : fHead[0];
	while (link != nullptr) {
		Node*	nextLink = link->fNext[0];
		delete link;
		link = nextLink;
	}
	fHead.resize (1);
	fHead[0] = nullptr;
	fLength = 0;

	Ensure (GetLength () == 0);
}

template <typename KEY, typename VALUE,typename TRAITS>
typename SkipList<KEY,VALUE,TRAITS>::Node*	SkipList<KEY,VALUE,TRAITS>::FindNearest (KeyType key, std::vector<Node*>& links) const
{
	Require (links.size () == 0);	// we want to be passed in a totally empty vector
	Assert (fHead.size () > 0);

	links = fHead;

	Node*	newOverShotNode = nullptr;
	Node*	foundNode = nullptr;
	size_t	linkIndex = links.size ()-1;
	do {
		Node*	n = links[linkIndex];

		// tweak to use pointer comparisons rather than key field compares. We know any link heigher than the current link being
		// tested must point past the key we are looking for, so we can compare our current node with that one and skip the
		// test if they are the same. In practice, seems to avoid 3-10% of all compares
		Node*	overShotNode = newOverShotNode;
		Assert (n == nullptr or overShotNode == nullptr or (TRAITS::Comparer::Compare (n->fEntry.GetKey (),overShotNode->fEntry.GetKey ()))  <= 0);

		links[linkIndex] = nullptr;
		while (n != overShotNode) {
			#if qKeepADTStatistics
				const_cast<SkipList<KEY,VALUE,TRAITS> *> (this)->fCompares++;
			#endif

			int comp = TRAITS::Comparer::Compare (n->fEntry.GetKey (), key);
			if (comp == 0) {
				foundNode = n;
				newOverShotNode = foundNode;
				break;
			}
			else if (comp < 0) {
				links[linkIndex] = n;
				n = n->fNext[linkIndex];
				newOverShotNode = n;
			}
			else {
				break;
			}
		}
		if ((linkIndex > 0) and (links[linkIndex] != nullptr)) {
			links[linkIndex-1] = links[linkIndex];
		}

	} while (linkIndex-- != 0);

	return foundNode;
}

template <typename KEY, typename VALUE,typename TRAITS>
typename SkipList<KEY,VALUE,TRAITS>::Node*	SkipList<KEY,VALUE,TRAITS>::FindNearest (const Iterator& it, std::vector<Node*>& links) const
{
    Node*   n = FindNearest (it->GetKey (), links);
    AssertNotNull (n);
    Assert (TRAITS::Comparer::Compare (n->fEntry.GetKey (),it->GetKey ())  == 0);

    if (not (TRAITS::kPolicy & ADT::eDuplicateAddThrowException)) {
        // not necessarily the correct node, just one that has the same key
        // however, it should at least be the first in the list, so can scan forwards for correct one
        while (n != it.fCurrent) {
            Node* next = n->fNext[0];
            Assert (TRAITS::Comparer::Compare (next->fEntry.GetKey (),it->GetKey ())  == 0); // else we were passed in a node not in the list
            for (size_t i = 0; i < links.size (); ++i) {
                if (links[i] != nullptr and links[i]->fNext[i] == n) {
                    links[i] = n;
                }
                else {
                    break;
                }
            }
            n = next;
        }
    }
    Assert (n == it.fCurrent);

    return n;
}

template <typename KEY, typename VALUE,typename TRAITS>
typename SkipList<KEY,VALUE,TRAITS>::Node*		SkipList<KEY,VALUE,TRAITS>::FirstNode () const
{
    if (fHead.size () == 0) {
        return nullptr;
    }
    return (fHead[0]);
}

template <typename KEY, typename VALUE,typename TRAITS>
typename SkipList<KEY,VALUE,TRAITS>::Node*		SkipList<KEY,VALUE,TRAITS>::LastNode () const
{
    if (fHead.size () == 0) {
        return nullptr;
    }

    size_t	linkIndex = fHead.size ()-1;
    Node*	n = fHead[linkIndex];
    if (n != nullptr) {
        Node*   prev = n;
        while (true) {
            while (n != nullptr) {
                prev = n;
                n = n->fNext[linkIndex];
            }
            n = prev;
            if (linkIndex == 0) {
                break;
            }
            --linkIndex;
        }
    }
    return n;
}

template <typename KEY, typename VALUE,typename TRAITS>
void    SkipList<KEY,VALUE,TRAITS>::Prioritize (const KeyType& key)
{
	std::vector<Node*>	links;

	Node* node = FindNearest (key, links);
	if (node != nullptr and node->fNext.size () <= fHead.size ()) {
	    if (node->fNext.size () == fHead.size ()) {
            GrowHeadLinksIfNeeded (fHead.size ()+1, node);
            links.resize (fHead.size (), node);
	    }

        size_t  oldLinkHeight = node->fNext.size ();
        node->fNext.resize (fHead.size (), nullptr);
        size_t  newLinkHeight = node->fNext.size ();
        Assert (oldLinkHeight < newLinkHeight);

        for (size_t i = oldLinkHeight; i <= newLinkHeight-1; ++i) {
            if (links[i] == nullptr) {
                fHead[i] = node;
            }
            else if (links[i] == node) {
                break;
            }
            else {
                #if qKeepADTStatistics
                    const_cast<SkipList<KEY,VALUE,TRAITS> *> (this)->fRotations++;
                #endif
                Node*	oldLink = links[i];
                AssertNotNull (oldLink);
                Assert (oldLink->fNext.size () > i);
                Node* nextL = oldLink->fNext[i];
                oldLink->fNext[i] = node;
                node->fNext[i] = nextL;
            }
        }
	}
}


template <typename KEY, typename VALUE,typename TRAITS>
typename SkipList<KEY,VALUE,TRAITS>::Iterator	SkipList<KEY,VALUE,TRAITS>::Iterate () const
{
    return Iterator (FirstNode ());
}

template <typename KEY, typename VALUE,typename TRAITS>
typename SkipList<KEY,VALUE,TRAITS>::Iterator SkipList<KEY,VALUE,TRAITS>::Iterate (const KeyType& key) const
{
    std::vector<Node*>  links;
    Node* n = FindNearest (key, links);
    return Iterator ((n == nullptr) ? links[0] : n);
}

template <typename KEY, typename VALUE,typename TRAITS>
void    SkipList<KEY,VALUE,TRAITS>::Update (const SkipListIterator<KEY,VALUE,TRAITS>& it, const ValueType& newValue)
{
    Require (not it.Done ());
    const_cast<SkipList<KEY, VALUE, TRAITS>::Node*> (it.fCurrent)->fEntry.SetValue (newValue);
}

template <typename KEY, typename VALUE,typename TRAITS>
void	SkipList<KEY,VALUE,TRAITS>::Remove (const SkipListIterator<KEY,VALUE,TRAITS>& it)
{
    Require (not it.Done ());

    // we need the links to reset, so have to refind
	std::vector<Node*>	links;
	Node*	n = FindNearest (it, links);
    AssertNotNull (n);  // else not it.Done ()
    RemoveNode (n, links);
}

template <typename KEY, typename VALUE,typename TRAITS>
typename SkipList<KEY,VALUE,TRAITS>::Iterator SkipList<KEY,VALUE,TRAITS>::begin () const
{
    return Iterate ();
}

template <typename KEY, typename VALUE,typename TRAITS>
typename SkipList<KEY,VALUE,TRAITS>::Iterator  SkipList<KEY,VALUE,TRAITS>::end () const
{
    return Iterator (nullptr);
}

template <typename KEY, typename VALUE,typename TRAITS>
typename SkipList<KEY,VALUE,TRAITS>::Iterator	SkipList<KEY,VALUE,TRAITS>::GetFirst () const
{
    return Iterate ();
}

template <typename KEY, typename VALUE,typename TRAITS>
typename SkipList<KEY,VALUE,TRAITS>::Iterator	SkipList<KEY,VALUE,TRAITS>::GetLast () const
{
    return Iterator (LastNode ());
}


template <typename KEY, typename VALUE,typename TRAITS>
void	SkipList<KEY,VALUE,TRAITS>::Balance ()
{
    if (GetLength () == 0) return;

	// precompute table of indices height
	// idea is to have a link for every log power of the probability at a particular index
	// for example, for a 1/2 chance, have heights start as 1 2 1 3 1 2 1 4
	double	indexBase = (GetLinkHeightProbability () == 0) ? 0 : 1/(GetLinkHeightProbability ()/100.0);
	size_t	height[kMaxLinkHeight];
	size_t	lastValidHeight = 0;
	for (size_t i = 0; i < sizeof (height)/sizeof (size_t); ++i) {
		height[i] = size_t (pow (indexBase, double (i)));

		if (height[i] == 0 or height[i] > GetLength ()) {
			Assert (i > 0);	// else have no valid heights
			break;
		}
		lastValidHeight = i;
	}

	// wipe out everything, keeping only a link to the first item in list
	Node*	node = fHead[0];
	fHead.clear ();
	fHead.resize (kMaxLinkHeight);

	Node**	patchNodes[kMaxLinkHeight];
	for (size_t i = 0; i < sizeof (patchNodes)/sizeof (size_t); ++i) {
		patchNodes[i] = &fHead[i];
	}

    Node*   prev = nullptr;
	size_t	index = 1;
	while (node != nullptr) {
		Node*	next = node->fNext[0];
		node->fNext.clear ();

        size_t  maxHeight = lastValidHeight;
 	    if ((not (TRAITS::kPolicy & ADT::eDuplicateAddThrowException)) and (prev != nullptr)) {
            // expensive, but for identical nodes we must make sure the first node is highest
            if (TRAITS::Comparer::Compare (prev->fEntry.GetKey (), node->fEntry.GetKey ()) == 0) {
                maxHeight = prev->fNext.size ();
            }
 	    }

#if qDebug
		bool	patched = false;
#endif
		for (size_t hIndex = lastValidHeight+1; hIndex-- > 0;) {
			if (index >= height[hIndex] and (index % height[hIndex] == 0)) {
			    size_t  newHeight = std::min (hIndex+1, maxHeight);
				node->fNext.resize (newHeight, nullptr);
				for (size_t patchIndex = node->fNext.size (); patchIndex-- > 0;) {
					*patchNodes[patchIndex] = node;
					patchNodes[patchIndex] = &node->fNext[patchIndex];
				}
#if qDebug
				patched = true;
#endif
				break;
			}
		}
		Assert (patched);

		++index;
		prev = node;
		node = next;
	}
	Assert (index == GetLength ()+1);

	ShrinkHeadLinksIfNeeded ();
}

template <typename KEY, typename VALUE,typename TRAITS>
void    SkipList<KEY,VALUE,TRAITS>::Invariant () const
{
    #if qDebug
        Invariant_ ();
    #endif
}



#if qDebug
    template <typename KEY, typename VALUE,typename TRAITS>
    void	SkipList<KEY,VALUE,TRAITS>::Invariant_ () const
    {
		Node* n = fHead[0];

		while (n != nullptr) {
			KEY	oldKey = n->fEntry.GetKey ();
			for (size_t i = 1; i < n->fNext.size (); ++i) {
				Node* newN = n->fNext[i];
				if (n == nullptr) {
					Assert (newN == nullptr);
				}
				else {
				    Assert (newN != n);
					Assert (newN == nullptr or (TRAITS::Comparer::Compare (oldKey, newN->fEntry.GetKey ()) <= 0));
				}
			}
			n = n->fNext[0];
			Assert (n == nullptr or (TRAITS::Comparer::Compare (n->fEntry.GetKey (), oldKey) >= 0));
		}
    }

	template <typename KEY, typename VALUE,typename TRAITS>
	void	SkipList<KEY,VALUE,TRAITS>::ListAll () const
	{
        std::cout << "[";
		for (size_t i = 0; i < fHead.size (); ++i) {
			if (fHead[i] == nullptr) {
                std::cout << "*" << ", ";
			}
			else {
                std::cout <<  fHead[i]->fEntry.GetValue () << ", ";
			}
		}
        std::cout << "]  ";

		Node* n = fHead[0];
		while (n != nullptr) {
            std::cout << n->fEntry.GetValue () << " (" << n->fNext.size () << "), ";
			n = n->fNext[0];
		}
        std::cout << std::endl << std::flush;
	}
#endif

#if qKeepADTStatistics
	template <typename KEY, typename VALUE,typename TRAITS>
	size_t	SkipList<KEY,VALUE,TRAITS>::CalcHeight (size_t* totalHeight) const
	{
		if (totalHeight != nullptr) {
			*totalHeight = 0;
			size_t	maxLinkHeight = 0;

			Node*	n = fHead[0];
			while (n != nullptr) {
                maxLinkHeight = std::max (maxLinkHeight, n->fNext.size ());
				*totalHeight += n->fNext.size ();
				n = n->fNext[0];
			}
			Assert (maxLinkHeight == fHead.size ());
		}

		return fHead.size ();
	}

#endif

#if qDebug

template <typename KEYTYPE>
void    SkipListValidationSuite (size_t testDataLength, bool verbose)
{
    TestTitle   tt ("SkipList Validation", 0, verbose);

    RunSuite<SkipList<KEYTYPE, size_t>, KEYTYPE> (testDataLength, verbose, 1);

    typedef SkipList<int, int>  DefaultSkipList;
    typedef SkipList<int, int, ADT::Traits<
        KeyValue<int,int>,
        ADT::DefaultComp<int>,
        ADT::eDuplicateAddThrowException> >  NoDupAdd;
    DuplicateAddBehaviorTest<DefaultSkipList, NoDupAdd> (testDataLength, verbose, 1);

   typedef    SkipList<int, int, ADT::Traits<
        KeyValue<int,int>,
        ADT::DefaultComp<int>,
        ADT::eInvalidRemoveIgnored> > InvalidRemoveIgnored;
    InvalidRemoveBehaviorTest<DefaultSkipList, InvalidRemoveIgnored> (verbose, 1);


    typedef    SkipList<string, string, ADT::Traits<
        SharedStringKeyValue,
        CaseInsensitiveCompare,
        ADT::eDefaultPolicy> > SharedCaseInsensitiveString;
    StringTraitOverrideTest<SharedCaseInsensitiveString> (verbose, 1);

    typedef SkipList<HashKey<string>, string>   HashedString;
    HashedStringTest<HashedString> (verbose, 1);
}

#endif

/*
SkipListNode
*/
template <typename KEY, typename VALUE,typename TRAITS>
SkipList<KEY,VALUE,TRAITS>::Node::Node (const KeyType& key, const ValueType& val)	:
	fEntry (key, val)
{
}


/*
SkipListIterator
*/
template <typename KEY,  typename VALUE, typename TRAITS>
SkipListIterator<KEY, VALUE, TRAITS>::SkipListIterator (const SkipListIterator<KEY, VALUE, TRAITS>& from) :
    fCurrent (from.fCurrent),
    fSuppressMore (from.fSuppressMore)
{
}

template <typename KEY,  typename VALUE, typename TRAITS>
SkipListIterator<KEY, VALUE, TRAITS>::SkipListIterator (SkipListNode* node) :
    fCurrent (node),
    fSuppressMore (true)
{
}


template <typename KEY,  typename VALUE, typename TRAITS>
void    SkipListIterator<KEY, VALUE, TRAITS>::Invariant () const
{
#if     qDebug
    Invariant_ ();
#endif
}

template <typename KEY,  typename VALUE, typename TRAITS>
SkipListIterator<KEY, VALUE, TRAITS>&  SkipListIterator<KEY, VALUE, TRAITS>::operator= (const SkipListIterator<KEY, VALUE, TRAITS>& rhs)
{
    Invariant ();

    fCurrent = rhs.fCurrent;
    fSuppressMore = rhs.fSuppressMore;

    Invariant ();
    return (*this);
}

template <typename KEY,  typename VALUE, typename TRAITS>
bool    SkipListIterator<KEY, VALUE, TRAITS>::Done () const
{
    Invariant ();
    return bool (fCurrent == nullptr);
}

template <typename KEY,  typename VALUE, typename TRAITS>
bool    SkipListIterator<KEY, VALUE, TRAITS>::More (KeyValue* current, bool advance)
{
    Invariant ();

    if (advance and fCurrent != nullptr) {
        if (fSuppressMore) {
            fSuppressMore = false;
        }
        else {
            Assert (fCurrent->fNext.size () > 0);
            fCurrent = fCurrent->fNext[0];
        }
    }
    Invariant ();
    if (current != nullptr and not Done ()) {
        *current = fCurrent->fEntry;
    }
    return (not Done ());
}

template <typename KEY,  typename VALUE, typename TRAITS>
const   typename  SkipListIterator<KEY, VALUE, TRAITS>::KeyValue&   SkipListIterator<KEY, VALUE, TRAITS>::Current () const
{
    Require (not Done ());
    Invariant ();
    AssertNotNull (fCurrent);
    return (fCurrent->fEntry);
}


template <typename KEY,  typename VALUE, typename TRAITS>
const typename SkipListIterator<KEY, VALUE, TRAITS>::KeyValue&  SkipListIterator<KEY, VALUE, TRAITS>::operator* () const
{
     Require (not Done ());
     Invariant ();

     return Current ();
}

template <typename KEY,  typename VALUE, typename TRAITS>
const typename SkipListIterator<KEY, VALUE, TRAITS>::KeyValue* SkipListIterator<KEY, VALUE, TRAITS>::operator-> () const
{
    Require (not Done ());
    Invariant ();

    return &fCurrent->fEntry;
}

template <typename KEY,  typename VALUE, typename TRAITS>
SkipListIterator<KEY, VALUE, TRAITS>&    SkipListIterator<KEY, VALUE, TRAITS>::operator++ ()
{
    Invariant ();

    if (fCurrent != nullptr) {
        Assert (fCurrent->fNext.size () > 0);
        fCurrent = fCurrent->fNext[0];
        fSuppressMore = true;
   }

    return *this;
}


template <typename KEY,  typename VALUE, typename TRAITS>
bool    SkipListIterator<KEY, VALUE, TRAITS>::operator== (const SkipListIterator<KEY, VALUE, TRAITS>& rhs) const
{
     Invariant ();
     return (fCurrent == rhs.fCurrent);
}

template <typename KEY,  typename VALUE, typename TRAITS>
bool    SkipListIterator<KEY, VALUE, TRAITS>::operator!= (const SkipListIterator<KEY, VALUE, TRAITS>&rhs) const
{
    Invariant ();
    return (fCurrent != rhs.fCurrent);
}

#if     qDebug
template <typename KEY,  typename VALUE, typename TRAITS>
void    SkipListIterator<KEY, VALUE, TRAITS>::Invariant_ () const
{
}
#endif



/*
SkipList_Patch
*/

template <typename KEY,  typename VALUE, typename TRAITS>
SkipList_Patch<KEY, VALUE, TRAITS>::SkipList_Patch () :
    fIterators (nullptr)
{
}

template <typename KEY,  typename VALUE, typename TRAITS>
SkipList_Patch<KEY, VALUE, TRAITS>::SkipList_Patch (const SkipList_Patch& s) :
    SkipList<KEY, VALUE, TRAITS> (s),
    fIterators (nullptr)
{
}

template <typename KEY,  typename VALUE, typename TRAITS>
SkipList_Patch<KEY, VALUE, TRAITS>::~SkipList_Patch ()
{
    Require (fIterators == nullptr);
}

template <typename KEY,  typename VALUE, typename TRAITS>
SkipList_Patch<KEY, VALUE, TRAITS>&	SkipList_Patch<KEY, VALUE, TRAITS>::operator= (const SkipList_Patch<KEY, VALUE, TRAITS>& t)
{
    Require (fIterators == nullptr);   // perhaps could support by letting iterators do a find after the assignment, based on their old KEY
    SkipList<KEY, VALUE, TRAITS>::operator= (t);
    return *this;
}

template <typename KEY,  typename VALUE, typename TRAITS>
typename SkipList_Patch<KEY, VALUE, TRAITS>::Iterator SkipList_Patch<KEY, VALUE, TRAITS>::Iterate () const
{
    return Iterator (*const_cast<SkipList_Patch*>(this), this->FirstNode ());
}

template <typename KEY,  typename VALUE, typename TRAITS>
typename SkipList_Patch<KEY, VALUE, TRAITS>::Iterator SkipList_Patch<KEY, VALUE, TRAITS>::Iterate (const KeyType& key) const
{
    std::vector<typename SkipList_Patch<KEY, VALUE, TRAITS>::Node*>  links;
    typename SkipList_Patch<KEY, VALUE, TRAITS>::Node* n = this->FindNearest (key, links);
    return Iterator (*const_cast<SkipList_Patch*>(this), (n == nullptr) ? links[0] : n);
}

template <typename KEY, typename VALUE,typename TRAITS>
typename SkipList_Patch<KEY, VALUE, TRAITS>::Iterator SkipList_Patch<KEY,VALUE,TRAITS>::begin () const
{
    return Iterate ();
}

template <typename KEY, typename VALUE,typename TRAITS>
typename SkipList_Patch<KEY, VALUE, TRAITS>::Iterator SkipList_Patch<KEY,VALUE,TRAITS>::end () const
{
    return Iterator ();
}

template <typename KEY, typename VALUE,typename TRAITS>
typename SkipList_Patch<KEY, VALUE, TRAITS>::Iterator SkipList_Patch<KEY,VALUE,TRAITS>::GetFirst () const
{
    return Iterate ();
}

template <typename KEY, typename VALUE,typename TRAITS>
typename SkipList_Patch<KEY, VALUE, TRAITS>::Iterator SkipList_Patch<KEY,VALUE,TRAITS>::GetLast () const
{
    return Iterate (this->GetLast ());
}


template <typename KEY,  typename VALUE, typename TRAITS>
void	SkipList_Patch<KEY, VALUE, TRAITS>::Remove (const KeyType& key)
{
    typedef typename SkipList<KEY, VALUE, TRAITS>::Node Node;
	std::vector<Node*>	links;
	Node*	n = this->FindNearest (key, links);
	if (n != nullptr) {
        for (auto it = fIterators; it != nullptr; it = it->fNext) {
            it->RemovePatch (n);
        }
    	this->RemoveNode (n, links);
	}
	else {
		if (not (TRAITS::kPolicy & ADT::eInvalidRemoveIgnored)) {
		    throw InvalidRemovalException ();
		}
	}
}

template <typename KEY,  typename VALUE, typename TRAITS>
void	SkipList_Patch<KEY, VALUE, TRAITS>::Remove (const Iterator& remIt)
{
    typedef typename SkipList<KEY, VALUE, TRAITS>::Node Node;
    Require (not remIt.Done ());
    // we need the links to reset, so have to refind
	std::vector<Node*>	links;
	Node*	n = this->FindNearest (remIt, links);
    AssertNotNull (n);  // else not it.Done ()
	if (n != nullptr) {
        for (auto it = fIterators; it != nullptr; it = it->fNext) {
            it->RemovePatch (n);
        }
    	this->RemoveNode (n, links);
	}
	else {
		if (not (TRAITS::kPolicy & ADT::eInvalidRemoveIgnored)) {
		    throw InvalidRemovalException ();
		}
	}
}


template <typename KEY,  typename VALUE, typename TRAITS>
void	SkipList_Patch<KEY, VALUE, TRAITS>::RemoveAll ()
{
    for (auto it = fIterators; it != nullptr; it = it->fNext) {
        it->RemoveAllPatch ();
    }
    inherited::RemoveAll ();
}

template <typename KEY,  typename VALUE, typename TRAITS>
void    SkipList_Patch<KEY, VALUE, TRAITS>::Invariant () const
{
    #if		qDebug
        Invariant_ ();
    #endif
}

#if		qDebug
    template <typename KEY,  typename VALUE, typename TRAITS>
    void	SkipList_Patch<KEY, VALUE, TRAITS>::Invariant_ () const
    {
        inherited::Invariant_ ();
        for (auto it = fIterators; it != nullptr; it = it->fNext) {
            Assert (it->fData == this);
         }
    }
#endif


/*
*/
template <typename KEY,  typename VALUE, typename TRAITS>
SkipListIterator_Patch<KEY, VALUE, TRAITS>::SkipListIterator_Patch () :
    SkipListIterator<KEY, VALUE, TRAITS> (),
    fData (nullptr),
    fNext (nullptr),
    fSuppressAdvance (false)
{
    Assert (this->Done ());
}

template <typename KEY,  typename VALUE, typename TRAITS>
SkipListIterator_Patch<KEY, VALUE, TRAITS>::SkipListIterator_Patch (const SkipListIterator_Patch<KEY, VALUE, TRAITS>& from) :
    SkipListIterator<KEY, VALUE, TRAITS> (from),
    fData (from.fData),
    fNext ((from.fData == nullptr) ? nullptr : from.fData->fIterators),
    fSuppressAdvance (from.fSuppressAdvance)
{
    if (fData != nullptr) {
        fData->fIterators = this;
    }
}

template <typename KEY,  typename VALUE, typename TRAITS>
SkipListIterator_Patch<KEY, VALUE, TRAITS>::SkipListIterator_Patch (SkipList_Patch<KEY, VALUE, TRAITS>& data, SkipListNode* node) :
    SkipListIterator<KEY, VALUE, TRAITS> (node),
    fData (&data),
    fNext (data.fIterators),
    fSuppressAdvance (false)
{
    AssertNotNull (fData);
    fData->fIterators = this;

    Invariant ();
}

template <typename KEY,  typename VALUE, typename TRAITS>
SkipListIterator_Patch<KEY, VALUE, TRAITS>& SkipListIterator_Patch<KEY, VALUE, TRAITS>::operator= (const SkipListIterator_Patch<KEY, VALUE, TRAITS>& from)
{
    RemoveFromChain ();

    inherited::operator= (from);
    fSuppressAdvance = from.fSuppressAdvance;

    fData = from.fData;
    if (fData == nullptr) {
        fNext = nullptr;
    }
    else {
        fNext = fData->fIterators;
        fData->fIterators = this;
    }

    Invariant ();

    return (*this);
}

template <typename KEY,  typename VALUE, typename TRAITS>
SkipListIterator_Patch<KEY, VALUE, TRAITS>::~SkipListIterator_Patch ()
{
    RemoveFromChain ();
}

template <typename KEY,  typename VALUE, typename TRAITS>
void    SkipListIterator_Patch<KEY, VALUE, TRAITS>::RemoveFromChain ()
{
    if (fData != nullptr) {
        if (fData->fIterators == this) {
            fData->fIterators = fNext;
        }
        else {
            for (auto it = fData->fIterators; it != nullptr; it = it->fNext) {
                 if (it->fNext == this) {
                    it->fNext = fNext;
                    break;
                }
            }
        }
    }
}

template <typename KEY,  typename VALUE, typename TRAITS>
bool    SkipListIterator_Patch<KEY, VALUE, TRAITS>::More (KeyValue* current, bool advance)
{
    Invariant ();

    if (advance and (this->fCurrent != nullptr) and fSuppressAdvance) {
        this->fSuppressMore = true;
        inherited::More (current, advance);
        fSuppressAdvance = false;
    }
    else {
        inherited::More (current, advance);
   }

    return (not this->Done ());
}


template <typename KEY,  typename VALUE, typename TRAITS>
SkipListIterator_Patch<KEY, VALUE, TRAITS>&    SkipListIterator_Patch<KEY, VALUE, TRAITS>::operator++()
{
    Invariant ();

    if (this->fCurrent != nullptr) {
        if (fSuppressAdvance) {
            fSuppressAdvance = false;
        }
        else {
            inherited::operator++ ();
        }
    }

    return *this;
}

template <typename KEY,  typename VALUE, typename TRAITS>
void    SkipListIterator_Patch<KEY, VALUE, TRAITS>::RemovePatch (typename SkipList<KEY, VALUE, TRAITS>::Node* n)
{
    RequireNotNull (n);
    if (this->fCurrent == n) {
         this->fCurrent = this->fCurrent->fNext[0];
         this->fSuppressAdvance = true;
    }

    Invariant ();
}

template <typename KEY,  typename VALUE, typename TRAITS>
void    SkipListIterator_Patch<KEY, VALUE, TRAITS>::RemoveAllPatch ()
{
    this->fCurrent = nullptr;

    Invariant ();
}

template <typename KEY,  typename VALUE, typename TRAITS>
void	SkipListIterator_Patch<KEY, VALUE, TRAITS>::Invariant () const
{
    #if		qDebug
        Invariant_ ();
    #endif
}


#if		qDebug
template <typename KEY,  typename VALUE, typename TRAITS>
void	SkipListIterator_Patch<KEY, VALUE, TRAITS>::Invariant_ () const
{
    inherited::Invariant_ ();

    Assert ((fData != nullptr) or (fNext == nullptr));
    if (fData != nullptr) {
        auto it = fData->fIterators;
        for (; (it != nullptr and it != this); it = it->fNext)  ;
        Assert (it == this);
    }
}
#endif



}   // namespace ADT
