#pragma once

#include <algorithm>
#include <stack>

#if qDebug
	#include <iostream>
    #include "../Shared/Headers/ContainerValidation.h"
#endif


namespace   ADT {
    namespace   BinaryTree {


template <typename KEY, typename VALUE, typename TRAITS>
SplayTree<KEY,VALUE,TRAITS>::SplayTree () :
	fHead (nullptr),
	fLength (0)
  #if qKeepADTStatistics
      ,fCompares (0)
      ,fRotations (0)
  #endif
{
}

template <typename KEY, typename VALUE, typename TRAITS>
SplayTree<KEY,VALUE,TRAITS>::SplayTree (const SplayTree& t) :
	fHead (nullptr),
	fLength (t.fLength)
  #if qKeepADTStatistics
      ,fCompares (t.fCompares)
      ,fRotations (t.fRotations)
  #endif
{
	fHead = DuplicateBranch (t.fHead);
}

template <typename KEY, typename VALUE, typename TRAITS>
SplayTree<KEY,VALUE,TRAITS>& SplayTree<KEY,VALUE,TRAITS>::operator= (const SplayTree& t)
{
	RemoveAll ();
	fLength = t.fLength;
	fHead = DuplicateBranch (t.fHead);

	return *this;
}


template <typename KEY, typename VALUE, typename TRAITS>
SplayTree<KEY,VALUE,TRAITS>::~SplayTree ()
{
	RemoveAll ();
}

template <typename KEY, typename VALUE, typename TRAITS>
bool	SplayTree<KEY,VALUE,TRAITS>::Find (const KeyType& key, ValueType* val)
{
	int	comparisonResult;
	size_t	height;
	Node* n = FindNode (key, &comparisonResult, &height);
	if (n != nullptr) {
		if (comparisonResult == 0) {
			if (val != nullptr) {
				*val = n->fEntry.GetValue ();
			}
			Splay (n, height, bool ((height > 8) and height > GetLength ()/10));

			return true;
		}
		else {
			// maybe splay the one we found? Seems to work poorly in practice
			Splay (n, height, bool ((height > 8) and height > GetLength ()/10));
		}
	}
	return false;
}


template <typename KEY, typename VALUE, typename TRAITS>
typename SplayTree<KEY,VALUE,TRAITS>::Node* SplayTree<KEY,VALUE,TRAITS>::Rotate (Node* n, Direction rotateDir)
{
	RequireNotNull (n);

	#if qKeepADTStatistics
		++fRotations;
	#endif

    Direction otherDir = OtherDir (rotateDir);
	Node* newTop = n->GetChild (otherDir);
	RequireNotNull (newTop);

    if (n->GetParent () == nullptr) {
		Assert (n == fHead);
		fHead = newTop;
		fHead->SetParent (nullptr);
	}
	else {
	    n->GetParent ()->SetChild (Node::GetChildDir (n), newTop);
    }

    n->SetChild (otherDir, newTop->GetChild (rotateDir));
    newTop->SetChild (rotateDir, n);
	return newTop;
}


template <typename KEY, typename VALUE, typename TRAITS>
const std::vector<size_t>&	SplayTree<KEY,VALUE,TRAITS>::GetHeightWeights (SplayType st)
{
	switch (st) {
		case eAlwaysSplay:				return sAlwaysSplayDistribution;
		case eUniformDistribution:		return sUniformDistribution;
		case eNormalDistribution:		return sNormalDistribution;
		case eZipfDistribution:			return sZipfDistribution;
		case eCustomSplayType:			return sCustomSplayTypeDistribution;
		default:	AssertNotReached ();
	}
	AssertNotReached (); return sCustomSplayTypeDistribution;
}

template <typename KEY, typename VALUE, typename TRAITS>
void	SplayTree<KEY,VALUE,TRAITS>::SetCustomHeightWeights (const std::vector<size_t>& newHeightWeights)
{
	sCustomSplayTypeDistribution = newHeightWeights;
}

template <typename KEY, typename VALUE, typename TRAITS>
void    SplayTree<KEY,VALUE,TRAITS>::Prioritize (const Iterator& it)
{
    Require (not it.Done ());
    Node*   n = const_cast<Node*> (it.GetNode ());
    RequireNotNull (n);
    Splay (n, 0, true);
}

template <typename KEY, typename VALUE, typename TRAITS>
void SplayTree<KEY,VALUE,TRAITS>::Splay (Node* n, size_t nodeHeight, bool forced)
{
	RequireNotNull (n);

	if (TRAITS::kSplayType == eNeverSplay) {
		return;
	}
	else if (TRAITS::kSplayType == eAlwaysSplay) {
		forced = true;
	}

//	static	std::tr1::uniform_int<size_t> sDist (1, 10000);

	const std::vector<size_t>&	kHeightBonus = GetHeightWeights (TRAITS::kSplayType);
//	size_t	dieRoll = (forced) ? 1 : std::tr1::uniform_int<size_t> (1, 10000) (GetEngine ());
//	size_t	dieRoll = (forced) ? 1 : rand () % 10000;

	/*
		Move upwards in the tree. In classic splay tree, move all the way to the top.
		Splay trees do a slightly more complicated action than simple rotations. Whenever possible it does two rotations
		at once: rotating the parent node and the grandparent node, making the node the new grandparent.
		In cases where the node being played is on the same side of its parent as it's parent is to its grandparent,
		if first rotates the grandparent, then the parent. If on opposite sides, it does the normal rotation sequence (parent, then grandparent);
		It only does a single rotation if the node has no grandparent (its parent is the head)
	 */
	while (n->GetParent () != nullptr) {
		Assert (n->GetParent ()->GetChild (Direction::kLeft) == n or n->GetParent ()->GetChild (Direction::kRight) == n);

		Node*	ancestor = n->GetParent ()->GetParent ();
		if (ancestor == nullptr) {
			if (not forced) {
				Assert (nodeHeight > 0);
				--nodeHeight;
				if (nodeHeight < kHeightBonus.size ()) {
 				    size_t	cutoff = kHeightBonus[nodeHeight];
 				    if ((cutoff == 0) or (size_t (rand () % 10000) > cutoff)) {
                         return;
                    }
				}
			}
			Rotate (n->GetParent (), OtherDir (Node::GetChildDir (n)));
		}
		else {
			if (not forced) {
				Assert (nodeHeight > 1);
				nodeHeight -= 2;
				if (nodeHeight < kHeightBonus.size ()) {
 				    size_t	cutoff = kHeightBonus[nodeHeight];
 				    if ((cutoff == 0) or (size_t (rand () % 10000) > cutoff)) {
                         return;
                    }
				}
			}
			Node*	parent = n->GetParent ();
			if ((parent->GetChild (Direction::kLeft) == n and ancestor->GetChild (Direction::kLeft) == parent) or (parent->GetChild (Direction::kRight) == n and ancestor->GetChild (Direction::kRight) == parent)) {
				// zig-zig
				Rotate (ancestor, OtherDir (Node::GetChildDir (n)));
				Rotate (parent, OtherDir (Node::GetChildDir (n)));
			}
			else {
				// zig-zag
				Rotate (parent, OtherDir (Node::GetChildDir (n)));
				Assert (ancestor->GetChild (Direction::kLeft) == n or ancestor->GetChild (Direction::kRight) == n);
				Rotate (ancestor, OtherDir (Node::GetChildDir (n)));
			}
		}

	}
	Ensure ((n->GetParent () == nullptr) == (fHead == n));
	Ensure ((n->GetParent () == nullptr) or (n->GetParent ()->GetChild (Direction::kLeft) == n) or (n->GetParent ()->GetChild (Direction::kRight) == n));
}

template <typename KEY, typename VALUE, typename TRAITS>
size_t SplayTree<KEY,VALUE,TRAITS>::ForceToBottom (Node* n)
{
	RequireNotNull (n);
	size_t rotations = 0;
	while (n->GetChild (Direction::kLeft) != nullptr or n->GetChild (Direction::kRight) != nullptr) {
	   Direction rotDir = (n->GetChild (Direction::kLeft) == nullptr) or (n->GetChild (Direction::kRight) != nullptr and FlipCoin ()) ? Direction::kLeft : Direction::kRight;
		Rotate (n, rotDir);
		++rotations;
    }


	Ensure (n->GetChild (Direction::kLeft) == nullptr and n->GetChild (Direction::kRight) == nullptr);
	Ensure (fHead->GetParent () == nullptr);

	return rotations;
}

template <typename KEY, typename VALUE, typename TRAITS>
void	SplayTree<KEY,VALUE,TRAITS>::Add (const KeyType& key, const ValueType& val)
{
 	Node* n = new Node (key, val);
 	try {
        AddNode (n);
	}
	catch (const DuplicateAddException& exp) {
	    delete n;
	    throw;
    }
}

template <typename KEY, typename VALUE, typename TRAITS>
void	SplayTree<KEY,VALUE,TRAITS>::Add (const KeyType& keyAndValue)
{
    Add (keyAndValue, keyAndValue);
}


template <typename KEY, typename VALUE, typename TRAITS>
void	SplayTree<KEY,VALUE,TRAITS>::AddNode (Node* n)
{
	RequireNotNull (n);

	int	comp;
	size_t height;
	Node* nearest =  FindNode (n->fEntry.GetKey (), &comp, &height);
	if (nearest == nullptr) {
		Assert (fHead == nullptr);
		fHead = n;
	}
	else {
		if (comp == 0) {
            if (TRAITS::kPolicy & ADT::eDuplicateAddThrowException) {
                throw DuplicateAddException ();
            }
			if (FlipCoin ()) {
			    n->SetChild (Direction::kLeft, nearest->GetChild (Direction::kLeft));
				nearest->SetChild (Direction::kLeft, n);
			}
			else {
			    n->SetChild (Direction::kRight, nearest->GetChild (Direction::kRight));
				nearest->SetChild (Direction::kRight, n);
			}
			height++;
		}
		else if (comp < 0) {
			Assert (nearest->GetChild (Direction::kLeft) == nullptr);
			nearest->SetChild (Direction::kLeft, n);
		}
		else {
			Assert (nearest->GetChild (Direction::kRight) == nullptr);
			nearest->SetChild (Direction::kRight, n);
		}

		Splay (n, height, bool ((height > 8) and height > GetLength ()/10));
	}

	fLength++;
}



template <typename KEY, typename VALUE, typename TRAITS>
void	SplayTree<KEY,VALUE,TRAITS>::Remove (const KeyType& key)
{
	int	comp;
	Node* n =  FindNode (key, &comp, nullptr);

	if ((n == nullptr) or (comp != 0)) {
		if (not (TRAITS::kPolicy & ADT::eInvalidRemoveIgnored)) {
		    throw InvalidRemovalException ();
		}
	}
	else {
		Assert (n->fEntry.GetKey () == key);
		RemoveNode (n);
	}
}

template <typename KEY, typename VALUE, typename TRAITS>
void	SplayTree<KEY,VALUE,TRAITS>::SwapNodes (Node* parent, Node* child)
{
	RequireNotNull (parent);

	Node* ancestor = parent->GetParent ();
	if (ancestor == nullptr) {
		Assert (fHead == parent);
		fHead = child;
		if (fHead != nullptr) {
            fHead->SetParent (nullptr);
		}
	}
	else {
	    ancestor->SetChild (Node::GetChildDir (parent), child);
	}
}

template <typename KEY, typename VALUE, typename TRAITS>
void	SplayTree<KEY,VALUE,TRAITS>::RemoveNode (Node* n)
{
	RequireNotNull (n);

	Splay (n, 0, true);
	Assert (n == fHead);

	if (n->GetChild (Direction::kLeft) == nullptr and n->GetChild (Direction::kRight) == nullptr) {
	    SwapNodes (n, nullptr);
	}
	else if (n->GetChild (Direction::kLeft) == nullptr) {
		SwapNodes (n, n->GetChild (Direction::kRight));
	}
	else if (n->GetChild (Direction::kRight) == nullptr) {
		SwapNodes (n, n->GetChild (Direction::kLeft));
	}
	else {
	    Node* minNode = Node::GetFirst (n->GetChild (Direction::kRight));

		AssertNotNull (minNode);
		if (minNode->GetParent () != n) {
			SwapNodes (minNode, minNode->GetChild (Direction::kRight));
			minNode->SetChild (Direction::kRight, n->GetChild (Direction::kRight));
		}
		SwapNodes (n, minNode);
		minNode->SetChild (Direction::kLeft, n->GetChild (Direction::kLeft));
	}

	delete n;
	--fLength;
}

template <typename KEY, typename VALUE, typename TRAITS>
void	SplayTree<KEY,VALUE,TRAITS>::RemoveAll ()
{
	// iterate rather than natural tail recursive version because splay trees get deep
	std::stack<Node*> nodes;
	if (fHead != nullptr) {
		nodes.push(fHead);
		while (not nodes.empty ()) {
			Node* curNode = nodes.top (); nodes.pop ();

			AssertNotNull (curNode);
			if (curNode->GetChild (Direction::kLeft) != nullptr) {
				nodes.push (curNode->GetChild (Direction::kLeft));
			}
			if (curNode->GetChild (Direction::kRight) != nullptr) {
				nodes.push (curNode->GetChild (Direction::kRight));
			}
			delete curNode;
		}

		fHead = nullptr;
	}
	fLength = 0;
}


template <typename KEY, typename VALUE, typename TRAITS>
typename SplayTree<KEY,VALUE,TRAITS>::Iterator SplayTree<KEY,VALUE,TRAITS>::Iterate (TraversalType t, Direction d) const
{
    return Iterator (Node::GetFirstTraversal (fHead, t, d, nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename SplayTree<KEY,VALUE,TRAITS>::Iterator SplayTree<KEY,VALUE,TRAITS>::Iterate (const KeyType& key, TraversalType t, Direction d) const
{
    int	comp;
    return Iterator (Node::GetFirstTraversal (FindNode (key, &comp), t, d, nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename SplayTree<KEY,VALUE,TRAITS>::Iterator  SplayTree<KEY,VALUE,TRAITS>::begin (TraversalType t, Direction d) const
{
    return Iterator (Node::GetFirstTraversal (fHead, t, d, nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename    SplayTree<KEY,VALUE,TRAITS>::Iterator  SplayTree<KEY,VALUE,TRAITS>::end () const
{
    return Iterator (nullptr);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename    SplayTree<KEY,VALUE,TRAITS>::Iterator	SplayTree<KEY,VALUE,TRAITS>::GetFirst (TraversalType t, Direction d) const
{
    return Iterator (Node::GetFirstTraversal (fHead, t, d, nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename    SplayTree<KEY,VALUE,TRAITS>::Iterator	SplayTree<KEY,VALUE,TRAITS>::GetLast (TraversalType t, Direction d) const
{
    return Iterator (Node::GetFirstTraversal (fHead, t, OtherDir (d), nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
void    SplayTree<KEY,VALUE,TRAITS>::Update (const Iterator& it, const ValueType& newValue)
{
    Require (not it.Done ());
    const_cast<SplayTree<KEY, VALUE, TRAITS>::Node*> (it.GetNode ())->fEntry.SetValue (newValue);
}

template <typename KEY, typename VALUE, typename TRAITS>
void	SplayTree<KEY,VALUE,TRAITS>::Remove (const Iterator& it)
{
    Require (not it.Done ());
    RemoveNode (const_cast<SplayTree<KEY,VALUE,TRAITS>::Node*> (it.GetNode ()));
}

template <typename KEY, typename VALUE, typename TRAITS>
size_t	SplayTree<KEY,VALUE,TRAITS>::GetLength () const
{
	Assert ((fLength == 0) == (fHead == nullptr));
	return fLength;
}


template <typename KEY, typename VALUE, typename TRAITS>
typename SplayTree<KEY,VALUE,TRAITS>::Node*	SplayTree<KEY,VALUE,TRAITS>::FindNode (const KeyType& key, int* comparisonResult, size_t* height)  const
{
	RequireNotNull (comparisonResult);

	Node*	n = fHead;
	Node*	nearest = n;
	if (height != nullptr) {
		*height = 0;
	}
	*comparisonResult = 0;
	while (n != nullptr) {
		#if qKeepADTStatistics
			const_cast<SplayTree<KEY,VALUE,TRAITS> *> (this)->fCompares++;
		#endif
		if (height != nullptr) {
			*height += 1;
		}
		nearest = n;
		*comparisonResult = TRAITS::Comparer::Compare (key, n->fEntry.GetKey ());
		if (*comparisonResult == 0) {
			return n;
		}
		n = (*comparisonResult < 0) ? n->GetChild (Direction::kLeft) : n->GetChild (Direction::kRight);
	}
	return nearest;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename	SplayTree<KEY,VALUE,TRAITS>::Node*	SplayTree<KEY,VALUE,TRAITS>::GetFirst (size_t* height) const
{
	Node* n = fHead;
	if (height != nullptr) {
		*height = 0;
	}
	while (n != nullptr and n->GetChild (Direction::kLeft) != nullptr) {
		if (height != nullptr) {
			*height += 1;
		}
		n = n->GetChild (Direction::kLeft);
	}
	return n;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename	SplayTree<KEY,VALUE,TRAITS>::Node*	SplayTree<KEY,VALUE,TRAITS>::GetLast (size_t* height) const
{
	Node* n = fHead;
	if (height != nullptr) {
		*height = 0;
	}
	while (n != nullptr and n->GetChild (Direction::kRight) != nullptr) {
		if (height != nullptr) {
			*height += 1;
		}
		n = n->GetChild (Direction::kRight);
	}
	return n;
}

template <typename KEY, typename VALUE, typename TRAITS>
SplayType	SplayTree<KEY,VALUE,TRAITS>::GetSplayType ()
{
	return TRAITS::kSplayType;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename SplayTree<KEY,VALUE,TRAITS>::Node*	SplayTree<KEY,VALUE,TRAITS>::DuplicateBranch (Node* n)
{
	// sadly, SplayTrees get very deep, so they hate recursion

	std::stack<Node*> nodes;
	std::stack<Node*> parents;
	std::stack<bool> childIsLeft;

	Node*	newTop = nullptr;

	nodes.push (n);
	parents.push (nullptr);
	childIsLeft.push (true);
	while (not nodes.empty ()) {
		Node* branchTop = nodes.top  ();	nodes.pop ();
		Node* newParent = parents.top  ();	parents.pop ();
		bool isLeft = childIsLeft.top  ();	childIsLeft.pop ();
		if (branchTop != nullptr) {
			Node*	newNode = new Node (*branchTop);

			if (newParent == nullptr) {
				Assert (newTop == nullptr);
				newTop = newNode;
			}
			else {
				if (isLeft) {
					newParent->SetChild (Direction::kLeft, newNode);
				}
				else {
					newParent->SetChild (Direction::kRight, newNode);
				}
			}
			if (branchTop->GetChild (Direction::kLeft) != nullptr) {
				nodes.push (branchTop->GetChild (Direction::kLeft));
				parents.push (newNode);
				childIsLeft.push (true);
			}
			if (branchTop->GetChild (Direction::kRight) != nullptr) {
				nodes.push (branchTop->GetChild (Direction::kRight));
				parents.push (newNode);
				childIsLeft.push (false);
			}
		}
	}
	return newTop;
}


template <typename KEY, typename VALUE, typename TRAITS>
void	SplayTree<KEY,VALUE,TRAITS>::Balance ()
{
    fHead = Node::RebalanceBranch (fHead, fLength);
}

template <typename KEY, typename VALUE, typename TRAITS>
void	SplayTree<KEY,VALUE,TRAITS>::Invariant () const
{
    #if qDebug
        Invariant_ ();
    #endif
}

#if qDebug

template <typename KEY, typename VALUE, typename TRAITS>
void	SplayTree<KEY,VALUE,TRAITS>::Invariant_ () const
{
	size_t	count = 0;

	if (fHead != nullptr) {
		ValidateBranch (fHead, count);
	}
	Assert (count == fLength);
}

template <typename KEY, typename VALUE, typename TRAITS>
void	SplayTree<KEY,VALUE,TRAITS>::ValidateBranch (Node* n, size_t& count)
{
	RequireNotNull (n);
	++count;
	if (n->GetChild (Direction::kLeft) != nullptr) {
		Assert (TRAITS::Comparer::Compare (n->fEntry.GetKey (), n->GetChild (Direction::kLeft)->fEntry.GetKey ()) >= 0);
		Assert (n->GetChild (Direction::kLeft)->GetParent () == n);
		ValidateBranch (n->GetChild (Direction::kLeft), count);
	}
	if (n->GetChild (Direction::kRight) != nullptr) {
		// we cannot do strict < 0, because rotations can put on either side
		Assert (TRAITS::Comparer::Compare (n->fEntry.GetKey (), n->GetChild (Direction::kRight)->fEntry.GetKey ()) <= 0);
		Assert (n->GetChild (Direction::kRight)->GetParent () == n);
		ValidateBranch (n->GetChild (Direction::kRight), count);
	}
}


template <typename KEY, typename VALUE, typename TRAITS>
void	SplayTree<KEY,VALUE,TRAITS>::ListAll (Iterator it) const
{
    std::cout << "[";
    for (; it.More (); ) {
        it.GetNode ()->Inspect ();
    }
    std::cout << "]" << std::endl;
}


#endif

#if qKeepADTStatistics
template <typename KEY, typename VALUE, typename TRAITS>
size_t	SplayTree<KEY,VALUE,TRAITS>::CalcHeight (size_t* totalHeight) const
{
	size_t maxHeight = 0;

	std::stack<Node*> nodes;
	std::stack<size_t> heights;
	nodes.push (fHead);
	heights.push (0);
	while (not nodes.empty ()) {
		Node* curNode = nodes.top  ();
		nodes.pop ();
		size_t height = heights.top  ();
		heights.pop ();
		if (curNode == nullptr) {
			if (totalHeight != nullptr) {
				*totalHeight += height;
			}
			maxHeight = std::max (maxHeight, height);
		}
		else {
			heights.push (height+1);
			nodes.push (curNode->GetChild (Direction::kLeft));
			heights.push (height+1);
			nodes.push (curNode->GetChild (Direction::kRight));
		}
	}
	return maxHeight;
}
#endif

const size_t kAlwaysWeights[] = {10000};
template <typename KEY, typename VALUE, typename TRAITS>
std::vector<size_t>	SplayTree<KEY,VALUE,TRAITS>::sAlwaysSplayDistribution (kAlwaysWeights, kAlwaysWeights + sizeof(kAlwaysWeights) / sizeof(kAlwaysWeights[0]));


const size_t kUniformWeights[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  };
template <typename KEY, typename VALUE, typename TRAITS>
std::vector<size_t>	SplayTree<KEY,VALUE,TRAITS>::sUniformDistribution (kUniformWeights, kUniformWeights + sizeof(kUniformWeights) / sizeof(kUniformWeights[0]));

//const size_t kNormalWeights[] ={0, 0, 100, 100, 250, 250, 250, 250, 250, 250, 250};	//30.1401/30.0162/21.5254
const size_t kNormalWeights[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 45, 47, 685, 685};  //27.50
template <typename KEY, typename VALUE, typename TRAITS>
std::vector<size_t>	SplayTree<KEY,VALUE,TRAITS>::sNormalDistribution (kNormalWeights, kNormalWeights + sizeof(kNormalWeights) / sizeof(kNormalWeights[0]));

//const size_t kZifpWeights[] = {0, 5, 15, 30, 30, 60, 60, 125, 125, 250, 250};
const size_t kZifpWeights[] = {0, 0, 0, 0, 0, 1, 1, 1, 1, 4, 7, 11, 24, 108, 202, 561, 1711  };
template <typename KEY, typename VALUE, typename TRAITS>
std::vector<size_t>	SplayTree<KEY,VALUE,TRAITS>::sZipfDistribution (kZifpWeights, kZifpWeights + sizeof(kZifpWeights) / sizeof(kZifpWeights[0]));

template <typename KEY, typename VALUE, typename TRAITS>
std::vector<size_t>	SplayTree<KEY,VALUE,TRAITS>::sCustomSplayTypeDistribution (SplayTree<KEY,VALUE,TRAITS>::sUniformDistribution);


template <typename KEY, typename VALUE, typename TRAITS>
typename SplayTree<KEY,VALUE,TRAITS>::Engine&	SplayTree<KEY,VALUE,TRAITS>::GetEngine ()
{
    static	std::mt19937	sEngine;
	static	bool	sFirstTime = true;
	if (sFirstTime) {
		sFirstTime = false;
        sEngine.seed (static_cast<unsigned int> (time (NULL)));
	}
	return sEngine;
}



#if qDebug


template <typename KEYTYPE>
void    SplayTreeValidationSuite (size_t testDataLength, bool verbose)
{
    TestTitle   tt ("SplayTree Validation", 0, verbose);

    RunSuite<SplayTree<KEYTYPE, size_t>, KEYTYPE> (testDataLength, verbose, 1);

    typedef SplayTree<int, int, SplayTraits<
        KeyValue<int,int>,
        ADT::DefaultComp<int>,
        ADT::eDuplicateAddThrowException,
        SplayType::eDefaultSplayType> >  NoDupAddTree;

    DuplicateAddBehaviorTest<SplayTree<int, int>, NoDupAddTree> (testDataLength, verbose, 1);

   typedef    SplayTree<int, int, SplayTraits<
        KeyValue<int,int>,
        ADT::DefaultComp<int>,
        ADT::eInvalidRemoveIgnored,
        SplayType::eDefaultSplayType> > InvalidRemoveIgnoredTree;
    InvalidRemoveBehaviorTest<SplayTree<int, int>, InvalidRemoveIgnoredTree> (verbose, 1);

    typedef    SplayTree<string, string, SplayTraits<
        SharedStringKeyValue,
        CaseInsensitiveCompare,
        ADT::eDefaultPolicy,
        SplayType::eDefaultSplayType> > SharedCaseInsensitiveString;
    StringTraitOverrideTest<SharedCaseInsensitiveString> (verbose, 1);

    typedef SplayTree<HashKey<string>, string>   HashedString;
    HashedStringTest<HashedString> (verbose, 1);

    SimpleIteratorTest< SplayTree<size_t, size_t> > (testDataLength, verbose, 1);
    PatchingIteratorTest<Tree_Patching<SplayTree<size_t, size_t> > > (testDataLength, verbose, 1);
}

#endif

    }   // namespace BinaryTree
}   // namespace ADT

