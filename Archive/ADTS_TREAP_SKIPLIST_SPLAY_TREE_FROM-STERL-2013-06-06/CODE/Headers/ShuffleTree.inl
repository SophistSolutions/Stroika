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
 ShuffleTree<KEY,VALUE,TRAITS>::ShuffleTree () :
	fHead (nullptr),
	fLength (0)
  #if qKeepADTStatistics
      ,fCompares (0)
      ,fRotations (0)
  #endif
{
}

template <typename KEY, typename VALUE, typename TRAITS>
ShuffleTree<KEY,VALUE,TRAITS>::ShuffleTree (const ShuffleTree& t) :
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
ShuffleTree<KEY,VALUE,TRAITS>& ShuffleTree<KEY,VALUE,TRAITS>::operator= (const ShuffleTree& t)
{
	RemoveAll ();
	fLength = t.fLength;
	fHead = DuplicateBranch (t.fHead);

	return *this;
}


template <typename KEY, typename VALUE, typename TRAITS>
ShuffleTree<KEY,VALUE,TRAITS>::~ShuffleTree ()
{
	RemoveAll ();
}

template <typename KEY, typename VALUE, typename TRAITS>
bool	ShuffleTree<KEY,VALUE,TRAITS>::Find (const KeyType& key, ValueType* val)
{
	int	comparisonResult;
	Node* n = FindNode (key, &comparisonResult);
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
typename ShuffleTree<KEY,VALUE,TRAITS>::Node* ShuffleTree<KEY,VALUE,TRAITS>::Rotate (Node* n, Direction rotateDir)
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
void    ShuffleTree<KEY,VALUE,TRAITS>::Prioritize (Node* n)
{
    RequireNotNull (n);

    while (n->GetParent () != nullptr) {
        Rotate (n->GetParent (), Node::OtherDir (Node::GetChildDir (n)));
    }
    Assert (n == fHead);
}

template <typename KEY, typename VALUE, typename TRAITS>
void	ShuffleTree<KEY,VALUE,TRAITS>::Add (const KeyType& key, const ValueType& val)
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
void	ShuffleTree<KEY,VALUE,TRAITS>::Add (const KeyType& keyAndValue)
{
    Add (keyAndValue, keyAndValue);
}


template <typename KEY, typename VALUE, typename TRAITS>
void	ShuffleTree<KEY,VALUE,TRAITS>::AddNode (Node* n)
{
	RequireNotNull (n);

	int	comp;
	Node* nearest =  FindNode (n->fEntry.GetKey (), &comp);
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
			    n->SetChild (kLeft, nearest->GetChild (kLeft));
				nearest->SetChild (kLeft, n);
			}
			else {
			    n->SetChild (kRight, nearest->GetChild (kRight));
				nearest->SetChild (kRight, n);
			}
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
}



template <typename KEY, typename VALUE, typename TRAITS>
void	ShuffleTree<KEY,VALUE,TRAITS>::Remove (const KeyType& key)
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
void	ShuffleTree<KEY,VALUE,TRAITS>::SwapNodes (Node* parent, Node* child)
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
void	ShuffleTree<KEY,VALUE,TRAITS>::RemoveNode (Node* n)
{
	RequireNotNull (n);

	if (n->GetChild (kLeft) == nullptr and n->GetChild (kRight) == nullptr) {
	    SwapNodes (n, nullptr);
	}
	else if (n->GetChild (kLeft) == nullptr) {
		SwapNodes (n, n->GetChild (kRight));
	}
	else if (n->GetChild (kRight) == nullptr) {
		SwapNodes (n, n->GetChild (kLeft));
	}
	else {
       Direction d = (FlipCoin ()) ? kRight : kLeft;

		Node* minNode = (d == kRight)
            ? Node::GetFirst (n->GetChild (d))
            : Node::GetLast (n->GetChild (d));

		AssertNotNull (minNode);
		if (minNode->GetParent () != n) {
			SwapNodes (minNode, minNode->GetChild (d));
			minNode->SetChild (d, n->GetChild (d));
		}
		SwapNodes (n, minNode);
		minNode->SetChild (Node::OtherDir (d), n->GetChild (Node::OtherDir (d)));
	}

	delete n;
	--fLength;
}

template <typename KEY, typename VALUE, typename TRAITS>
void	ShuffleTree<KEY,VALUE,TRAITS>::RemoveAll ()
{
	// iterate rather than natural tail recursive version because splay trees get deep
	std::stack<Node*> nodes;
	if (fHead != nullptr) {
		nodes.push(fHead);
		while (not nodes.empty ()) {
			Node* curNode = nodes.top (); nodes.pop ();

			AssertNotNull (curNode);
			if (curNode->GetChild (kLeft) != nullptr) {
				nodes.push (curNode->GetChild (kLeft));
			}
			if (curNode->GetChild (kRight) != nullptr) {
				nodes.push (curNode->GetChild (kRight));
			}
			delete curNode;
		}

		fHead = nullptr;
	}
	fLength = 0;
}

template <typename KEY, typename VALUE, typename TRAITS>
size_t	ShuffleTree<KEY,VALUE,TRAITS>::GetLength () const
{
	Assert ((fLength == 0) == (fHead == nullptr));
	return fLength;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename ShuffleTree<KEY,VALUE,TRAITS>::Node*	ShuffleTree<KEY,VALUE,TRAITS>::FindNode (const KeyType& key, int* comparisonResult)
{
	RequireNotNull (comparisonResult);

	bool    shuffled = (GetLength () == 0);
	size_t  shuffleCounter = (shuffled) ? 0 : (rand () % GetLength ());

	Node*	n = fHead;
	Node*	nearest = n;
	*comparisonResult = 0;
	while (n != nullptr) {

		#if qKeepADTStatistics
			const_cast<ShuffleTree<KEY,VALUE,TRAITS> *> (this)->fCompares++;
		#endif
		nearest = n;
		*comparisonResult = TRAITS::Comparer::Compare (key, n->fEntry.GetKey ());
		if (*comparisonResult == 0) {
			return n;
		}

		if (*comparisonResult < 0) {
		    n = n->GetChild (kLeft);
		    if ((not shuffled) and (shuffleCounter == 0) and (n != nullptr)) {
		        nearest =   Rotate (nearest, kRight);
		        shuffled = true;
		    }

		}
		else {
		    n = n->GetChild (kRight);
		    if ((not shuffled) and (shuffleCounter == 0) and (n != nullptr)) {
		        nearest =   Rotate (nearest, kLeft);
		        shuffled = true;
		    }
		}

	    if (not shuffled) {
            shuffleCounter = (shuffleCounter-1)/2;
	    }
	}
	return nearest;
}



template <typename KEY, typename VALUE, typename TRAITS>
typename	ShuffleTree<KEY,VALUE,TRAITS>::Node*	ShuffleTree<KEY,VALUE,TRAITS>::GetFirst () const
{
    return Node::GetFirst (fHead);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename	ShuffleTree<KEY,VALUE,TRAITS>::Node*	ShuffleTree<KEY,VALUE,TRAITS>::GetLast () const
{
    return Node::GetLast (fHead);
}


template <typename KEY, typename VALUE, typename TRAITS>
typename ShuffleTree<KEY,VALUE,TRAITS>::Node*	ShuffleTree<KEY,VALUE,TRAITS>::DuplicateBranch (Node* n)
{
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
					newParent->SetChild (kLeft, newNode);
				}
				else {
					newParent->SetChild (kRight, newNode);
				}
			}
			if (branchTop->GetChild (kLeft) != nullptr) {
				nodes.push (branchTop->GetChild (kLeft));
				parents.push (newNode);
				childIsLeft.push (true);
			}
			if (branchTop->GetChild (kRight) != nullptr) {
				nodes.push (branchTop->GetChild (kRight));
				parents.push (newNode);
				childIsLeft.push (false);
			}
		}
	}
	return newTop;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename ShuffleTree<KEY,VALUE,TRAITS>::Node*   ShuffleTree<KEY,VALUE,TRAITS>::Node::RebalanceBranch (Node* oldTop, size_t length)
{
    if (oldTop == nullptr) {
        Assert (length == 0);
        return nullptr;
    }

    // technique discussed in Galperin and Rivest, see http://www.akira.ruc.dk/~keld/teaching/algoritmedesign_f07/Artikler/03/Galperin93.pdf


    // turns the tree into a linked list (by fRight pointers) in sort order
    // currently slightly slower than building an array, but no mallocs
    // conceptually, flatten is just plain old iteration, but tree iteration is naturally recursive, unfortunately
	std::function<Node*(Node*,Node*)>	Flatten = [&Flatten] (Node* x, Node* y)
	{
        x->fParent = nullptr;

        Node* cRight = x->fChildren[kRight];
        Node* cLeft = x->fChildren[kLeft];

        x->SetChild (kLeft, nullptr);

        x->fChildren[kRight] = (cRight == nullptr) ? y : Flatten (cRight, y);
        return (cLeft == nullptr) ? x :  Flatten (cLeft, x);
	};

    // takes the results of flatten (ordered linked list, by right child pointers) and constructs the proper
    // balanced tree out of it
	std::function<Node*(size_t,Node*)>	RebuildBranch = [&RebuildBranch] (size_t n, Node* x)
	{
	    RequireNotNull (x);
	    Require (n > 0);

        size_t  tmp = (n-1)/2;
        Node* r = (tmp == 0) ? x : RebuildBranch (tmp, x);
        AssertNotNull (r);

        tmp = n/2;
        Node* s = (tmp == 0) ? r->GetChild (kRight) : RebuildBranch (tmp, r->GetChild (kRight));
        AssertNotNull (s);

        r->SetChild (kRight, s->GetChild (kLeft));
        s->SetChild (kLeft, r);
        return s;
	};

    Node*   oldParent = oldTop->GetParent ();
    Direction   oldDir = GetChildDir (oldTop);

	Node    dummy = *oldTop;
    Node* result = RebuildBranch (length, Flatten (oldTop, &dummy));
    result = result->GetChild (kLeft);  // original result was our dummy last
    AssertNotNull (result);

    SetChild_Safe (oldParent, result, oldDir);
    return result;
}

template <typename KEY, typename VALUE, typename TRAITS>
void	ShuffleTree<KEY,VALUE,TRAITS>::ReBalance ()
{
    fHead = Node::RebalanceBranch (fHead, fLength);
}


// my old code for this, which works fine but takes N space rather than the log(N) space (from recursion) of the RebalanceBranch implementation
#if 0
template <typename KEY, typename VALUE, typename TRAITS>
void	ShuffleTree<KEY,VALUE,TRAITS>::ReBalance ()
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

		return n;
	};
	if (fHead != nullptr) {
		fHead = BalanceNode (0, GetLength ()-1, 1);
		fHead->SetParent (nullptr);
	}

	delete[] nodeList;
}
#endif

template <typename KEY, typename VALUE, typename TRAITS>
ShuffleTree<KEY,VALUE,TRAITS>::Node::Node (const KeyType& key, const ValueType& val)	:
	fEntry (key, val),
	fParent (nullptr)
{
    fChildren[kLeft] = nullptr;
    fChildren[kRight] = nullptr;
}

template <typename KEY, typename VALUE, typename TRAITS>
ShuffleTree<KEY,VALUE,TRAITS>::Node::Node (const Node& n)	:
	fEntry (n.fEntry),
	fParent (nullptr)
{
    fChildren[kLeft] = nullptr;
    fChildren[kRight] = nullptr;
}


template <typename KEY, typename VALUE, typename TRAITS>
typename ShuffleTree<KEY,VALUE,TRAITS>::Node*   ShuffleTree<KEY,VALUE,TRAITS>::Node::GetParent () const
{
    return fParent;
}

template <typename KEY, typename VALUE, typename TRAITS>
void    ShuffleTree<KEY,VALUE,TRAITS>::Node::SetParent (Node* p)
{
    fParent = p;
}

 template <typename KEY, typename VALUE, typename TRAITS>
 typename ShuffleTree<KEY,VALUE,TRAITS>::Node::Node*   ShuffleTree<KEY,VALUE,TRAITS>::Node::GetChild (Direction direction)
 {
    Require (direction == kLeft or direction == kRight);
    return (fChildren[direction]);
}

template <typename KEY, typename VALUE, typename TRAITS>
 void   ShuffleTree<KEY,VALUE,TRAITS>::Node::SetChild (Direction direction, Node* n)
{
    Require (direction == kLeft or direction == kRight);
    fChildren[direction] = n;
    if (n != nullptr) {
        n->fParent = this;
    }
}
template <typename KEY, typename VALUE, typename TRAITS>
bool    ShuffleTree<KEY,VALUE,TRAITS>::Node::IsChild (Direction direction)
{
    Require (direction == kLeft or direction == kRight);
    return (fParent != nullptr and fParent->GetChild (direction) == this);
}

template <typename KEY, typename VALUE, typename TRAITS>
Direction     ShuffleTree<KEY,VALUE,TRAITS>::Node::GetChildDir (Node* n)
{
    if (n != nullptr and n->GetParent () != nullptr) {
        if (n == n->GetParent ()->GetChild (kLeft)) {
            return kLeft;
        }
        if (n == n->GetParent ()->GetChild (kRight)) {
            return kRight;
        }
    }
    return kBadDir;
}

template <typename KEY, typename VALUE, typename TRAITS>
Direction    ShuffleTree<KEY,VALUE,TRAITS>::Node::OtherDir (Direction dir)
{
    Require (dir == kLeft or dir == kRight);
    return ((dir == kLeft) ? kRight : kLeft);
}

template <typename KEY, typename VALUE, typename TRAITS>
void    ShuffleTree<KEY,VALUE,TRAITS>::Node::SetChild_Safe (Node* parent, Node* n, Direction d)
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
typename ShuffleTree<KEY,VALUE,TRAITS>::Node*	ShuffleTree<KEY,VALUE,TRAITS>::Node::GetFirst (Node* n)
{
 	while (n != nullptr and n->GetChild (kLeft) != nullptr) {
		n = n->GetChild (kLeft);
	}
	return n;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename ShuffleTree<KEY,VALUE,TRAITS>::Node*	ShuffleTree<KEY,VALUE,TRAITS>::Node::GetLast (Node* n)
{
  	while (n != nullptr and n->GetChild (kRight) != nullptr) {
		n = n->GetChild (kRight);
	}
	return n;
}

#if qDebug

template <typename KEY, typename VALUE, typename TRAITS>
void	ShuffleTree<KEY,VALUE,TRAITS>::ValidateBranch (Node* n, size_t& count)
{
	RequireNotNull (n);
	++count;
	if (n->GetChild (kLeft) != nullptr) {
		Assert (TRAITS::Comparer::Compare (n->fEntry.GetKey (), n->GetChild (kLeft)->fEntry.GetKey ()) >= 0);
		Assert (n->GetChild (kLeft)->GetParent () == n);
		ValidateBranch (n->GetChild (kLeft), count);
	}
	if (n->GetChild (kRight) != nullptr) {
		// we cannot do strict < 0, because rotations can put on either side
		Assert (TRAITS::Comparer::Compare (n->fEntry.GetKey (), n->GetChild (kRight)->fEntry.GetKey ()) <= 0);
		Assert (n->GetChild (kRight)->GetParent () == n);
		ValidateBranch (n->GetChild (kRight), count);
	}
}

template <typename KEY, typename VALUE, typename TRAITS>
void	ShuffleTree<KEY,VALUE,TRAITS>::ValidateAll () const
{
	size_t	count = 0;

	if (fHead != nullptr) {
		ValidateBranch (fHead, count);
	}
	Assert (count == fLength);
}

template <typename KEY, typename VALUE, typename TRAITS>
void	ShuffleTree<KEY,VALUE,TRAITS>::ListAll () const
{
	std::function<void(Node*)>	ListNode = [&ListNode] (Node* n)
	{
		if (n->GetChild (kLeft) != nullptr) {
			ListNode (n->GetChild (kLeft));
		}
        std::cout << "(" << n->fEntry.GetKey () << ")";
		if (n->GetChild (kRight) != nullptr) {
			ListNode (n->GetChild (kRight));
		}
	};

    std::cout << "[";
	if (fHead != nullptr) {
		ListNode (fHead);
	}
    std::cout << "]" << std::endl;
}


#endif

#if qKeepADTStatistics
template <typename KEY, typename VALUE, typename TRAITS>
size_t	ShuffleTree<KEY,VALUE,TRAITS>::CalcHeight (size_t* totalHeight) const
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
			nodes.push (curNode->GetChild (kLeft));
			heights.push (height+1);
			nodes.push (curNode->GetChild (kRight));
		}
	}
	return maxHeight;
}
#endif


#if qDebug

template <typename KEYTYPE>
void    ShuffleTreeValidationSuite (size_t testDataLength, bool verbose)
{
    TestTitle   tt ("ShuffleTree Validation", 0, verbose);

    RunSuite<ShuffleTree<KEYTYPE, size_t>, KEYTYPE> (testDataLength, verbose, 1);

    typedef ShuffleTree<int, int, ADT::Traits<
        KeyValue<int,int>,
        ADT::DefaultComp<int>,
        ADT::eDuplicateAddThrowException > >  NoDupAddTree;

    DuplicateAddBehaviorTest<ShuffleTree<int, int>, NoDupAddTree> (testDataLength, verbose, 1);

   typedef    ShuffleTree<int, int, ADT::Traits<
        KeyValue<int,int>,
        ADT::DefaultComp<int>,
        ADT::eInvalidRemoveIgnored > > InvalidRemoveIgnoredTree;
    InvalidRemoveBehaviorTest<ShuffleTree<int, int>, InvalidRemoveIgnoredTree> (verbose, 1);

    typedef    ShuffleTree<string, string, ADT::Traits<
        SharedStringKeyValue,
        CaseInsensitiveCompare,
        ADT::eDefaultPolicy > > SharedCaseInsensitiveString;
    StringTraitOverrideTest<SharedCaseInsensitiveString> (verbose, 1);

    typedef ShuffleTree<HashKey<string>, string>   HashedString;
    HashedStringTest<HashedString> (verbose, 1);
}

#endif


    }   // namespace BinaryTree
}   // namespace ADT

