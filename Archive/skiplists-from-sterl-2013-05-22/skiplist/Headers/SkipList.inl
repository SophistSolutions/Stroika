#pragma once

#include <math.h>

#if qSkipListDebug
	#include <iostream>
#endif





template <typename KEY, typename VALUE>	
KeyValue_SkipList<KEY, VALUE>::KeyValue_SkipList (KEY k, VALUE v) : 
	fKey (k),
	fValue (v)  
{
}

template <typename KEY, typename VALUE>	
KEY	KeyValue_SkipList<KEY, VALUE>::GetKey ()  const
{
	return fKey;
}


template <>
struct Counter_SkipList<true> {
	Counter_SkipList () :  
		fCount (0) 
	{
	}

	size_t	GetCount () const  
	{ 
		return fCount; 
	}

	size_t	ChangeCount (int inc) 
	{ 
		Require ((inc >= 0) or (size_t (-inc) <= fCount));
		fCount += inc; 
		return fCount; 
	}

	size_t	fCount;
};


template <typename KEY, typename VALUE,typename COMP,bool SHOULDCOUNT>
SkipList<KEY,VALUE,COMP,SHOULDCOUNT>::SkipList (double linkHeightProbability) :
	fLength (0),
#if qSkipListStatistics
	fLinksChecked (0),
	fLinksPointerChecked (0),
#endif
	fLinkHeightProbability (linkHeightProbability)
{
	Require (linkHeightProbability >= 0);
	Require (linkHeightProbability <= 0.5);

	GrowHeadLinksIfNeeded (1, nullptr);
}

template <typename KEY, typename VALUE,typename COMP,bool SHOULDCOUNT>
SkipList<KEY,VALUE,COMP,SHOULDCOUNT>::~SkipList ()
{
	RemoveAll ();
}


template <typename KEY, typename VALUE,typename COMP,bool SHOULDCOUNT>
inline	double	SkipList<KEY,VALUE,COMP,SHOULDCOUNT>::GetLinkHeightProbability ()  const
{
	return fLinkHeightProbability;
}


template <typename KEY, typename VALUE,typename COMP,bool SHOULDCOUNT>
inline	size_t	SkipList<KEY,VALUE,COMP,SHOULDCOUNT>::GetLength () const
{
	return fLength;
}

template <typename KEY, typename VALUE,typename COMP,bool SHOULDCOUNT>
inline	size_t	SkipList<KEY,VALUE,COMP,SHOULDCOUNT>::GetKeyCount () const
{
	if (SHOULDCOUNT) {
		return fKeyCount.GetCount ();
	}
	return fLength;
}


template <typename KEY, typename VALUE,typename COMP,bool SHOULDCOUNT>
typename SkipList<KEY,VALUE,COMP,SHOULDCOUNT>::Node*	SkipList<KEY,VALUE,COMP,SHOULDCOUNT>::FindNode (KEY key)  const
{
	Assert (fHead.size () > 0);

	std::vector<Node*>* startV = const_cast<std::vector<Node*>*> (&fHead);
	size_t	linkHeight = fHead.size ()-1;
	do {
		Node* n = FindLink (key, &startV, linkHeight);
		if (n != nullptr) {
			return n;
		}
	} while (linkHeight-- != 0);

	return nullptr;
}

template <typename KEY, typename VALUE,typename COMP,bool SHOULDCOUNT>
bool	SkipList<KEY,VALUE,COMP,SHOULDCOUNT>::Find (KEY key, VALUE* val)  const
{
	Node* n = FindNode (key);
	if (n != nullptr) {
		if (val != nullptr) {
			*val = n->fEntry.fValue;
		}
		return true;
	}
	return false;
}

template <typename KEY, typename VALUE,typename COMP,bool SHOULDCOUNT>
typename SkipList<KEY,VALUE,COMP,SHOULDCOUNT>::Node*	SkipList<KEY,VALUE,COMP,SHOULDCOUNT>::FindLink (KEY key, std::vector<Node*>** startV, size_t linkIndex)  const
{
	Node*	n = (**startV)[linkIndex];
	Node*	overShotNode = ((*startV)->size () <= (linkIndex+1)) ? nullptr : (**startV)[linkIndex+1];
	Node*	prev = n;
	bool	advanced = false;
	while (n != nullptr) {
		#if qSkipListStatistics
			const_cast<SkipList<KEY,VALUE,COMP,SHOULDCOUNT> *> (this)->fLinksChecked++;	
		#endif

		// tweak to use pointer comparisons rather than key field compares. We know any link heigher than the current link being 
		// tested must point past the key we are looking for, so we can compare our current node with that one and skip the 
		// test if they are the same. In practice, seems to avoid 5-10% of all compares
		if (n == overShotNode) {
			#if qSkipListStatistics
				const_cast<SkipList<KEY,VALUE,COMP,SHOULDCOUNT> *> (this)->fLinksPointerChecked++;	
			#endif
			break;	// overshot
		}

		int comp = COMP::Compare (n->fEntry.GetKey (), key);

		if (comp == 0) {
			return n;
		}
		else if (comp < 0) {
			prev = n;
			n = n->fNext[linkIndex];
			advanced = true;
		}
		else {
			break;	// overshot
		}
	}
	if (advanced) {
		*startV = &prev->fNext;
	}
	return nullptr;	// no links at this height left
}


template <typename KEY, typename VALUE,typename COMP,bool SHOULDCOUNT>
void	SkipList<KEY,VALUE,COMP,SHOULDCOUNT>::Add (KEY key, VALUE val)
{
	std::vector<Node*>	links;
	Node*	n = FindNearest (key, &links);
	if (n != nullptr and SHOULDCOUNT) {
		n->fCounter.ChangeCount (1);
	}
	else {
		size_t	newLinkHeight = DetermineLinkHeight ();
		Node*	node = new Node (key, val);
		node->fNext.resize (newLinkHeight);

		size_t	linksToPatch = min (fHead.size (), newLinkHeight);
		for (size_t i = 0; i < linksToPatch; ++i) {
			Node*	nextL = nullptr;
			if (links[i] == nullptr) {
				nextL = fHead[i];
				fHead[i] = node;
			}
			else {
				Node*	oldLink = links[i];
				AssertNotNull (oldLink);
				nextL = oldLink->fNext[i];
				oldLink->fNext[i] = node;
			}

			node->fNext[i] = nextL;
		}

		if (SHOULDCOUNT) {
			node->fCounter.ChangeCount (1);
		}
		GrowHeadLinksIfNeeded (newLinkHeight, node);
		if (SHOULDCOUNT) {
			fKeyCount.ChangeCount (1);
		}
	}
	++fLength;
}

template <typename KEY, typename VALUE,typename COMP,bool SHOULDCOUNT>
void	SkipList<KEY,VALUE,COMP,SHOULDCOUNT>::Remove (KEY key)
{
	std::vector<Node*>	links;
	Node*	n = FindNearest (key, &links);
	if (n != nullptr) {
		if (SHOULDCOUNT and n->fCounter.GetCount () > 1) {
			n->fCounter.ChangeCount (-1);
		}
		else {
			size_t	index = 0;
			for (auto it = links.begin (); it != links.end (); ++it) {
				size_t index = it - links.begin ();
				Node**	patchNode = (*it == nullptr) ? &fHead[index] : &(*it)->fNext[index];
				if (*patchNode == n) {
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
			if (SHOULDCOUNT) {
				fKeyCount.ChangeCount (-1);
			}
		}
		--fLength;
	}
	else {
		// need to determine policy: exception, assertion, ignore
		AssertNotReached ();
	}
}

template <typename KEY, typename VALUE,typename COMP,bool SHOULDCOUNT>
size_t	SkipList<KEY,VALUE,COMP,SHOULDCOUNT>::DetermineLinkHeight () const
{
	int	linkHeight = 1;
	int	maxHeight = min (fHead.size ()+1, size_t (kMaxLinkHeight));
	while ((linkHeight < maxHeight) and (double (rand ())/RAND_MAX <= GetLinkHeightProbability ())) {
		++linkHeight;
	}
	return linkHeight;
}


template <typename KEY, typename VALUE,typename COMP,bool SHOULDCOUNT>
void	SkipList<KEY,VALUE,COMP,SHOULDCOUNT>::GrowHeadLinksIfNeeded (size_t newSize, Node* nodeToPointTo)
{
	if (newSize > fHead.size ()) {
		fHead.resize (newSize, nodeToPointTo);
		Assert (fHead[newSize-1] == nodeToPointTo);
	}
}

template <typename KEY, typename VALUE,typename COMP,bool SHOULDCOUNT>
void	SkipList<KEY,VALUE,COMP,SHOULDCOUNT>::ShrinkHeadLinksIfNeeded ()
{
	if (fHead.size () <= 1) return;

	for (size_t i = fHead.size ()-1; i > 1; --i) {
		if (fHead[i] == nullptr) {
			fHead.pop_back ();
		}
	}
}

template <typename KEY, typename VALUE,typename COMP,bool SHOULDCOUNT>
void	SkipList<KEY,VALUE,COMP,SHOULDCOUNT>::RemoveAll ()
{
	Node*	link = fHead[0];
	while (link != nullptr) {
		Node*	nextLink = link->fNext[0];
		delete link;
		link = nextLink;
	}
	fHead.resize (1);
	fHead[0] = nullptr;
	fLength = 0;
	if (SHOULDCOUNT) {
		fKeyCount = Counter_SkipList<SHOULDCOUNT> ();	// clear the key count
	}

	Ensure (GetLength () == 0);
	Ensure (GetKeyCount () == 0);
}




template <typename KEY, typename VALUE,typename COMP,bool SHOULDCOUNT>
typename SkipList<KEY,VALUE,COMP,SHOULDCOUNT>::Node*	SkipList<KEY,VALUE,COMP,SHOULDCOUNT>::FindNearest (KEY key, std::vector<Node*>* links) const
{
	RequireNotNull (links);
	Require (links->size () == 0);	// we want to be passed in a totally empty vector

	Assert (fHead.size () > 0);

	links->resize (fHead.size ());
	for (size_t i = 0; i < fHead.size (); ++i) {
		(*links)[i] = fHead[i];
	}

	Node*	foundNode = nullptr;
	size_t	curLinkIndex = links->size ()-1;
	do {
		Node* n = CheckLink (key, links, curLinkIndex);
		if (n != nullptr) {
			foundNode = n;
		}
		if ((curLinkIndex > 0) and ((*links)[curLinkIndex] != nullptr)) {
			(*links)[curLinkIndex-1] = (*links)[curLinkIndex];
		}
	} while (curLinkIndex-- != 0);

	return foundNode;
}

template <typename KEY, typename VALUE,typename COMP,bool SHOULDCOUNT>
typename SkipList<KEY,VALUE,COMP,SHOULDCOUNT>::Node*	SkipList<KEY,VALUE,COMP,SHOULDCOUNT>::CheckLink (KEY key, std::vector<Node*>* links, size_t linkIndex) const
{
	Node*	n = (*links)[linkIndex];
	Node*	prev = nullptr;
	bool	advanced = false;
	while (n != nullptr) {
		#if qSkipListStatistics
			const_cast<SkipList<KEY,VALUE,COMP,SHOULDCOUNT> *> (this)->fLinksChecked++;	
		#endif

		int comp = COMP::Compare (n->fEntry.GetKey (), key);
		if (comp == 0) {
			(*links)[linkIndex] = (advanced) ? prev : nullptr;
			return n;
		}
		else if (comp < 0) {
			prev = n;
			(*links)[linkIndex] = n->fNext[linkIndex];
			n = (*links)[linkIndex];
			advanced = true;
		}
		else {
			break;	// overshot
		}
	}
	(*links)[linkIndex] = (advanced) ? prev : nullptr;
	return nullptr;
}

template <typename KEY, typename VALUE,typename COMP,bool SHOULDCOUNT>
void		SkipList<KEY,VALUE,COMP,SHOULDCOUNT>::OptimizeLinks (double linkHeightProbability)
{
	if (fHead.size () == 0) return;
	if (linkHeightProbability != eLeaveProbabilityUnchanged) {
		Require (linkHeightProbability >= 0);
		Require (linkHeightProbability <= 0.5);
		fLinkHeightProbability = linkHeightProbability;
	}

	// precompute table of indices height
	// idea is to have a link for every log power of the probability at a particular index
	// for example, for a 1/2 chance, have heights start as 1 2 1 3 1 2 1 4
	double	indexBase = (GetLinkHeightProbability () == 0) ? 0 : 1/GetLinkHeightProbability ();
	size_t	height[kMaxLinkHeight];
	size_t	lastValidHeight = 0;
	for (size_t i = 0; i < sizeof (height)/sizeof (size_t); ++i) {
		height[i] = size_t (pow (indexBase, double (i)));
		if (height[i] == 0) {
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

	size_t	index = 1;
	while (node != nullptr) {
		Node*	next = node->fNext[0];
		node->fNext.clear ();

		bool	patched = false;
		for (size_t hIndex = lastValidHeight+1; hIndex-- > 0;) {
			size_t foo = index % height[hIndex];
			if (index >= height[hIndex] and (index % height[hIndex] == 0)) {
				node->fNext.resize (hIndex+1, nullptr);
				for (size_t patchIndex = node->fNext.size (); patchIndex-- > 0;) {
					*patchNodes[patchIndex] = node;
					patchNodes[patchIndex] = &node->fNext[patchIndex];
				}
				patched = true;
				break;
			}
		}
		Assert (patched);

		++index;
		node = next;
	}
	Assert (index == GetLength ()+1);

	ShrinkHeadLinksIfNeeded ();
}

template <typename KEY, typename VALUE,typename COMP,bool SHOULDCOUNT>
SkipList<KEY,VALUE,COMP,SHOULDCOUNT>::Node::Node (KEY key, VALUE val)	:
	fEntry (key, val)
{
}


#if qSkipListDebug
	template <typename KEY, typename VALUE,typename COMP,bool SHOULDCOUNT>
	void	SkipList<KEY,VALUE,COMP,SHOULDCOUNT>::ListAll () const
	{
		cout << "[";
		for (size_t i = 0; i < fHead.size (); ++i) {
			if (fHead[i] == nullptr) {
				cout << "*" << ", ";
			}
			else {
				cout <<  fHead[i]->fEntry.fValue << ", ";
			}
		}
		cout << "]  ";

		Node* n = fHead[0];
		while (n != nullptr) {
			cout << n->fEntry.fValue << " (" << n->fNext.size () << "), ";
			n = n->fNext[0];
		}
		cout << endl << flush;
	}

	template <typename KEY, typename VALUE,typename COMP,bool SHOULDCOUNT>
	void	SkipList<KEY,VALUE,COMP,SHOULDCOUNT>::ValidateAll () const
	{
		Node* n = fHead[0];
		size_t	keyCount = 0;

		while (n != nullptr) {
			++keyCount;
			KEY	oldKey = n->fEntry.GetKey ();
			for (size_t i = 1; i < n->fNext.size (); ++i) {
				Node* newN = n->fNext[i];
				if (n == nullptr) {
					Assert (newN == nullptr);
				}
				else {
					Assert (newN == nullptr or (COMP::Compare (oldKey, newN->fEntry.GetKey ()) <= 0));
				}
			}
			n = n->fNext[0];
			Assert (n == nullptr or (COMP::Compare (n->fEntry.GetKey (), oldKey) >= 0));
		}
		Assert (keyCount == GetKeyCount ());
	}
#endif

#if qSkipListStatistics
	template <typename KEY, typename VALUE,typename COMP,bool SHOULDCOUNT>
	void	SkipList<KEY,VALUE,COMP,SHOULDCOUNT>::ComputeLinkHeightStats (double* linkHeight, size_t* maxLinkHeight)
	{
		RequireNotNull (linkHeight);
		RequireNotNull (maxLinkHeight);

		*linkHeight = 0;
		*maxLinkHeight = 0; 
		Node*	n = fHead[0];
		while (n != nullptr) {
			*maxLinkHeight = max (*maxLinkHeight, n->fNext.size ());
			*linkHeight += n->fNext.size ();
			n = n->fNext[0];
		}
	}
#endif

