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
Treap<KEY,VALUE,TRAITS>::Treap () :
	fHead (nullptr),
	fLength (0)
#if qKeepADTStatistics
    ,fCompares (0)
    ,fRotations (0)
#endif
{
    Require (TRAITS::kPrioritizeOnFind == kNoPrioritizeFinds or TRAITS::kPrioritizeOnFind == kStandardPrioritizeFinds or TRAITS::kPrioritizeOnFind == kAlwaysPrioritizeOnFind);
}


template <typename KEY, typename VALUE, typename TRAITS>
Treap<KEY,VALUE,TRAITS>::Treap (const Treap& t) :
	fHead (nullptr),
	fLength (t.fLength)
  #if qKeepADTStatistics
      ,fCompares (t.fCompares)
      ,fRotations (t.fRotations)
  #endif
{
    Require (TRAITS::kPrioritizeOnFind == kNoPrioritizeFinds or TRAITS::kPrioritizeOnFind == kStandardPrioritizeFinds or TRAITS::kPrioritizeOnFind == kAlwaysPrioritizeOnFind);

	fHead = DuplicateBranch (t.fHead);
}

template <typename KEY, typename VALUE, typename TRAITS>
Treap<KEY,VALUE,TRAITS>::~Treap ()
{
	RemoveAll ();
}

template <typename KEY, typename VALUE, typename TRAITS>
Treap<KEY,VALUE,TRAITS>& Treap<KEY,VALUE,TRAITS>::operator= (const Treap& t)
{
	RemoveAll ();
	fHead = DuplicateBranch (t.fHead);
	fLength = t.fLength;
	return *this;
}

template <typename KEY, typename VALUE, typename TRAITS>
bool	Treap<KEY,VALUE,TRAITS>::Find (const KeyType& key, ValueType* val)  const
{
	int	comp;
	Node* n = FindNode (key, &comp);
	if (n != nullptr and (comp == 0)) {
		if (val != nullptr) {
			*val = n->fEntry.GetValue ();
		}

		if ((TRAITS::kPrioritizeOnFind == kAlwaysPrioritizeOnFind) or ((TRAITS::kPrioritizeOnFind == kStandardPrioritizeFinds) and (FlipCoin () == 1))) {
			// still only move if get higher priority
			size_t	newPriority = RandomSize_t ();
			if (newPriority > n->GetPriority ()) {
				n->SetPriority (newPriority);
				const_cast<Treap<KEY,VALUE,TRAITS> *> (this)->BubbleUp (n) ;
			}
		}
		return true;
	}
	return false;
}


template <typename KEY, typename VALUE, typename TRAITS>
typename Treap<KEY,VALUE,TRAITS>::Node* Treap<KEY,VALUE,TRAITS>::Rotate (Node* n, Direction rotateDir)
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
void Treap<KEY,VALUE,TRAITS>::BubbleUp (Node* n)
{
	RequireNotNull (n);

    #define	qUseSplayStyleZigZigWhenCan	0       // doesn't seem to improve things


	while (n->GetParent () != nullptr and (n->GetParent ()->GetPriority () < n->GetPriority ())) {
#if qUseSplayStyleZigZigWhenCan
		Node*	ancestor = n->GetParent ()->GetParent ();
		if (ancestor != nullptr and (ancestor->GetPriority () < n->GetPriority ())) {
			Node*	parent = n->GetParent ();
			if (Node::GetChildDir (n) == Node::GetChildDir (parent)) {
		//	if ((parent->GetChild (kLeft) == n and ancestor->GetChild (kLeft) == parent) or (parent->GetChild (kRight) == n and ancestor->GetChild (kRight) == parent)) {
				Node* uncle = ancestor->GetChild (Node::OtherDir (Node::GetChildDir (parent)));
				if (uncle != nullptr and parent->GetPriority () >= uncle->GetPriority ()) {
					// zig-zig
					SwapPriorities (parent, ancestor);
					Direction rotateDir = Node::OtherDir (Node::GetChildDir (n));
					Rotate (ancestor, rotateDir);
					Rotate (parent, rotateDir);
					continue;
				}
			}
			else {
				// zig-zag
 				Node* uncle = ancestor->GetChild (Node::OtherDir (Node::GetChildDir (parent)));
				if (uncle != nullptr and parent->GetPriority () >= uncle->GetPriority ()) {
				    SwapPriorities (parent, ancestor);
                    Rotate (parent, Node::OtherDir (Node::GetChildDir (n)));
                    Assert (ancestor->GetChild (kLeft) == n or ancestor->GetChild (kRight) == n);
                    Rotate (ancestor, Node::OtherDir (Node::GetChildDir (n)));
                    continue;
				}
			}
		}
#endif

		Assert (n->GetParent ()->GetChild (kLeft) == n or n->GetParent ()->GetChild (kRight) == n);
		Rotate (n->GetParent (), Node::OtherDir (Node::GetChildDir (n)));
	}
	Ensure ((n->GetParent () == nullptr) == (fHead == n));
	Ensure ((n->GetParent () == nullptr) or (n->GetParent ()->GetChild (kLeft) == n) or (n->GetParent ()->GetChild (kRight) == n));
}



template <typename KEY, typename VALUE, typename TRAITS>
void	Treap<KEY,VALUE,TRAITS>::Add (const KeyType& key, ValueType val)
{
	Node* n = new Node (key, val);
	n->SetPriority (RandomSize_t ());
	try {
        AddNode (n);
	}
	catch (const DuplicateAddException& exp) {
	    delete n;
	    throw;
    }
}

template <typename KEY, typename VALUE, typename TRAITS>
void	Treap<KEY,VALUE,TRAITS>::Add (const KeyType& keyAndValue)
{
    Add (keyAndValue, keyAndValue);
}



template <typename KEY, typename VALUE, typename TRAITS>
void	Treap<KEY,VALUE,TRAITS>::AddNode (Node* n)
{
	RequireNotNull (n);

	int	comp;
	Node* nearest =  FindNode (n->fEntry.GetKey (), &comp);
	if (nearest == nullptr) {
		Assert (fHead == nullptr);
		fHead = n;
	}
	else {
		n->SetParent (nearest);
		if (comp == 0) {
		    if (TRAITS::kPolicy & ADT::eDuplicateAddThrowException) {
		        throw DuplicateAddException ();
		    }
			n->SetChild (kLeft, nearest->GetChild (kLeft));
			nearest->SetChild (kLeft, n);
			ForceToBottom (n);
		}
		else if (comp < 0) {
			Assert (nearest->GetChild (kLeft) == nullptr);
			nearest->SetChild (kLeft, n);
		}
		else {
			Assert (nearest->GetChild (kRight) == nullptr);
			nearest->SetChild (kRight, n);
		}
		BubbleUp (n);
	}

	fLength++;
}


template <typename KEY, typename VALUE, typename TRAITS>
void Treap<KEY,VALUE,TRAITS>::ForceToBottom (Node* n)
{
	RequireNotNull (n);
	while (n->GetChild (kLeft) != nullptr or n->GetChild (kRight) != nullptr) {
	    Direction rotateDir = (n->GetChild (kLeft) == nullptr) or ((n->GetChild (kRight) != nullptr) and (n->GetChild (kLeft)->GetPriority () <= n->GetChild (kRight)->GetPriority ())) ? kLeft : kRight;
		Rotate (n, rotateDir);
    }

	Ensure (fHead->GetParent () == nullptr);
}

template <typename KEY, typename VALUE, typename TRAITS>
void Treap<KEY,VALUE,TRAITS>::Prioritize (Node* n)
{
    RequireNotNull (n);

    while (n->GetParent () != nullptr) {
        Rotate (n->GetParent (), Node::OtherDir (Node::GetChildDir (n)));
        SwapPrioritiesIfNeeded (n);
    }
    Assert (n->GetParent () == nullptr);
    fHead = n;
}

template <typename KEY, typename VALUE, typename TRAITS>
void Treap<KEY,VALUE,TRAITS>::SwapPrioritiesIfNeeded (Node* n)
{
    RequireNotNull (n);
    if (n->GetChild (kLeft) != nullptr and n->GetChild (kLeft)->GetPriority () > n->GetPriority ()) {
        Node::SwapPriorities (n, n->GetChild (kLeft));
        SwapPrioritiesIfNeeded (n->GetChild (kLeft));
    }
    if (n->GetChild (kRight) != nullptr and n->GetChild (kRight)->GetPriority () > n->GetPriority ()) {
        Node::SwapPriorities (n, n->GetChild (kRight));
        SwapPrioritiesIfNeeded (n->GetChild (kRight));
    }
}

template <typename KEY, typename VALUE, typename TRAITS>
void    Treap<KEY,VALUE,TRAITS>::Node::SwapPriorities (Node* n1, Node* n2)
{
    RequireNotNull (n1);
    RequireNotNull (n2);

    size_t  tmp = n1->GetPriority ();
    n1->SetPriority (n2->GetPriority ());
    n2->SetPriority (tmp);
}

template <typename KEY, typename VALUE, typename TRAITS>
void	Treap<KEY,VALUE,TRAITS>::Remove (const KeyType& key)
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
void	Treap<KEY,VALUE,TRAITS>::RemoveNode (Node* n)
{
	RequireNotNull (n);
	// we need to move it to the bottom of the tree, and only then remove it. Thus we keep
	// priorities in proper order
	ForceToBottom (n);
	if (n->GetParent () != nullptr) {
	    n->GetParent ()->SetChild (Node::GetChildDir (n), nullptr);
	}
	else {
		Assert (fHead == n);
		Assert (fLength == 1);
		fHead = nullptr;
	}
	delete n;
	--fLength;
}

template <typename KEY, typename VALUE, typename TRAITS>
void	Treap<KEY,VALUE,TRAITS>::RemoveAll ()
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
size_t	Treap<KEY,VALUE,TRAITS>::GetLength () const
{
	Assert ((fLength == 0) == (fHead == nullptr));
	return fLength;
}


template <typename KEY, typename VALUE, typename TRAITS>
typename Treap<KEY,VALUE,TRAITS>::Node*	Treap<KEY,VALUE,TRAITS>::FindNode (const KeyType& key, int* comparisonResult)  const
{
	RequireNotNull (comparisonResult);

	Node*	n = fHead;
	Node*	nearest = n;
	while (n != nullptr) {
		#if qKeepADTStatistics
			const_cast<Treap<KEY,VALUE,TRAITS> *> (this)->fCompares++;
		#endif
		nearest = n;
		*comparisonResult = TRAITS::Comparer::Compare (key, n->fEntry.GetKey ());
		if (*comparisonResult == 0) {
			return n;
		}
		n = (*comparisonResult < 0) ? n->GetChild (kLeft) : n->GetChild (kRight);
	}
	return nearest;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename Treap<KEY,VALUE,TRAITS>::Node*	Treap<KEY,VALUE,TRAITS>::GetFirst ()  const
{
    return Node::GetFirst (fHead);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename Treap<KEY,VALUE,TRAITS>::Node*	Treap<KEY,VALUE,TRAITS>::GetLast ()  const
{
    return Node::GetLast (fHead);
}


template <typename KEY, typename VALUE, typename TRAITS>
typename Treap<KEY,VALUE,TRAITS>::Node*	Treap<KEY,VALUE,TRAITS>::DuplicateBranch (Node* branchTop)
{
	Node* newNode = nullptr;
	if (branchTop != nullptr) {
		newNode = new Node (*branchTop);
		newNode->SetChild (kLeft, DuplicateBranch (branchTop->GetChild (kLeft)));
		newNode->SetChild (kRight, DuplicateBranch (branchTop->GetChild (kRight)));
	}
	return newNode;
}


template <typename KEY, typename VALUE, typename TRAITS>
void	Treap<KEY,VALUE,TRAITS>::ReBalance ()
{
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
	size_t	kMaxPriority = size_t (-1);
	size_t	maxHeight = size_t (log (double (GetLength ()))/log (2.0))+1;
	size_t	bucketSize = kMaxPriority/maxHeight;

	std::function<Node*(int startIndex, int endIndex, size_t curHeight)>	BalanceNode = [&BalanceNode, &nodeList, &bucketSize, &maxHeight] (int startIndex, int endIndex,  size_t curHeight) -> Node*
	{
		Require (startIndex <= endIndex);
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

		if (curIdx == startIndex) {
			n->SetChild (kLeft, nullptr);
		}
		else {
			n->SetChild (kLeft, BalanceNode (startIndex, curIdx-1, curHeight+1));

            size_t  priorLow =  bucketSize*(maxHeight - curHeight -1);
            size_t  priorHigh = bucketSize*(maxHeight - curHeight) -1;

			n->GetChild (kLeft)->SetPriority (RandomSize_t (priorLow,  priorHigh));

			Assert (n->GetChild (kLeft)->GetChild (kLeft) == nullptr or n->GetChild (kLeft)->GetPriority () >  n->GetChild (kLeft)->GetChild (kLeft)->GetPriority ());
			Assert (n->GetChild (kLeft)->GetChild (kRight) == nullptr or n->GetChild (kLeft)->GetPriority () > n->GetChild (kLeft)->GetChild (kRight)->GetPriority ());
		}
		if (curIdx == endIndex) {
			n->SetChild (kRight, nullptr);
		}
		else {
			n->SetChild (kRight, BalanceNode (curIdx+1, endIndex, curHeight+1));

            size_t  priorLow =  bucketSize*(maxHeight - curHeight -1);
            size_t  priorHigh = bucketSize*(maxHeight - curHeight) -1;

			n->GetChild (kRight)->SetPriority (RandomSize_t (priorLow,  priorHigh));

			Assert (n->GetChild (kRight)->GetChild (kLeft) == nullptr or n->GetChild (kRight)->GetPriority () >  n->GetChild (kRight)->GetChild (kLeft)->GetPriority ());
			Assert (n->GetChild (kRight)->GetChild (kRight) == nullptr or n->GetChild (kRight)->GetPriority () > n->GetChild (kRight)->GetChild (kRight)->GetPriority ());
		}

		return n;
	};
	if (fHead != nullptr) {
		fHead = BalanceNode (0, GetLength ()-1, 1);
		fHead->SetPriority (RandomSize_t (bucketSize*(maxHeight-1), bucketSize*maxHeight -1));
		Assert (fHead->GetChild (kLeft) == nullptr or fHead->GetPriority () > fHead->GetChild (kLeft)->GetPriority ());
		Assert (fHead->GetChild (kRight) == nullptr or fHead->GetPriority () > fHead->GetChild (kRight)->GetPriority ());
		fHead->SetParent (nullptr);
	}

	delete[] nodeList;
}

template <typename KEY, typename VALUE, typename TRAITS>
FindPrioritization	Treap<KEY,VALUE,TRAITS>::GetFindPrioritization ()
{
    Require (TRAITS::kOptimizeOnFindChance == kNoPrioritizeFinds or TRAITS::kOptimizeOnFindChance == kStandardPrioritizeFinds or TRAITS::kOptimizeOnFindChance == kAlwaysPrioritizeOnFind);
	return FindPrioritization (TRAITS::kOptimizeOnFindChance);
}

template <typename KEY, typename VALUE, typename TRAITS>
Treap<KEY,VALUE,TRAITS>::Node::Node (const KeyType& key, const ValueType& val)	:
	fEntry (key, val),
	fParent (nullptr),
	fPriority (0)
{
    SetChild (kLeft, nullptr);
    SetChild (kRight, nullptr);
}

template <typename KEY, typename VALUE, typename TRAITS>
Treap<KEY,VALUE,TRAITS>::Node::Node (const Node& n)	:
	fEntry (n.fEntry),
	fParent (nullptr),
	fPriority (n.fPriority)
{
    SetChild (kLeft, nullptr);
    SetChild (kRight, nullptr);
}



template <typename KEY, typename VALUE, typename TRAITS>
typename Treap<KEY,VALUE,TRAITS>::Node*   Treap<KEY,VALUE,TRAITS>::Node::GetParent () const
{
    return fParent;
}

template <typename KEY, typename VALUE, typename TRAITS>
void    Treap<KEY,VALUE,TRAITS>::Node::SetParent (Node* p)
{
    fParent = p;
}

 template <typename KEY, typename VALUE, typename TRAITS>
 typename Treap<KEY,VALUE,TRAITS>::Node::Node*   Treap<KEY,VALUE,TRAITS>::Node::GetChild (Direction direction)
 {
    Require (direction == kLeft or direction == kRight);
    return (fChildren[direction]);
}

template <typename KEY, typename VALUE, typename TRAITS>
 void   Treap<KEY,VALUE,TRAITS>::Node::SetChild (Direction direction, Node* n)
{
    Require (direction == kLeft or direction == kRight);
    fChildren[direction] = n;
    if (n != nullptr) {
        n->fParent = this;
    }
}
template <typename KEY, typename VALUE, typename TRAITS>
bool    Treap<KEY,VALUE,TRAITS>::Node::IsChild (Direction direction)
{
    Require (direction == kLeft or direction == kRight);
    return (fParent != nullptr and fParent->GetChild (direction) == this);
}

template <typename KEY, typename VALUE, typename TRAITS>
Direction     Treap<KEY,VALUE,TRAITS>::Node::GetChildDir (Node* n)
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
Direction    Treap<KEY,VALUE,TRAITS>::Node::OtherDir (Direction dir)
{
    Require (dir == kLeft or dir == kRight);
    return ((dir == kLeft) ? kRight : kLeft);
}

template <typename KEY, typename VALUE, typename TRAITS>
void    Treap<KEY,VALUE,TRAITS>::Node::SetChild_Safe (Node* parent, Node* n, Direction d)
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
typename Treap<KEY,VALUE,TRAITS>::Node*	Treap<KEY,VALUE,TRAITS>::Node::GetFirst (Node* n)
{
 	while (n != nullptr and n->GetChild (kLeft) != nullptr) {
		n = n->GetChild (kLeft);
	}
	return n;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename Treap<KEY,VALUE,TRAITS>::Node*	Treap<KEY,VALUE,TRAITS>::Node::GetLast (Node* n)
{
  	while (n != nullptr and n->GetChild (kRight) != nullptr) {
		n = n->GetChild (kRight);
	}
	return n;
}

template <typename KEY, typename VALUE, typename TRAITS>
size_t  Treap<KEY,VALUE,TRAITS>::Node::GetPriority () const
{
    return fPriority;
}

template <typename KEY, typename VALUE, typename TRAITS>
void    Treap<KEY,VALUE,TRAITS>::Node::SetPriority (size_t newP)
{
    fPriority = newP;
}

template <typename KEY, typename VALUE, typename TRAITS>
bool	Treap<KEY,VALUE,TRAITS>::FlipCoin ()
{
	static	size_t	sCounter = 0;
	return (++sCounter & 1);
}



#if qDebug

template <typename KEY, typename VALUE, typename TRAITS>
void	Treap<KEY,VALUE,TRAITS>::ValidateBranch (Node* n, size_t& count)
{
	RequireNotNull (n);
	++count;

	for (int i = kFirstChild; i <= kLastChild; ++i) {
	    Node* child = n->GetChild (Direction (i));
        if (child != nullptr) {
            int comp = TRAITS::Comparer::Compare (n->fEntry.GetKey (), child->fEntry.GetKey ());
            Assert ((comp == 0) or ((comp > 0) == (i == kLeft)));
            Assert (n->GetPriority () >= child->GetPriority ());
            Assert (child->GetParent () == n);
            ValidateBranch (child, count);
        }
    }
}

template <typename KEY, typename VALUE, typename TRAITS>
void	Treap<KEY,VALUE,TRAITS>::ValidateAll () const
{
	size_t	count = 0;

	if (fHead != nullptr) {
		ValidateBranch (fHead, count);
	}
	Assert (count == fLength);
}

template <typename KEY, typename VALUE, typename TRAITS>
void	Treap<KEY,VALUE,TRAITS>::ListAll () const
{
	std::function<void(Node*)>	ListNode = [&ListNode] (Node* n)
	{
	    if (n != nullptr) {
 			ListNode (n->GetChild (kLeft));
            std::cout << "(" << n->fEntry.GetKey () << "," << n->GetPriority () << ")";
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
size_t	Treap<KEY,VALUE,TRAITS>::CalcNodeHeight (Node* n, size_t height, size_t* totalHeight)
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
size_t	Treap<KEY,VALUE,TRAITS>::CalcHeight (size_t* totalHeight) const
{
	return CalcNodeHeight (fHead, 0, totalHeight);
}

#endif


#if qDebug

template <typename KEYTYPE>
void    TreapValidationSuite (size_t testDataLength, bool verbose)
{
    TestTitle   tt ("Treap Validation", 0, verbose);

    RunSuite<Treap<KEYTYPE, size_t>, KEYTYPE> (testDataLength, verbose, 1);
    typedef Treap<int, int, TreapTraits<
        KeyValue<int,int>,
        ADT::DefaultComp<int>,
        ADT::eDuplicateAddThrowException,
        kDefaultPrioritizeFinds> >  NoDupAddTree;
    DuplicateAddBehaviorTest<Treap<int, int>, NoDupAddTree> (testDataLength, verbose, 1);

   typedef    Treap<int, int, TreapTraits<
        KeyValue<int,int>,
        ADT::DefaultComp<int>,
        ADT::eInvalidRemoveIgnored,
        kDefaultPrioritizeFinds> > InvalidRemoveIgnoredTree;
    InvalidRemoveBehaviorTest<Treap<int, int>, InvalidRemoveIgnoredTree> (verbose, 1);

    typedef    Treap<string, string, TreapTraits<
        SharedStringKeyValue,
        CaseInsensitiveCompare,
        ADT::eDefaultPolicy,
        kDefaultPrioritizeFinds> > SharedCaseInsensitiveString;
    StringTraitOverrideTest<SharedCaseInsensitiveString> (verbose, 1);

    typedef Treap<HashKey<string>, string>   HashedString;
    HashedStringTest<HashedString> (verbose, 1);}

#endif

    }   // namespace BinaryTree
}   // namespace ADT
