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
AVLTree<KEY,VALUE,TRAITS>::AVLTree () :
	fHead (nullptr),
	fLength (0)
  #if qKeepADTStatistics
      ,fCompares (0)
      ,fRotations (0)
  #endif
{
}

template <typename KEY, typename VALUE, typename TRAITS>
AVLTree<KEY,VALUE,TRAITS>::AVLTree (const AVLTree& t) :
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
AVLTree<KEY,VALUE,TRAITS>& AVLTree<KEY,VALUE,TRAITS>::operator= (const AVLTree& t)
{
	RemoveAll ();
	fLength = t.fLength;
	fHead = DuplicateBranch (t.fHead);

	return *this;
}


template <typename KEY, typename VALUE, typename TRAITS>
AVLTree<KEY,VALUE,TRAITS>::~AVLTree ()
{
	RemoveAll ();
}

template <typename KEY, typename VALUE, typename TRAITS>
bool	AVLTree<KEY,VALUE,TRAITS>::Find (const KeyType& key, ValueType* val) const
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
typename AVLTree<KEY,VALUE,TRAITS>::Node* AVLTree<KEY,VALUE,TRAITS>::Rotate (Node* n, Direction rotateDir)
{
	RequireNotNull (n);
    Require (rotateDir == kLeft or rotateDir == kRight);

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
typename AVLTree<KEY,VALUE,TRAITS>::Node*  AVLTree<KEY,VALUE,TRAITS>::DoubleRotate (Node* n, Direction rotateDir)
{
	RequireNotNull (n);
    Require (rotateDir == kLeft or rotateDir == kRight);

    Direction   otherDir = Node::OtherDir (rotateDir);

    Node*   child = n->GetChild (rotateDir);
    AssertNotNull (child);
    Node*   grandkid = child->GetChild (otherDir);
    AssertNotNull (grandkid);

    Rotate (child, rotateDir);
    Node* newTop = Rotate (n, otherDir);

    Assert (newTop == grandkid);

    if (grandkid->GetBalance () == 0) {
        n->SetBalance (0);
        child->SetBalance (0);
    }
    else if ((grandkid->GetBalance () == -1) == (rotateDir == kLeft)) {
        n->SetBalance ((grandkid->GetBalance () == -1) ? 1 : -1);
        child->SetBalance (0);
    }
    else {
        n->SetBalance (0);
        child->SetBalance ((grandkid->GetBalance () == -1) ? 1 : -1);
    }
    grandkid->SetBalance (0);

    return newTop;
}

template <typename KEY, typename VALUE, typename TRAITS>
void	AVLTree<KEY,VALUE,TRAITS>::Add (const KeyType& key, const ValueType& val)
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
void	AVLTree<KEY,VALUE,TRAITS>::Add (const KeyType& keyAndValue)
{
    Add (keyAndValue, keyAndValue);
}


template <typename KEY, typename VALUE, typename TRAITS>
void	AVLTree<KEY,VALUE,TRAITS>::AddNode (Node* n)
{
	RequireNotNull (n);


    bool    allowDuplicateAdds = not (TRAITS::kPolicy & ADT::eDuplicateAddThrowException);
	int	comp;
	Node* nearest =  FindNode (n->fEntry.GetKey (), &comp, allowDuplicateAdds);
	if (nearest == nullptr) {
		Assert (fHead == nullptr);
		fHead = n;
	}
	else {
	    Direction childDir = kBadDir;
		if (comp == 0) {
            if (TRAITS::kPolicy & ADT::eDuplicateAddThrowException) {
                throw DuplicateAddException ();
            }
            childDir = kLeft;
		}
		else if (comp < 0) {
            childDir = kLeft;
		}
		else {
            childDir = kRight;
		}

		Assert (childDir != kBadDir);
		Assert (nearest->GetChild (childDir) == nullptr);
		nearest->SetChild (childDir, n);

		Node*   current = nearest;
		while (current != nullptr) {
		    if (current->GetBalance () == 0) {
		        current->SetBalance (current->GetBalance () + (childDir == kLeft) ? -1 : 1);
		        childDir = Node::GetChildDir (current);
		        current = current->GetParent ();
		    }
		    else {
		        bool    doubleLean = ((current->GetBalance () < 0) == (childDir == kLeft));
		        if (doubleLean) {
                    RebalanceNode (current, childDir, 1);
		            break;
		        }
		        else {
		            current->SetBalance (0);
		            break;  // our height was not altered, so no node above us can be affected
		        }
		    }
		}
	}

	fLength++;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename AVLTree<KEY,VALUE,TRAITS>::Node*    AVLTree<KEY,VALUE,TRAITS>::RebalanceNode (Node* n, Direction leanDir, int mod)
{
    RequireNotNull (n);

    Node*   child = n->GetChild (leanDir);
    AssertNotNull (child);

    if (mod == 1) {
        Assert (child->GetBalance () == -1 or child->GetBalance () == 1);
        if ((child->GetBalance () == -1) == (leanDir == kLeft)) {
            Rotate (n, Node::OtherDir (leanDir));
            n->SetBalance (0);
            child->SetBalance (0);
        }
        else {
            DoubleRotate (n, leanDir);
        }
    }
    else {
        if (child->GetBalance () == 0) {
             Node* newTop = Rotate (n, Node::OtherDir (leanDir));
             newTop->SetBalance (newTop->GetBalance ());
             Assert (child == newTop);
             child->SetBalance ((leanDir == kLeft) ? 1 : -1);
        }
        else if (n->GetBalance () == child->GetBalance ()) {
           Node* newTop = Rotate (n, Node::OtherDir (leanDir));
           n->SetBalance (0);
           child->SetBalance (0);
           Assert (child == newTop);
           return newTop;
        }
        else {
            Node* newTop = DoubleRotate (n, leanDir);
            Assert (newTop->GetBalance () == 0);
            return newTop;
         }
    }
    return nullptr;
}

template <typename KEY, typename VALUE, typename TRAITS>
void	AVLTree<KEY,VALUE,TRAITS>::Remove (const KeyType& key)
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
void	AVLTree<KEY,VALUE,TRAITS>::SwapNodes (Node* parent, Node* child)
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
void	AVLTree<KEY,VALUE,TRAITS>::RemoveNode (Node* n)
{
	RequireNotNull (n);

    Node*   parent = n->GetParent ();
    Direction   childDir = Node::GetChildDir (n);

	if (n->GetChild (kLeft) == nullptr and n->GetChild (kRight) == nullptr) {
	    SwapNodes (n, nullptr);
	}
	else if (n->GetChild (kLeft) == nullptr) {
		SwapNodes (n, n->GetChild (kRight));
		if (parent == nullptr) {
            fHead->SetBalance (n->GetBalance () -1);
		}
	}
	else if (n->GetChild (kRight) == nullptr) {
		SwapNodes (n, n->GetChild (kLeft));
		if (parent == nullptr) {
            fHead->SetBalance (n->GetBalance () +1);
		}
	}
	else {
        Direction d = (FlipCoin ()) ? kRight : kLeft;
        Node* minNode = (d == kRight)
            ? Node::GetFirst (n->GetChild (d))
            : Node::GetLast (n->GetChild (d));

		AssertNotNull (minNode);
		if (minNode->GetParent () != n) {
            parent = minNode->GetParent ();
            childDir = Node::GetChildDir (minNode);
			SwapNodes (minNode, minNode->GetChild (d));
			minNode->SetChild (d, n->GetChild (d));
			minNode->SetBalance (n->GetBalance ());
		}
		else {
		    parent = minNode;
            minNode->SetBalance (n->GetBalance ());
            childDir = Node::GetChildDir (minNode);
		}

		SwapNodes (n, minNode);
		minNode->SetChild (Node::OtherDir (d), n->GetChild (Node::OtherDir (d)));
	}

	if (childDir != kBadDir) {
		Node*   current = parent;
		while (current != nullptr) {
		    if (current->GetBalance () == 0) {
		        current->SetBalance ((childDir == kLeft) ? 1 : -1);
		        break;
		    }
		    else {
		        bool    doubleLean = ((current->GetBalance () > 0) == (childDir == kLeft));
		        if (doubleLean) {
                    current = RebalanceNode (current, Node::OtherDir (childDir), -1);
                    if (current != nullptr) {
                       childDir = Node::GetChildDir (current);
                       current = current->GetParent ();
                    }
		        }
		        else {
		            current->SetBalance (0);
                    childDir = Node::GetChildDir (current);
                    current = current->GetParent ();
		        }
		    }
		}
	}

	delete n;
	--fLength;
}


template <typename KEY, typename VALUE, typename TRAITS>
void	AVLTree<KEY,VALUE,TRAITS>::RemoveAll ()
{
	// iterate rather than natural tail recursive version
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
size_t	AVLTree<KEY,VALUE,TRAITS>::GetLength () const
{
	Assert ((fLength == 0) == (fHead == nullptr));
	return fLength;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename AVLTree<KEY,VALUE,TRAITS>::Node*	AVLTree<KEY,VALUE,TRAITS>::FindNode (const KeyType& key, int* comparisonResult, bool ignoreMatch)  const
{
	RequireNotNull (comparisonResult);

	Node*	n = fHead;
	Node*	nearest = n;
	*comparisonResult = 0;
	while (n != nullptr) {
		#if qKeepADTStatistics
			const_cast<AVLTree<KEY,VALUE,TRAITS> *> (this)->fCompares++;
		#endif

		nearest = n;
		*comparisonResult = TRAITS::Comparer::Compare (key, n->fEntry.GetKey ());
		if (*comparisonResult == 0 and (not ignoreMatch)) {
			return n;
		}
		n = (*comparisonResult <= 0) ? n->GetChild (kLeft) : n->GetChild (kRight);
	}
	return nearest;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename	AVLTree<KEY,VALUE,TRAITS>::Node*	AVLTree<KEY,VALUE,TRAITS>::GetFirst () const
{
    return Node::GetFirst (fHead);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename	AVLTree<KEY,VALUE,TRAITS>::Node*	AVLTree<KEY,VALUE,TRAITS>::GetLast () const
{
    return Node::GetLast (fHead);
}


template <typename KEY, typename VALUE, typename TRAITS>
typename AVLTree<KEY,VALUE,TRAITS>::Node*	AVLTree<KEY,VALUE,TRAITS>::DuplicateBranch (Node* n)
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
			newNode->SetBalance (branchTop->GetBalance ());

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
void	AVLTree<KEY,VALUE,TRAITS>::ReBalance ()
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

	std::function<Node*(int startIndex, int endIndex, int& curNodeHeight)>	BalanceNode = [&BalanceNode, &nodeList] (int startIndex, int endIndex, int& curNodeHeight) -> Node*
	{
		Require (startIndex <= endIndex);

		if (startIndex == endIndex) {
			Node* n = nodeList[startIndex];
			n->SetChild (kLeft, nullptr);
			n->SetChild (kRight, nullptr);
            n->SetBalance (0);
			curNodeHeight = 1;
			return n;
		}

		int curIdx = startIndex + (endIndex-startIndex)/2;
		Assert (curIdx <= endIndex);
		Assert (curIdx >= startIndex);

		Node* n = nodeList[curIdx];
		AssertNotNull (n);

        int lht = 0;
        int rht = 0;
        n->SetChild (kLeft, (curIdx == startIndex) ? nullptr : BalanceNode (startIndex, curIdx-1, lht));
        n->SetChild (kRight, (curIdx == endIndex) ? nullptr : BalanceNode (curIdx+1, endIndex, rht));

        curNodeHeight = std::max (lht, rht) + 1;
        n->SetBalance (rht-lht);

		return n;
	};
	if (fHead != nullptr) {
	    int ignored;
		fHead = BalanceNode (0, GetLength ()-1, ignored);
		fHead->SetParent (nullptr);
	}

	delete[] nodeList;
}

template <typename KEY, typename VALUE, typename TRAITS>
AVLTree<KEY,VALUE,TRAITS>::Node::Node (const KeyType& key, const ValueType& val)	:
	fEntry (key, val),
	fParent (nullptr),
	fBalance (0)
{
    fChildren[kLeft] = nullptr;
    fChildren[kRight] = nullptr;
}

template <typename KEY, typename VALUE, typename TRAITS>
AVLTree<KEY,VALUE,TRAITS>::Node::Node (const Node& n)	:
	fEntry (n.fEntry),
	fParent (nullptr),
	fBalance (0)
{
    fChildren[kLeft] = nullptr;
    fChildren[kRight] = nullptr;
}

template <typename KEY, typename VALUE, typename TRAITS>
short   AVLTree<KEY,VALUE,TRAITS>::Node::GetBalance () const
{
    return fBalance;
}

template <typename KEY, typename VALUE, typename TRAITS>
void    AVLTree<KEY,VALUE,TRAITS>::Node::SetBalance (short b)
{
    Require (b >= -1 and b <= 1);
    fBalance = b;
}

template <typename KEY, typename VALUE, typename TRAITS>
bool    AVLTree<KEY,VALUE,TRAITS>::Node::IsUnbalanced () const
{
   return (fBalance < -1 or fBalance > 1);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename AVLTree<KEY,VALUE,TRAITS>::Node*   AVLTree<KEY,VALUE,TRAITS>::Node::GetParent () const
{
    return fParent;
}

template <typename KEY, typename VALUE, typename TRAITS>
void    AVLTree<KEY,VALUE,TRAITS>::Node::SetParent (Node* p)
{
    fParent = p;
}

 template <typename KEY, typename VALUE, typename TRAITS>
 typename AVLTree<KEY,VALUE,TRAITS>::Node::Node*   AVLTree<KEY,VALUE,TRAITS>::Node::GetChild (Direction direction)
 {
    Require (direction == kLeft or direction == kRight);
    return (fChildren[direction]);
}

template <typename KEY, typename VALUE, typename TRAITS>
 void   AVLTree<KEY,VALUE,TRAITS>::Node::SetChild (Direction direction, Node* n)
{
    Require (direction == kLeft or direction == kRight);
    fChildren[direction] = n;
    if (n != nullptr) {
        n->fParent = this;
    }
}
template <typename KEY, typename VALUE, typename TRAITS>
bool    AVLTree<KEY,VALUE,TRAITS>::Node::IsChild (Direction direction)
{
    Require (direction == kLeft or direction == kRight);
    return (fParent != nullptr and fParent->GetChild (direction) == this);
}

template <typename KEY, typename VALUE, typename TRAITS>
Direction     AVLTree<KEY,VALUE,TRAITS>::Node::GetChildDir (Node* n)
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
Direction    AVLTree<KEY,VALUE,TRAITS>::Node::OtherDir (Direction dir)
{
    Require (dir == kLeft or dir == kRight);
    return ((dir == kLeft) ? kRight : kLeft);
}

template <typename KEY, typename VALUE, typename TRAITS>
void    AVLTree<KEY,VALUE,TRAITS>::Node::SetChild_Safe (Node* parent, Node* n, Direction d)
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
typename AVLTree<KEY,VALUE,TRAITS>::Node*	AVLTree<KEY,VALUE,TRAITS>::Node::GetFirst (Node* n)
{
 	while (n != nullptr and n->GetChild (kLeft) != nullptr) {
		n = n->GetChild (kLeft);
	}
	return n;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename AVLTree<KEY,VALUE,TRAITS>::Node*	AVLTree<KEY,VALUE,TRAITS>::Node::GetLast (Node* n)
{
  	while (n != nullptr and n->GetChild (kRight) != nullptr) {
		n = n->GetChild (kRight);
	}
	return n;
}

#if qDebug

template <typename KEY, typename VALUE, typename TRAITS>
int	AVLTree<KEY,VALUE,TRAITS>::ValidateBranch (Node* n, size_t& count)
{
	RequireNotNull (n);
	++count;
	int lHeight = 0;
	int rHeight = 0;
	if (n->GetChild (kLeft) != nullptr) {
		Assert (TRAITS::Comparer::Compare (n->fEntry.GetKey (), n->GetChild (kLeft)->fEntry.GetKey ()) >= 0);
		Assert (n->GetChild (kLeft)->GetParent () == n);
		lHeight = ValidateBranch (n->GetChild (kLeft), count);
	}
	if (n->GetChild (kRight) != nullptr) {
		// we cannot do strict < 0, because rotations can put on either side
		Assert (TRAITS::Comparer::Compare (n->fEntry.GetKey (), n->GetChild (kRight)->fEntry.GetKey ()) <= 0);
		Assert (n->GetChild (kRight)->GetParent () == n);
		rHeight = ValidateBranch (n->GetChild (kRight), count);
	}

	Assert (std::abs (rHeight-lHeight) <= 1);
if ((rHeight-lHeight) != n->GetBalance ()) {
std::cout << "for " << n->fEntry.GetValue () << "; balance = " << n->GetBalance () << "; lheight = " << lHeight << "; rHeight = " << rHeight << std::endl;
}

	Assert ((rHeight-lHeight) == n->GetBalance ());

	return 1 + std::max (lHeight, rHeight);
}

template <typename KEY, typename VALUE, typename TRAITS>
void	AVLTree<KEY,VALUE,TRAITS>::ValidateAll () const
{
	size_t	count = 0;

	if (fHead != nullptr) {
		ValidateBranch (fHead, count);
	}
	Assert (count == fLength);
}

template <typename KEY, typename VALUE, typename TRAITS>
void	AVLTree<KEY,VALUE,TRAITS>::ListAll () const
{
	std::function<void(Node*)>	ListNode = [&ListNode] (Node* n)
	{
		if (n->GetChild (kLeft) != nullptr) {
			ListNode (n->GetChild (kLeft));
		}
        int cCount = 0;
        if (n->GetChild (kLeft) != nullptr) cCount++;
        if (n->GetChild (kRight) != nullptr) cCount++;
        std::cout << "(" << n->fEntry.GetKey () << ", " << cCount << ", " << n->GetBalance () << ")";
		if (n->GetChild (kRight) != nullptr) {
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
size_t	AVLTree<KEY,VALUE,TRAITS>::CalcHeight (size_t* totalHeight) const
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
void    AVLTreeValidationSuite (size_t testDataLength, bool verbose)
{
    TestTitle   tt ("AVLTree Validation", 0, verbose);

    RunSuite<AVLTree<KEYTYPE, size_t>, KEYTYPE> (testDataLength, verbose, 1);

    typedef AVLTree<int, int, ADT::Traits<
        KeyValue<int,int>,
        ADT::DefaultComp<int>,
        ADT::eDuplicateAddThrowException > >  NoDupAddTree;

    DuplicateAddBehaviorTest<AVLTree<int, int>, NoDupAddTree> (testDataLength, verbose, 1);

   typedef    AVLTree<int, int, ADT::Traits<
        KeyValue<int,int>,
        ADT::DefaultComp<int>,
        ADT::eInvalidRemoveIgnored > > InvalidRemoveIgnoredTree;
    InvalidRemoveBehaviorTest<AVLTree<int, int>, InvalidRemoveIgnoredTree> (verbose, 1);

    typedef    AVLTree<string, string, ADT::Traits<
        SharedStringKeyValue,
        CaseInsensitiveCompare,
        ADT::eDefaultPolicy > > SharedCaseInsensitiveString;
    StringTraitOverrideTest<SharedCaseInsensitiveString> (verbose, 1);

    typedef AVLTree<HashKey<string>, string>   HashedString;
   HashedStringTest<HashedString> (verbose, 1);
}

#endif


    }   // namespace BinaryTree
}   // namespace ADT
