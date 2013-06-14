#pragma once

#include <algorithm>
#include <math.h>


#if qDebug
	#include <iostream>
    #include "../Shared/Headers/ContainerValidation.h"
#endif

#include "../Shared/Headers/Utils.h"



namespace   ADT {
    namespace   BinaryTree {

template <typename KEY, typename VALUE, typename TRAITS>
RedBlackTree<KEY,VALUE,TRAITS>::RedBlackTree () :
	fHead (nullptr),
	fLength (0)
#if qKeepADTStatistics
    ,fCompares (0)
    ,fRotations (0)
#endif
{
}


template <typename KEY, typename VALUE, typename TRAITS>
RedBlackTree<KEY,VALUE,TRAITS>::RedBlackTree (const RedBlackTree& t) :
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
RedBlackTree<KEY,VALUE,TRAITS>::~RedBlackTree ()
{
	RemoveAll ();
}

template <typename KEY, typename VALUE, typename TRAITS>
RedBlackTree<KEY,VALUE,TRAITS>& RedBlackTree<KEY,VALUE,TRAITS>::operator= (const RedBlackTree& t)
{
	RemoveAll ();
	fHead = DuplicateBranch (t.fHead);
	fLength = t.fLength;
	return *this;
}

template <typename KEY, typename VALUE, typename TRAITS>
bool	RedBlackTree<KEY,VALUE,TRAITS>::Find (const KeyType& key, ValueType* val)  const
{
	int	comp;
	Node* n = FindNode (key, &comp);
	if (n != nullptr and (comp == 0)) {
		if (val != nullptr) {
			*val = n->fEntry.GetValue ();
		}
		return true;
	}
	return false;
}



template <typename KEY, typename VALUE, typename TRAITS>
void	RedBlackTree<KEY,VALUE,TRAITS>::Add (const KeyType& key, ValueType val)
{
 	AddNode (new Node (key, val));
}

template <typename KEY, typename VALUE, typename TRAITS>
void	RedBlackTree<KEY,VALUE,TRAITS>::Add (const KeyType& keyAndValue)
{
    Add (keyAndValue, keyAndValue);
}


template <typename KEY, typename VALUE, typename TRAITS>
typename RedBlackTree<KEY,VALUE,TRAITS>::Node* RedBlackTree<KEY,VALUE,TRAITS>::Rotate (Node* n, Direction rotateDir)
{
	RequireNotNull (n);

	#if qKeepADTStatistics
		++fRotations;
	#endif

    Direction otherDir = Node::OtherDir (rotateDir);
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
typename RedBlackTree<KEY,VALUE,TRAITS>::Node*	RedBlackTree<KEY,VALUE,TRAITS>::AddNode (Node* n)
{
	RequireNotNull (n);

    bool    allowDuplicateAdds = not (TRAITS::kPolicy & ADT::eDuplicateAddThrowException);
	int	comp;
	Node* nearest =  FindNode (n->fEntry.GetKey (), &comp, allowDuplicateAdds);
	if (nearest == nullptr) {
	    Assert (fHead == nullptr);
	    fHead = n;
	    n->SetColor (kBlack);
    }
	else {
		n->SetParent (nearest);
		if (comp == 0) {
		    if (not allowDuplicateAdds) {
		        throw DuplicateAddException ();
		    }
			Assert (nearest->GetChild (kLeft) == nullptr);
		    nearest->SetChild (kLeft, n);
		}
		else if (comp < 0) {
			Assert (nearest->GetChild (kLeft) == nullptr);
		    nearest->SetChild (kLeft, n);
		}
		else {
			Assert (nearest->GetChild (kRight) == nullptr);
		    nearest->SetChild (kRight, n);
		}
	}

	fLength++;

    FixAfterInsert (n);
	return nearest;
}


template <typename KEY, typename VALUE, typename TRAITS>
void	RedBlackTree<KEY,VALUE,TRAITS>::FixAfterInsert (Node* node)
{
    RequireNotNull (node);
    Node*   parent = node->GetParent ();

    if (parent == nullptr) {
 		Assert (fHead == node);
 		Assert (node->GetColor () == kBlack);
        return;
    }

    node->SetColor (kRed);
    while (Node::IsRed (parent)) {
        Node* grandparent = parent->GetParent ();
        if (grandparent == nullptr) {
            parent = fHead;
            parent->SetColor (kBlack);
            break;
        }
        Direction parentDir = Node::GetChildDir (parent);
        Node* uncle = grandparent->GetChild (Node::OtherDir (parentDir));
        if (Node::IsRed (uncle)) {
            parent->SetColor (kBlack);
            uncle->SetColor (kBlack);
            node = grandparent;
            parent = node->GetParent ();
            grandparent->SetColor ((parent == nullptr) ? kBlack : kRed);
       }
        else {
            if (node->IsChild (Node::OtherDir (parentDir))) {
                node = parent;
                Rotate (node, parentDir);
            }
            parent = node->GetParent ();
            AssertNotNull (parent);
            parent->SetColor (kBlack);

            grandparent = parent->GetParent ();
            if (grandparent != nullptr) {
                grandparent->SetColor (kRed);
                Rotate (grandparent, Node::OtherDir (parentDir));
            }
        }
    }
}

template <typename KEY, typename VALUE, typename TRAITS>
void	RedBlackTree<KEY,VALUE,TRAITS>::Remove (const KeyType& key)
{
	int	comp;
	Node* n =  FindNode (key, &comp);

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
void	RedBlackTree<KEY,VALUE,TRAITS>::RemoveNode (Node* n)
{
	RequireNotNull (n);

	Node*   sub = nullptr;
	if (n->GetChild (kLeft) == nullptr or n->GetChild (kRight) == nullptr) {
	    sub = n;
	}
	else {
	    sub = Node::GetFirst (n->GetChild (kRight));
	}
	AssertNotNull (sub);
	Node* x = (sub->GetChild (kLeft) != nullptr) ? sub->GetChild (kLeft) : sub->GetChild (kRight);
	Direction yDir = Node::GetChildDir (sub);

	if (yDir == kBadDir) {
        Assert (n->GetChild (kLeft) == nullptr or n->GetChild (kRight) == nullptr);
	    Assert (sub == fHead);
	    Assert (sub == n);
	    fHead = x;
	    if (fHead != nullptr) {
	        fHead->SetColor (kBlack);
	        fHead->SetParent (nullptr);
	    }
	}
    else {
        Node* parent = sub->GetParent ();
        Node::SetChild_Safe (parent, x, yDir);
        if (sub != n) {
            n->fEntry = sub->fEntry;
        }
        if (x == nullptr) {
            if (sub->GetColor () == kBlack) {
                // here comes trickery. Still need to patch things up, and patch routine needs to be
                // sent a node that is black, and that has the proper parent set. Fortunately, the sub
                // node is out of the chain and about to be deleted, so play with it
                sub->SetParent (parent);
                x = sub;
                Node::SetChild_Safe (parent, x, yDir);
                FixAfterRemove (x);
                Node::SetChild_Safe (parent, nullptr, yDir);
            }
        }
        else if (sub->GetColor () == kBlack) {
            FixAfterRemove (x);
        }
    }

    delete sub;
    Assert (fLength > 0);
    --fLength;

    Ensure ((fHead == nullptr) == (fLength == 0));
}

template <typename KEY, typename VALUE, typename TRAITS>
void	RedBlackTree<KEY,VALUE,TRAITS>::FixAfterRemove (Node* n)
{
    RequireNotNull (n);

    while (n != fHead and n->GetColor () == kBlack) {
        AssertNotNull (n->GetParent ());     // else n == fHead

        Direction dir = Node::GetChildDir (n);
        if (dir == kBadDir) {
            fHead = n;
            break;
        }
        Direction oDir = Node::OtherDir (dir);
        Node* sibling = n->GetParent ()->GetChild (oDir);
        AssertNotNull (sibling);    // reasons this is true are subtle, but otherwise we would be 2 off in our black node count, which cannot be

        if (Node::IsRed (sibling)) {
            sibling->SetColor (kBlack);
            n->GetParent ()->SetColor (kRed);
            Rotate (n->GetParent (), dir);
            sibling = n->GetParent ()->GetChild (oDir);
       }

        if (not Node::IsRed (sibling->GetChild (kLeft)) and not Node::IsRed (sibling->GetChild (kRight))) {
            sibling->SetColor (kRed);
           n = n->GetParent ();
        }
        else {
            if (not Node::IsRed (sibling->GetChild (oDir))) {
                AssertNotNull (sibling->GetChild (dir));
                sibling->GetChild (dir)->SetColor (kBlack);
                sibling->SetColor (kRed);
                Rotate (sibling, oDir);
                sibling = n->GetParent ()->GetChild (oDir);
            }
            sibling->SetColor (n->GetParent ()->GetColor ());
            n->GetParent ()->SetColor (kBlack);
            if (sibling->GetChild (oDir) != nullptr) {
                sibling->GetChild (oDir)->SetColor (kBlack);
            }
            Rotate (n->GetParent (), dir);
            n = fHead;
        }

    }
    n->SetColor (kBlack);
}

template <typename KEY, typename VALUE, typename TRAITS>
void	RedBlackTree<KEY,VALUE,TRAITS>::RemoveAll ()
{
	std::function<void(Node*)>	DeleteANode = [&DeleteANode] (Node* n)
	{
		if (n != nullptr) {
			DeleteANode (n->GetChild (kLeft));
			DeleteANode (n->GetChild (kRight));
			delete n;
		}
	};

	DeleteANode (fHead);

	fHead = nullptr;
	fLength = 0;
}

template <typename KEY, typename VALUE, typename TRAITS>
size_t	RedBlackTree<KEY,VALUE,TRAITS>::GetLength () const
{
	Assert ((fLength == 0) == (fHead == nullptr));
	return fLength;
}


template <typename KEY, typename VALUE, typename TRAITS>
typename RedBlackTree<KEY,VALUE,TRAITS>::Node*	RedBlackTree<KEY,VALUE,TRAITS>::FindNode (const KeyType& key, int* comparisonResult, bool ignoreMatch)  const
{
	RequireNotNull (comparisonResult);

	Node*	n = fHead;
	Node*	nearest = n;
	while (n != nullptr) {
		#if qKeepADTStatistics
			const_cast<RedBlackTree<KEY,VALUE,TRAITS> *> (this)->fCompares++;
		#endif
		nearest = n;
		*comparisonResult = TRAITS::Comparer::Compare (key, n->fEntry.GetKey ());
		if (*comparisonResult == 0 and (not ignoreMatch)) {
			return n;
		}
		n = (*comparisonResult <= 0) ? n->GetChild (kLeft): n->GetChild (kRight);
	}
	return nearest;
}


template <typename KEY, typename VALUE, typename TRAITS>
typename RedBlackTree<KEY,VALUE,TRAITS>::Node*	RedBlackTree<KEY,VALUE,TRAITS>::GetFirst ()  const
{
	return Node::GetFirst (fHead);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename RedBlackTree<KEY,VALUE,TRAITS>::Node*	RedBlackTree<KEY,VALUE,TRAITS>::GetLast ()  const
{
	return Node::GetLast (fHead);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename RedBlackTree<KEY,VALUE,TRAITS>::Node*	RedBlackTree<KEY,VALUE,TRAITS>::Node::GetFirst (Node* n)
{
 	while (n != nullptr and n->GetChild (kLeft) != nullptr) {
		n = n->GetChild (kLeft);
	}
	return n;
}


template <typename KEY, typename VALUE, typename TRAITS>
typename RedBlackTree<KEY,VALUE,TRAITS>::Node*	RedBlackTree<KEY,VALUE,TRAITS>::Node::GetLast (Node* n)
{
  	while (n != nullptr and n->GetChild (kRight) != nullptr) {
		n = n->GetChild (kRight);
	}
	return n;
}


template <typename KEY, typename VALUE, typename TRAITS>
typename RedBlackTree<KEY,VALUE,TRAITS>::Node*	RedBlackTree<KEY,VALUE,TRAITS>::DuplicateBranch (Node* branchTop)
{
	Node* newNode = nullptr;
	if (branchTop != nullptr) {
		newNode = new Node (*branchTop);
		newNode->SetChild (kLeft,  DuplicateBranch (branchTop->GetChild (kLeft)));
		newNode->SetChild (kRight, DuplicateBranch (branchTop->GetChild (kRight)));
	}
	return newNode;
}


template <typename KEY, typename VALUE, typename TRAITS>
void	RedBlackTree<KEY,VALUE,TRAITS>::ReBalance ()
{
    if (GetLength () == 0) {
        return;
    }

	// better to build on the stack
	Node**	nodeList = new Node* [GetLength ()];
	int	curIndex = 0;

	// stuff the array with the nodes. Better if have iterator support
	std::function<void(Node*)>	AssignNodeToArray = [&AssignNodeToArray, &nodeList, &curIndex] (Node* n)
	{
	    if (n != nullptr) {
			AssignNodeToArray (n->GetChild (kLeft));
            nodeList[curIndex++] = n;
			AssignNodeToArray (n->GetChild (kRight));
        }
	};

	AssignNodeToArray (fHead);

	// from now on, working with an array (nodeList) that has all the tree nodes in sorted order
	size_t	maxHeight = size_t (log (double (GetLength ()))/log (2.0))+1;

	std::function<Node*(int startIndex, int endIndex, int curNodeHeight)>	BalanceNode = [&BalanceNode, &nodeList, &maxHeight] (int startIndex, int endIndex, size_t curNodeHeight) -> Node*
	{
		Require (startIndex <= endIndex);
		Assert (curNodeHeight <= maxHeight);
		if (startIndex == endIndex) {
			Node* n = nodeList[startIndex];
	//		n->SetColor ((curNodeHeight == maxHeight or (not (curNodeHeight & 1))) ? kRed : kBlack);
			n->SetColor ((curNodeHeight == maxHeight) ? kRed : kBlack);
			n->SetChild (kLeft, nullptr);
			n->SetChild (kRight, nullptr);
			return n;
		}

		int curIdx = startIndex + (endIndex-startIndex)/2;
		Assert (curIdx <= endIndex);
		Assert (curIdx >= startIndex);

		Node* n = nodeList[curIdx];
		AssertNotNull (n);

        n->SetChild (kLeft, (curIdx == startIndex) ? nullptr : BalanceNode (startIndex, curIdx-1, curNodeHeight+1));
        n->SetChild (kRight, (curIdx == endIndex) ? nullptr : BalanceNode (curIdx+1, endIndex, curNodeHeight+1));
        n->SetColor (kBlack);

		return n;
	};
	if (fHead != nullptr) {
		fHead = BalanceNode (0, GetLength ()-1, 1);
		fHead->SetParent (nullptr);
		fHead->SetColor (kBlack);
	}

	delete[] nodeList;
}

template <typename KEY, typename VALUE, typename TRAITS>
RedBlackTree<KEY,VALUE,TRAITS>::Node::Node (const KeyType& key, const ValueType& val)	:
	fEntry (key, val),
 	fParent (nullptr),
    fColor (kBlack)
{
    SetChild (kLeft, nullptr);
    SetChild (kRight, nullptr);
}

template <typename KEY, typename VALUE, typename TRAITS>
RedBlackTree<KEY,VALUE,TRAITS>::Node::Node (const Node& n)	:
	fEntry (n.fEntry),
	fParent (nullptr),
    fColor (n.fColor)
{
    SetChild (kLeft, nullptr);
    SetChild (kRight, nullptr);
}

 template <typename KEY, typename VALUE, typename TRAITS>
 typename RedBlackTree<KEY,VALUE,TRAITS>::Node::Node*   RedBlackTree<KEY,VALUE,TRAITS>::Node::GetChild (Direction direction)
 {
    Require (direction == kLeft or direction == kRight);
    return (fChildren[direction]);
}

template <typename KEY, typename VALUE, typename TRAITS>
 void   RedBlackTree<KEY,VALUE,TRAITS>::Node::SetChild (Direction direction, Node* n)
{
    Require (direction == kLeft or direction == kRight);
    fChildren[direction] = n;
    if (n != nullptr) {
        n->fParent = this;
    }
}

template <typename KEY, typename VALUE, typename TRAITS>
void    RedBlackTree<KEY,VALUE,TRAITS>::Node::SetChild_Safe (Node* parent, Node* n, Direction d)
{
   Require (parent == nullptr or d == kLeft or d == kRight);
   if (parent == nullptr) {
       if (n != nullptr) {
           n->fParent = nullptr;
       }
   }
   else {
       parent->SetChild (d, n);
   }
}

template <typename KEY, typename VALUE, typename TRAITS>
bool    RedBlackTree<KEY,VALUE,TRAITS>::Node::IsChild (Direction direction)
{
    Require (direction == kLeft or direction == kRight);
    return (fParent != nullptr and fParent->GetChild (direction) == this);
}

template <typename KEY, typename VALUE, typename TRAITS>
Direction     RedBlackTree<KEY,VALUE,TRAITS>::Node::GetChildDir (Node* n)
{
    if (n != nullptr and n->GetParent () != nullptr) {
        if (n == n->fParent->GetChild (kLeft)) {
            return kLeft;
        }
        if (n == n->fParent->GetChild (kRight)) {
            return kRight;
        }
    }
    return kBadDir;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename RedBlackTree<KEY,VALUE,TRAITS>::Node*   RedBlackTree<KEY,VALUE,TRAITS>::Node::GetParent () const
{
    return fParent;
}

template <typename KEY, typename VALUE, typename TRAITS>
void    RedBlackTree<KEY,VALUE,TRAITS>::Node::SetParent (Node* p)
{
    fParent = p;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename RedBlackTree<KEY,VALUE,TRAITS>::Color    RedBlackTree<KEY,VALUE,TRAITS>::Node::GetColor () const
{
    return fColor;
}

template <typename KEY, typename VALUE, typename TRAITS>
void    RedBlackTree<KEY,VALUE,TRAITS>::Node::SetColor (Color c)
{
    fColor = c;
}

template <typename KEY, typename VALUE, typename TRAITS>
Direction    RedBlackTree<KEY,VALUE,TRAITS>::Node::OtherDir (Direction dir)
{
    Require (dir == kLeft or dir == kRight);
    return ((dir == kLeft) ? kRight : kLeft);
}

template <typename KEY, typename VALUE, typename TRAITS>
bool    RedBlackTree<KEY,VALUE,TRAITS>::Node::IsRed (Node* n)
{
    return (n != nullptr and n->GetColor () == kRed);
}


#if qDebug

template <typename KEY, typename VALUE, typename TRAITS>
void	RedBlackTree<KEY,VALUE,TRAITS>::ValidateBranch (Node* n, size_t& count, size_t& blackNodesToHead)
{
	RequireNotNull (n);
	++count;

    /*
    Da Rules:
    1) A node is either red or black.
    2) The root is black.
    3) All leaves are black
    4) Both children of every red node are black.
    5) Every simple path from a given node to any of its descendant leaves contains the same number of black nodes.
    */

    auto    CountBlacksToHead = [] (Node* nn) -> size_t
    {
        RequireNotNull (nn);
        size_t blackNodeCount = 0;
        while (nn->GetParent ()!= nullptr) {
            if (nn->GetColor () == kBlack) {
                blackNodeCount++;
            }
            nn = nn->GetParent ();
        }
        return blackNodeCount;
    };

    if (n->GetColor () == kRed) {
        Assert (not Node::IsRed (n->GetChild (kLeft)));
        Assert (not Node::IsRed (n->GetChild (kRight)));
   }

    for (int i = kFirstChild; i <= kLastChild; ++i) {
        Node* child = n->GetChild (Direction (i));
        if (child != nullptr) {
            int comp = TRAITS::Comparer::Compare (n->fEntry.GetKey (), child->fEntry.GetKey ());
            Assert ((comp == 0) or ((comp > 0) == (i == kLeft)));
            Assert (child->GetParent () == n);

            ValidateBranch (child, count, blackNodesToHead);
        }
        else {
            size_t blackCount = CountBlacksToHead (n);
            if (blackNodesToHead == 0) {
                if (blackCount > 0) {
 //std::cout << "bc = " << blackCount << std::endl << std::flush;
                }
                blackNodesToHead = blackCount;
            }
            else {
                Assert (blackCount == blackNodesToHead);
            }
        }
    }
}

template <typename KEY, typename VALUE, typename TRAITS>
void	RedBlackTree<KEY,VALUE,TRAITS>::ValidateAll () const
{
	size_t	count = 0;
	size_t  blackNodesToHead = 0;


	if (fHead != nullptr) {
 //std::cout << "Validating, count = " << fLength << std::endl << std::flush;
	    Assert (fHead->GetColor () == kBlack);
		ValidateBranch (fHead, count, blackNodesToHead);
	}
	Assert (count == fLength);
}

template <typename KEY, typename VALUE, typename TRAITS>
void	RedBlackTree<KEY,VALUE,TRAITS>::ListAll () const
{
	std::function<void(Node*)>	ListNode = [&ListNode] (Node* n)
	{
	    if (n != nullptr) {
 			ListNode (n->GetChild (kLeft));
            int cCount = 0;
            if (n->GetChild (kLeft) != nullptr) cCount++;
            if (n->GetChild (kRight) != nullptr) cCount++;
            std::cout << "(" << n->fEntry.GetKey () << "," << ((n->GetColor () == kRed) ? "R" : "B") << "," << cCount << ")";
 			ListNode (n->GetChild (kRight));
       }
	};

    std::cout << "[";
	if (fHead != nullptr) {
	    std::cout << "H = " << fHead->fEntry.GetKey () << " :";
		ListNode (fHead);
	}
    std::cout << "]" << std::endl;
}


#endif

#if qKeepADTStatistics

template <typename KEY, typename VALUE, typename TRAITS>
size_t	RedBlackTree<KEY,VALUE,TRAITS>::CalcNodeHeight (Node* n, size_t height, size_t* totalHeight)
{
	if (n == nullptr) {
		if (totalHeight != nullptr) {
			*totalHeight += height;
		}
		return height;
	}

	size_t	newHeight = std::max (
		CalcNodeHeight (n->GetChild (kLeft), height+1, totalHeight),
		CalcNodeHeight (n->GetChild (kRight), height+1, totalHeight));

	return newHeight;
}



template <typename KEY, typename VALUE, typename TRAITS>
size_t	RedBlackTree<KEY,VALUE,TRAITS>::CalcHeight (size_t* totalHeight) const
{
	return CalcNodeHeight (fHead, 0, totalHeight);
}

#endif /* qKeepADTStatistics */

#if qDebug


template <typename KEYTYPE>
void    RedBlackTreeValidationSuite (size_t testDataLength, bool verbose)
{
    TestTitle   tt ("Red-Black Tree Validation", 0, verbose);

    RunSuite<RedBlackTree<KEYTYPE, size_t>, KEYTYPE> (testDataLength, verbose, 1);

    typedef RedBlackTree<int, int, ADT::Traits<
        KeyValue<int,int>,
        ADT::DefaultComp<int>,
        ADT::eDuplicateAddThrowException> >  NoDupAddTree;
    DuplicateAddBehaviorTest<RedBlackTree<int, int>, NoDupAddTree> (testDataLength, verbose, 1);

   typedef    RedBlackTree<int, int, ADT::Traits<
        KeyValue<int,int>,
        ADT::DefaultComp<int>,
        ADT::eInvalidRemoveIgnored> > InvalidRemoveIgnoredTree;
    InvalidRemoveBehaviorTest<RedBlackTree<int, int>, InvalidRemoveIgnoredTree> (verbose, 1);

    typedef    RedBlackTree<string, string, ADT::Traits<
        SharedStringKeyValue,
        CaseInsensitiveCompare,
        ADT::eDefaultPolicy> > SharedCaseInsensitiveString;
    StringTraitOverrideTest<SharedCaseInsensitiveString> (verbose, 1);

    typedef RedBlackTree<HashKey<string>, string>   HashedString;
    HashedStringTest<HashedString> (verbose, 1);
}

#endif


    }   // namespace BinaryTree
}   // namespace ADT
