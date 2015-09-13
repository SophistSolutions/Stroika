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
const   double  ScapeGoatTree<KEY,VALUE,TRAITS>::kBalanceFactor = log (100.0/TRAITS::kAlpha);

template <typename KEY, typename VALUE, typename TRAITS>
 ScapeGoatTree<KEY,VALUE,TRAITS>::ScapeGoatTree () :
	fHead (nullptr),
	fLength (0),
	fLengthBounds (0)
  #if qKeepADTStatistics
      ,fCompares (0)
      ,fRotations (0)
  #endif
{
    Require (TRAITS::kAlpha >= 50);
    Require (TRAITS::kAlpha <= 100);
}

template <typename KEY, typename VALUE, typename TRAITS>
ScapeGoatTree<KEY,VALUE,TRAITS>::ScapeGoatTree (const ScapeGoatTree& t) :
	fHead (nullptr),
	fLength (t.fLength),
	fLengthBounds (fLength)
  #if qKeepADTStatistics
      ,fCompares (0)
      ,fRotations (0)
  #endif
{
	fHead = DuplicateBranch (t.fHead);
}

template <typename KEY, typename VALUE, typename TRAITS>
ScapeGoatTree<KEY,VALUE,TRAITS>& ScapeGoatTree<KEY,VALUE,TRAITS>::operator= (const ScapeGoatTree& t)
{
	RemoveAll ();
	fLength = t.fLength;
	fLengthBounds = fLength;
	fHead = DuplicateBranch (t.fHead);

	return *this;
}


template <typename KEY, typename VALUE, typename TRAITS>
ScapeGoatTree<KEY,VALUE,TRAITS>::~ScapeGoatTree ()
{
	RemoveAll ();
}

template <typename KEY, typename VALUE, typename TRAITS>
double  ScapeGoatTree<KEY,VALUE,TRAITS>::GetAlpha () const
{
    return (double (TRAITS::kAlpha) / 100.0);
}

template <typename KEY, typename VALUE, typename TRAITS>
bool	ScapeGoatTree<KEY,VALUE,TRAITS>::Find (const KeyType& key, ValueType* val) const
{
	int	comparisonResult;
	Node* n = FindNode (key, &comparisonResult, nullptr);
	if (n != nullptr) {
		if (comparisonResult == 0) {
			if (val != nullptr) {
				*val = n->fEntry.GetValue ();
			}
			return true;
		}
	}
	return false;
}


template <typename KEY, typename VALUE, typename TRAITS>
typename ScapeGoatTree<KEY,VALUE,TRAITS>::Node* ScapeGoatTree<KEY,VALUE,TRAITS>::Rotate (Node* n, Direction rotateDir)
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
void	ScapeGoatTree<KEY,VALUE,TRAITS>::Add (const KeyType& key, const ValueType& val)
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
void	ScapeGoatTree<KEY,VALUE,TRAITS>::Add (const KeyType& keyAndValue)
{
    Add (keyAndValue, keyAndValue);
}


template <typename KEY, typename VALUE, typename TRAITS>
void	ScapeGoatTree<KEY,VALUE,TRAITS>::AddNode (Node* n)
{
	RequireNotNull (n);

    size_t  newNodeSize = 1;

    int height;
	int	comp;
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
			newNodeSize = GetNodeSize (n);
		}
		else if (comp < 0) {
			Assert (nearest->GetChild (Direction::kLeft) == nullptr);
			nearest->SetChild (Direction::kLeft, n);
		}
		else {
			Assert (nearest->GetChild (Direction::kRight) == nullptr);
			nearest->SetChild (Direction::kRight, n);
		}
	}

	fLength++;
	fLengthBounds++;

	int  allowedMaxHeight = log (double (fLength))/kBalanceFactor + 1;
	if (height > allowedMaxHeight) {
	    size_t  sgSize;
	    Node* scapegoat = FindScapegoat (n, newNodeSize, &sgSize);
	    if (scapegoat != nullptr) {
	        Node*   sgParent = scapegoat->GetParent ();
	        Direction   childDir = Node::GetChildDir (scapegoat);
	        Node*   newTop = Node::RebalanceBranch (scapegoat, sgSize);
#if qKeepADTStatistics
    fRotations += sgSize;  // kinda
#endif
	        if (sgParent == nullptr) {
	            fHead = newTop;
	            newTop->SetParent (nullptr);
	        }
	        else {
	            sgParent->SetChild (childDir, newTop);
	        }
	    }
	}
}

template <typename KEY, typename VALUE, typename TRAITS>
typename ScapeGoatTree<KEY,VALUE,TRAITS>::Node*   ScapeGoatTree<KEY,VALUE,TRAITS>::FindScapegoat (Node* n, size_t nSize, size_t* newTopSize) const
{
    RequireNotNull (n);
    RequireNotNull (newTopSize);

    while (n != nullptr) {
        Node* parent = n->GetParent ();
        if (parent != nullptr) {
            size_t  sibSize = GetNodeSize (parent->GetChild (OtherDir (Node::GetChildDir (n))));
#if qKeepADTStatistics
    fRotations += sibSize;  // kinda
#endif

            size_t  pSize = 1 + nSize + sibSize;
            size_t  cutoff = (pSize * TRAITS::kAlpha) / 100;
            bool    bad = ((nSize > cutoff) or (sibSize > cutoff));
           if (bad) {
  //std::cout << "n Size = " << nSize << "; sib size = " << sibSize << "; p size = " << pSize << "; bad = " << bad << std::endl;
              *newTopSize = pSize;
               return parent;
            }
            nSize = pSize;
            n = parent;
        }
        else {
            break;
        }
    }
//std::cout << "FAILED TO FIND SCAPEGOAT" << std:: endl;
    return nullptr;
}

template <typename KEY, typename VALUE, typename TRAITS>
void	ScapeGoatTree<KEY,VALUE,TRAITS>::Remove (const KeyType& key)
{
	int	comp;
	int height = 0;
	Node* n =  FindNode (key, &comp, &height);

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
void	ScapeGoatTree<KEY,VALUE,TRAITS>::SwapNodes (Node* parent, Node* child)
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
void	ScapeGoatTree<KEY,VALUE,TRAITS>::RemoveNode (Node* n)
{
	RequireNotNull (n);

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
       Direction d = (FlipCoin ()) ? Direction::kRight : Direction::kLeft;

		Node* minNode = (d == Direction::kRight)
            ? Node::GetFirst (n->GetChild (d))
            : Node::GetLast (n->GetChild (d));

		AssertNotNull (minNode);
		if (minNode->GetParent () != n) {
			SwapNodes (minNode, minNode->GetChild (d));
			minNode->SetChild (d, n->GetChild (d));
		}
		SwapNodes (n, minNode);
		minNode->SetChild (OtherDir (d), n->GetChild (OtherDir (d)));
	}

	delete n;
	--fLength;

	if (fLength < fLengthBounds/2) {
	    Balance ();
#if qKeepADTStatistics
    fRotations += fLength;  // kinda
#endif
	}
}

template <typename KEY, typename VALUE, typename TRAITS>
void	ScapeGoatTree<KEY,VALUE,TRAITS>::RemoveAll ()
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
	fLengthBounds = 0;
}

template <typename KEY, typename VALUE, typename TRAITS>
size_t	ScapeGoatTree<KEY,VALUE,TRAITS>::GetLength () const
{
	Assert ((fLength == 0) == (fHead == nullptr));
	return fLength;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename ScapeGoatTree<KEY,VALUE,TRAITS>::Node*	ScapeGoatTree<KEY,VALUE,TRAITS>::FindNode (const KeyType& key, int* comparisonResult, int* height)  const
{
	RequireNotNull (comparisonResult);

	Node*	n = fHead;
	Node*	nearest = n;
	*comparisonResult = 0;
    if (height != nullptr) {
        *height = 0;
    }
	while (n != nullptr) {
		#if qKeepADTStatistics
			const_cast<ScapeGoatTree<KEY,VALUE,TRAITS> *> (this)->fCompares++;
		#endif
		if (height != nullptr) {
		    (*height)++;
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
typename	ScapeGoatTree<KEY,VALUE,TRAITS>::Node*	ScapeGoatTree<KEY,VALUE,TRAITS>::GetFirst () const
{
    return Node::GetFirst (fHead);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename	ScapeGoatTree<KEY,VALUE,TRAITS>::Node*	ScapeGoatTree<KEY,VALUE,TRAITS>::GetLast () const
{
    return Node::GetLast (fHead);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename ScapeGoatTree<KEY,VALUE,TRAITS>::Iterator ScapeGoatTree<KEY,VALUE,TRAITS>::Iterate (TraversalType t, Direction d) const
{
    return Iterator (Node::GetFirstTraversal (fHead, t, d, nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename ScapeGoatTree<KEY,VALUE,TRAITS>::Iterator ScapeGoatTree<KEY,VALUE,TRAITS>::Iterate (const KeyType& key, TraversalType t, Direction d) const
{
    int	comp;
    return Iterator (Node::GetFirstTraversal (FindNode (key, &comp), t, d, nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename ScapeGoatTree<KEY,VALUE,TRAITS>::Iterator  ScapeGoatTree<KEY,VALUE,TRAITS>::begin (TraversalType t, Direction d) const
{
    return Iterator (Node::GetFirstTraversal (fHead, t, d, nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename    ScapeGoatTree<KEY,VALUE,TRAITS>::Iterator  ScapeGoatTree<KEY,VALUE,TRAITS>::end () const
{
    return Iterator (nullptr);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename    ScapeGoatTree<KEY,VALUE,TRAITS>::Iterator	ScapeGoatTree<KEY,VALUE,TRAITS>::GetFirst (TraversalType t, Direction d) const
{
    return Iterator (Node::GetFirstTraversal (fHead, t, d, nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename    ScapeGoatTree<KEY,VALUE,TRAITS>::Iterator	ScapeGoatTree<KEY,VALUE,TRAITS>::GetLast (TraversalType t, Direction d) const
{
    return Iterator (Node::GetFirstTraversal (fHead, t, OtherDir (d), nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
void    ScapeGoatTree<KEY,VALUE,TRAITS>::Update (const Iterator& it, const ValueType& newValue)
{
    Require (not it.Done ());
    const_cast<ScapeGoatTree<KEY, VALUE, TRAITS>::Node*> (it.GetNode ())->fEntry.SetValue (newValue);
}

template <typename KEY, typename VALUE, typename TRAITS>
void	ScapeGoatTree<KEY,VALUE,TRAITS>::Remove (const Iterator& it)
{
    Require (not it.Done ());
    RemoveNode (const_cast<ScapeGoatTree<KEY,VALUE,TRAITS>::Node*> (it.GetNode ()));
}


template <typename KEY, typename VALUE, typename TRAITS>
typename ScapeGoatTree<KEY,VALUE,TRAITS>::Node*	ScapeGoatTree<KEY,VALUE,TRAITS>::DuplicateBranch (Node* n)
{
	// sadly, ScapeGoatTrees get very deep, so they hate recursion

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
void	ScapeGoatTree<KEY,VALUE,TRAITS>::Balance ()
{
    fHead = Node::RebalanceBranch (fHead, GetLength ());
    fLengthBounds = fLength;
}


template <typename KEY, typename VALUE, typename TRAITS>
size_t ScapeGoatTree<KEY,VALUE,TRAITS>::GetNodeSize (Node* n)
{
    if (n == nullptr) {
        return 0;
    }
    return (1 + GetNodeSize (n->GetChild (Direction::kLeft)) + GetNodeSize (n->GetChild (Direction::kRight)));
}

template <typename KEY, typename VALUE, typename TRAITS>
void	ScapeGoatTree<KEY,VALUE,TRAITS>::Invariant () const
{
    #if qDebug
        Invariant_ ();
    #endif
}


#if qDebug

template <typename KEY, typename VALUE, typename TRAITS>
void	ScapeGoatTree<KEY,VALUE,TRAITS>::ValidateBranch (Node* n, size_t& count)
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
void	ScapeGoatTree<KEY,VALUE,TRAITS>::Invariant_ () const
{
	size_t	count = 0;

	if (fHead != nullptr) {
		ValidateBranch (fHead, count);
	}
	Assert (count == fLength);
}


template <typename KEY, typename VALUE, typename TRAITS>
void	ScapeGoatTree<KEY,VALUE,TRAITS>::ListAll (Iterator it) const
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
size_t	ScapeGoatTree<KEY,VALUE,TRAITS>::CalcHeight (size_t* totalHeight) const
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


#if qDebug

template <typename KEYTYPE>
void    ScapeGoatTreeValidationSuite (size_t testDataLength, bool verbose)
{
    TestTitle   tt ("ScapeGoatTree Validation", 0, verbose);

    RunSuite<ScapeGoatTree<KEYTYPE, size_t>, KEYTYPE> (testDataLength, verbose, 1);

    typedef ScapeGoatTree<int, int, ScapeGoatTraits<
        KeyValue<int,int>,
        ADT::DefaultComp<int>,
        ADT::eDuplicateAddThrowException,
        kDefaultScapeGoatBalance> >  NoDupAddTree;

    DuplicateAddBehaviorTest<ScapeGoatTree<int, int>, NoDupAddTree> (testDataLength, verbose, 1);

   typedef    ScapeGoatTree<int, int, ScapeGoatTraits<
        KeyValue<int,int>,
        ADT::DefaultComp<int>,
        ADT::eInvalidRemoveIgnored,
        kDefaultScapeGoatBalance > > InvalidRemoveIgnoredTree;
    InvalidRemoveBehaviorTest<ScapeGoatTree<int, int>, InvalidRemoveIgnoredTree> (verbose, 1);

    typedef    ScapeGoatTree<string, string, ScapeGoatTraits<
        SharedStringKeyValue,
        CaseInsensitiveCompare,
        ADT::eDefaultPolicy,
        kDefaultScapeGoatBalance > > SharedCaseInsensitiveString;
    StringTraitOverrideTest<SharedCaseInsensitiveString> (verbose, 1);

    typedef ScapeGoatTree<HashKey<string>, string>   HashedString;
    HashedStringTest<HashedString> (verbose, 1);

    SimpleIteratorTest< ScapeGoatTree<size_t, size_t> > (testDataLength, verbose, 1);
    PatchingIteratorTest<Tree_Patching<ScapeGoatTree<size_t, size_t> > > (testDataLength, verbose, 1);
}

#endif


    }   // namespace BinaryTree
}   // namespace ADT

