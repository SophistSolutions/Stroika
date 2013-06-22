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
 SortedBinaryTree<KEY,VALUE,TRAITS>::SortedBinaryTree () :
	fHead (nullptr),
	fLength (0)
  #if qKeepADTStatistics
      ,fCompares (0)
      ,fRotations (0)
  #endif
{
}

template <typename KEY, typename VALUE, typename TRAITS>
SortedBinaryTree<KEY,VALUE,TRAITS>::SortedBinaryTree (const SortedBinaryTree& t) :
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
SortedBinaryTree<KEY,VALUE,TRAITS>& SortedBinaryTree<KEY,VALUE,TRAITS>::operator= (const SortedBinaryTree& t)
{
	RemoveAll ();
	fLength = t.fLength;
	fHead = DuplicateBranch (t.fHead);

	return *this;
}


template <typename KEY, typename VALUE, typename TRAITS>
SortedBinaryTree<KEY,VALUE,TRAITS>::~SortedBinaryTree ()
{
	RemoveAll ();
}

template <typename KEY, typename VALUE, typename TRAITS>
bool	SortedBinaryTree<KEY,VALUE,TRAITS>::Find (const KeyType& key, ValueType* val) const
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
typename SortedBinaryTree<KEY,VALUE,TRAITS>::Node* SortedBinaryTree<KEY,VALUE,TRAITS>::Rotate (Node* n, Direction rotateDir)
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
void    SortedBinaryTree<KEY,VALUE,TRAITS>::Prioritize (const Iterator& it)
{
    Node*   n = const_cast<Node*> (it.GetNode ());
    RequireNotNull (n);

    while (n->GetParent () != nullptr) {
        Rotate (n->GetParent (), OtherDir (Node::GetChildDir (n)));
    }
    Assert (n == fHead);
}

template <typename KEY, typename VALUE, typename TRAITS>
void	SortedBinaryTree<KEY,VALUE,TRAITS>::Add (const KeyType& key, const ValueType& val)
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
void	SortedBinaryTree<KEY,VALUE,TRAITS>::Add (const KeyType& keyAndValue)
{
    Add (keyAndValue, keyAndValue);
}


template <typename KEY, typename VALUE, typename TRAITS>
void	SortedBinaryTree<KEY,VALUE,TRAITS>::AddNode (Node* n)
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
			    n->SetChild (Direction::kLeft, nearest->GetChild (Direction::kLeft));
				nearest->SetChild (Direction::kLeft, n);
			}
			else {
			    n->SetChild (Direction::kRight, nearest->GetChild (Direction::kRight));
				nearest->SetChild (Direction::kRight, n);
			}
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
}



template <typename KEY, typename VALUE, typename TRAITS>
void	SortedBinaryTree<KEY,VALUE,TRAITS>::Remove (const KeyType& key)
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
void	SortedBinaryTree<KEY,VALUE,TRAITS>::SwapNodes (Node* parent, Node* child)
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
void	SortedBinaryTree<KEY,VALUE,TRAITS>::RemoveNode (Node* n)
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
}

template <typename KEY, typename VALUE, typename TRAITS>
void	SortedBinaryTree<KEY,VALUE,TRAITS>::RemoveAll ()
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
size_t	SortedBinaryTree<KEY,VALUE,TRAITS>::GetLength () const
{
	Assert ((fLength == 0) == (fHead == nullptr));
	return fLength;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename SortedBinaryTree<KEY,VALUE,TRAITS>::Node*	SortedBinaryTree<KEY,VALUE,TRAITS>::FindNode (const KeyType& key, int* comparisonResult)  const
{
	RequireNotNull (comparisonResult);

	Node*	n = fHead;
	Node*	nearest = n;
	*comparisonResult = 0;
	while (n != nullptr) {
		#if qKeepADTStatistics
			const_cast<SortedBinaryTree<KEY,VALUE,TRAITS> *> (this)->fCompares++;
		#endif
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
typename	SortedBinaryTree<KEY,VALUE,TRAITS>::Node*	SortedBinaryTree<KEY,VALUE,TRAITS>::GetFirst () const
{
    return Node::GetFirst (fHead);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename	SortedBinaryTree<KEY,VALUE,TRAITS>::Node*	SortedBinaryTree<KEY,VALUE,TRAITS>::GetLast () const
{
    return Node::GetLast (fHead);
}

template <typename KEY, typename VALUE, typename TRAITS>
void	SortedBinaryTree<KEY,VALUE,TRAITS>::Balance ()
{
    fHead = Node::RebalanceBranch (fHead, fLength);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename SortedBinaryTree<KEY,VALUE,TRAITS>::Iterator SortedBinaryTree<KEY,VALUE,TRAITS>::Iterate (TraversalType t, Direction d) const
{
    return Iterator (Node::GetFirstTraversal (fHead, t, d, nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename SortedBinaryTree<KEY,VALUE,TRAITS>::Iterator SortedBinaryTree<KEY,VALUE,TRAITS>::Iterate (const KeyType& key, TraversalType t, Direction d) const
{
    int	comp;
    return Iterator (Node::GetFirstTraversal (FindNode (key, &comp), t, d, nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename SortedBinaryTree<KEY,VALUE,TRAITS>::Iterator  SortedBinaryTree<KEY,VALUE,TRAITS>::begin (TraversalType t, Direction d) const
{
    return Iterator (Node::GetFirstTraversal (fHead, t, d, nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename    SortedBinaryTree<KEY,VALUE,TRAITS>::Iterator  SortedBinaryTree<KEY,VALUE,TRAITS>::end () const
{
    return Iterator (nullptr);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename    SortedBinaryTree<KEY,VALUE,TRAITS>::Iterator	SortedBinaryTree<KEY,VALUE,TRAITS>::GetFirst (TraversalType t, Direction d) const
{
    return Iterator (Node::GetFirstTraversal (fHead, t, d, nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename    SortedBinaryTree<KEY,VALUE,TRAITS>::Iterator	SortedBinaryTree<KEY,VALUE,TRAITS>::GetLast (TraversalType t, Direction d) const
{
    return Iterator (Node::GetFirstTraversal (fHead, t, OtherDir (d), nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
void    SortedBinaryTree<KEY,VALUE,TRAITS>::Update (const Iterator& it, const ValueType& newValue)
{
    Require (not it.Done ());
    const_cast<SortedBinaryTree<KEY, VALUE, TRAITS>::Node*> (it.GetNode ())->fEntry.SetValue (newValue);
}

template <typename KEY, typename VALUE, typename TRAITS>
void	SortedBinaryTree<KEY,VALUE,TRAITS>::Remove (const Iterator& it)
{
    Require (not it.Done ());
    RemoveNode (const_cast<SortedBinaryTree<KEY,VALUE,TRAITS>::Node*> (it.GetNode ()));
}


template <typename KEY, typename VALUE, typename TRAITS>
typename SortedBinaryTree<KEY,VALUE,TRAITS>::Node*	SortedBinaryTree<KEY,VALUE,TRAITS>::DuplicateBranch (Node* n)
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
void	SortedBinaryTree<KEY,VALUE,TRAITS>::Invariant () const
{
    #if qDebug
        Invariant_ ();
    #endif
}

#if qDebug

template <typename KEY, typename VALUE, typename TRAITS>
void	SortedBinaryTree<KEY,VALUE,TRAITS>::ValidateBranch (Node* n, size_t& count)
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
void	SortedBinaryTree<KEY,VALUE,TRAITS>::Invariant_ () const
{
	size_t	count = 0;

	if (fHead != nullptr) {
		ValidateBranch (fHead, count);
	}
	Assert (count == fLength);
}

template <typename KEY, typename VALUE, typename TRAITS>
void	SortedBinaryTree<KEY,VALUE,TRAITS>::ListAll (Iterator it) const
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
size_t	SortedBinaryTree<KEY,VALUE,TRAITS>::CalcHeight (size_t* totalHeight) const
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
void    SortedBinaryTreeValidationSuite (size_t testDataLength, bool verbose)
{
    TestTitle   tt ("SortedBinaryTree Validation", 0, verbose);

    RunSuite<SortedBinaryTree<KEYTYPE, size_t>, KEYTYPE> (testDataLength, verbose, 1);

    typedef SortedBinaryTree<int, int, ADT::Traits<
        KeyValue<int,int>,
        ADT::DefaultComp<int>,
        ADT::eDuplicateAddThrowException > >  NoDupAddTree;

    DuplicateAddBehaviorTest<SortedBinaryTree<int, int>, NoDupAddTree> (testDataLength, verbose, 1);

   typedef    SortedBinaryTree<int, int, ADT::Traits<
        KeyValue<int,int>,
        ADT::DefaultComp<int>,
        ADT::eInvalidRemoveIgnored > > InvalidRemoveIgnoredTree;
    InvalidRemoveBehaviorTest<SortedBinaryTree<int, int>, InvalidRemoveIgnoredTree> (verbose, 1);

    typedef    SortedBinaryTree<string, string, ADT::Traits<
        SharedStringKeyValue,
        CaseInsensitiveCompare,
        ADT::eDefaultPolicy > > SharedCaseInsensitiveString;
    StringTraitOverrideTest<SharedCaseInsensitiveString> (verbose, 1);

    typedef SortedBinaryTree<HashKey<string>, string>   HashedString;
    HashedStringTest<HashedString> (verbose, 1);


    SimpleIteratorTest< SortedBinaryTree<size_t, size_t> > (testDataLength, verbose, 1);
    PatchingIteratorTest<Tree_Patching<SortedBinaryTree<size_t, size_t> > > (testDataLength, verbose, 1);
}

#endif



// my old code for this, which works fine but takes N space rather than the log(N) space (from recursion) of the RebalanceBranch implementation
#if 0
template <typename KEY, typename VALUE, typename TRAITS>
void	SortedBinaryTree<KEY,VALUE,TRAITS>::Balance ()
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
			AssignNodeToArray (n->GetChild (Direction::kLeft));
            nodeList[curIndex++] = n;
			AssignNodeToArray (n->GetChild (Direction::kRight));
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
			n->SetChild (Direction::kLeft, nullptr);
			n->SetChild (Direction::kRight, nullptr);
			return n;
		}

		int curIdx = startIndex + (endIndex-startIndex)/2;
		Assert (curIdx <= endIndex);
		Assert (curIdx >= startIndex);

		Node* n = nodeList[curIdx];
		AssertNotNull (n);

        n->SetChild (Direction::kLeft, (curIdx == startIndex) ? nullptr : BalanceNode (startIndex, curIdx-1, curNodeHeight+1));
        n->SetChild (Direction::kRight, (curIdx == endIndex) ? nullptr : BalanceNode (curIdx+1, endIndex, curNodeHeight+1));

		return n;
	};
	if (fHead != nullptr) {
		fHead = BalanceNode (0, GetLength ()-1, 1);
		fHead->SetParent (nullptr);
	}

	delete[] nodeList;
}
#endif

    }   // namespace BinaryTree
}   // namespace ADT

