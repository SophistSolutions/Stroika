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


template  <typename KEYVALUE>
 class	TreapNode : public TreeNode<KEYVALUE> {
    public:
        typedef KEYVALUE    KeyValue;
        typedef typename KEYVALUE::KeyType  KeyType;
        typedef typename KEYVALUE::ValueType  ValueType;
        typedef TreeNode<KEYVALUE>  inherited;

    public:
        TreapNode (const KeyType& key, const ValueType& val) :
            inherited (key, val)
        {
        }

        TreapNode (const TreapNode& n) :
            inherited (n),
            fPriority (n.fPriority)
        {
        }

        DECLARE_USE_BLOCK_ALLOCATION(TreapNode);

        // dumb shadowing
        nonvirtual  TreapNode*   GetParent () const
        {
            return static_cast<TreapNode*> (inherited::GetParent ());
        }

        TreapNode*   GetChild (Direction direction)
        {
              return static_cast<TreapNode*> (inherited::GetChild (direction));
        }

        static  TreapNode*   GetFirst (TreapNode* n)
        {
            return static_cast<TreapNode*> (inherited::GetFirst (n));
        }

        static  TreapNode*   GetLast (TreapNode* n)
        {
            return static_cast<TreapNode*> (inherited::GetLast (n));
        }

        nonvirtual  TreapNode*   GetSibling () const
        {
             return static_cast<TreapNode*> (inherited::GetSibling ());
        }

        nonvirtual  TreapNode*   Traverse (TraversalType t, Direction d) const
        {
              return static_cast<TreapNode*> (inherited::Traverse (t, d));
        }

        static      TreapNode*   GetFirstTraversal (const TreapNode* n, TraversalType t, Direction d, size_t* height)
        {
               return static_cast<TreapNode*> (inherited::GetFirstTraversal (n, t, d, height));
       }

        static      TreapNode*   Descend (TreapNode* n, int height, Direction d)
        {
            return static_cast<TreapNode*> (inherited::Descend (n, height, d));
        }

        static  void    Apply (const TreapNode* node, TraversalType t, Direction d,  const  std::function<void(const TreapNode& n)>& func)
        {
            inherited::Apply (node, t, d, [func] (const TreeNode<KEYVALUE>& n) { func (static_cast<const TreapNode&> (n)); });
        }

        static  const   TreapNode*   Find (const TreapNode* node, TraversalType t, Direction d,  const std::function<bool(const TreapNode& n)>& func)
        {
            return static_cast<const TreapNode*> (inherited::Find (node, t, d, [func] (const TreeNode<KEYVALUE>& n)->bool  {  return func (static_cast<const TreapNode&> (n)); }));
        }

        void    Inspect () const
        {
            std::cout << "(" << this->fEntry.GetKey () << "," << this->fEntry.GetValue () << "," << this->GetPriority () << ")";
        }

        // treap additions
        nonvirtual  size_t  GetPriority () const
        {
            return fPriority;
        }

        nonvirtual  void    SetPriority (size_t newP)
        {
            fPriority = newP;
        }

        static  void    SwapPriorities (TreapNode* n1, TreapNode* n2)
        {
            RequireNotNull (n1);
            RequireNotNull (n2);

            size_t  tmp = n1->GetPriority ();
            n1->SetPriority (n2->GetPriority ());
            n2->SetPriority (tmp);
       }

   private:
        size_t	fPriority;
};

#if 0
template  <typename KEYVALUE>
void        TreapNode<KEYVALUE>::Apply (const TreapNode* node, TraversalType t, Direction d,  const  std::function<void(const TreapNode& n)>& func)
{
    inherited::Apply (node, t, d, [func] (const TreeNode<KEYVALUE>& n) { func (static_cast<const TreapNode&> (n)); });
}


template  <typename KEYVALUE>
const TreapNode<KEYVALUE>*   TreapNode<KEYVALUE>::Find (const TreapNode* node, TraversalType t, Direction d,  const std::function<bool(const TreapNode& n)>& func)
{
    return inherited::Find (node, t, d, [func] (const TreeNode<KEYVALUE>& n)->bool { return func (static_cast<const TreapNode&> (n)); });
}
#endif


template <typename KEY, typename VALUE, typename TRAITS>
Treap<KEY,VALUE,TRAITS>::Treap () :
	fHead (nullptr),
	fLength (0)
#if qKeepADTStatistics
    ,fCompares (0)
    ,fRotations (0)
#endif
{
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

		if ((TRAITS::kPrioritizeOnFind == FindPrioritization::kAlways) or ((TRAITS::kPrioritizeOnFind == FindPrioritization::kStandard) and (FlipCoin () == 1))) {
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
		//	if ((parent->GetChild (Direction::kLeft) == n and ancestor->GetChild (Direction::kLeft) == parent) or (parent->GetChild (Direction::kRight) == n and ancestor->GetChild (Direction::kRight) == parent)) {
				Node* uncle = ancestor->GetChild (OtherDir (Node::GetChildDir (parent)));
				if (uncle != nullptr and parent->GetPriority () >= uncle->GetPriority ()) {
					// zig-zig
					SwapPriorities (parent, ancestor);
					Direction rotateDir = OtherDir (Node::GetChildDir (n));
					Rotate (ancestor, rotateDir);
					Rotate (parent, rotateDir);
					continue;
				}
			}
			else {
				// zig-zag
 				Node* uncle = ancestor->GetChild (OtherDir (Node::GetChildDir (parent)));
				if (uncle != nullptr and parent->GetPriority () >= uncle->GetPriority ()) {
				    SwapPriorities (parent, ancestor);
                    Rotate (parent, OtherDir (Node::GetChildDir (n)));
                    Assert (ancestor->GetChild (Direction::kLeft) == n or ancestor->GetChild (Direction::kRight) == n);
                    Rotate (ancestor, OtherDir (Node::GetChildDir (n)));
                    continue;
				}
			}
		}
#endif

		Assert (n->GetParent ()->GetChild (Direction::kLeft) == n or n->GetParent ()->GetChild (Direction::kRight) == n);
		Rotate (n->GetParent (), OtherDir (Node::GetChildDir (n)));
	}
	Ensure ((n->GetParent () == nullptr) == (fHead == n));
	Ensure ((n->GetParent () == nullptr) or (n->GetParent ()->GetChild (Direction::kLeft) == n) or (n->GetParent ()->GetChild (Direction::kRight) == n));
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
			n->SetChild (Direction::kLeft, nearest->GetChild (Direction::kLeft));
			nearest->SetChild (Direction::kLeft, n);
			ForceToBottom (n);
		}
		else if (comp < 0) {
			Assert (nearest->GetChild (Direction::kLeft) == nullptr);
			nearest->SetChild (Direction::kLeft, n);
		}
		else {
			Assert (nearest->GetChild (Direction::kRight) == nullptr);
			nearest->SetChild (Direction::kRight, n);
		}
		BubbleUp (n);
	}

	fLength++;
}


template <typename KEY, typename VALUE, typename TRAITS>
void Treap<KEY,VALUE,TRAITS>::ForceToBottom (Node* n)
{
	RequireNotNull (n);
	while (n->GetChild (Direction::kLeft) != nullptr or n->GetChild (Direction::kRight) != nullptr) {
	    Direction rotateDir = (n->GetChild (Direction::kLeft) == nullptr) or ((n->GetChild (Direction::kRight) != nullptr) and (n->GetChild (Direction::kLeft)->GetPriority () <= n->GetChild (Direction::kRight)->GetPriority ())) ? Direction::kLeft : Direction::kRight;
		Rotate (n, rotateDir);
    }

	Ensure (fHead->GetParent () == nullptr);
}

template <typename KEY, typename VALUE, typename TRAITS>
void Treap<KEY,VALUE,TRAITS>::Prioritize (const Iterator& it)
{
    Require (not it.Done ());
    Node*   n = const_cast<Node*> (it.GetNode ());

    RequireNotNull (n);

    while (n->GetParent () != nullptr) {
        Rotate (n->GetParent (), OtherDir (Node::GetChildDir (n)));
        SwapPrioritiesIfNeeded (n);
    }
    Assert (n->GetParent () == nullptr);
    fHead = n;
}

template <typename KEY, typename VALUE, typename TRAITS>
void Treap<KEY,VALUE,TRAITS>::SwapPrioritiesIfNeeded (Node* n)
{
    RequireNotNull (n);
    if (n->GetChild (Direction::kLeft) != nullptr and n->GetChild (Direction::kLeft)->GetPriority () > n->GetPriority ()) {
        Node::SwapPriorities (n, n->GetChild (Direction::kLeft));
        SwapPrioritiesIfNeeded (n->GetChild (Direction::kLeft));
    }
    if (n->GetChild (Direction::kRight) != nullptr and n->GetChild (Direction::kRight)->GetPriority () > n->GetPriority ()) {
        Node::SwapPriorities (n, n->GetChild (Direction::kRight));
        SwapPrioritiesIfNeeded (n->GetChild (Direction::kRight));
    }
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
			DeleteANode (n->GetChild (Direction::kLeft));
			DeleteANode (n->GetChild (Direction::kRight));
			delete n;
		}
	};

	DeleteANode (fHead);

	fHead = nullptr;
	fLength = 0;
}



template <typename KEY, typename VALUE, typename TRAITS>
typename Treap<KEY,VALUE,TRAITS>::Iterator Treap<KEY,VALUE,TRAITS>::Iterate (TraversalType t, Direction d) const
{
    return Iterator (Node::GetFirstTraversal (fHead, t, d, nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename Treap<KEY,VALUE,TRAITS>::Iterator Treap<KEY,VALUE,TRAITS>::Iterate (const KeyType& key, TraversalType t, Direction d) const
{
    int	comp;
    return Iterator (Node::GetFirstTraversal (FindNode (key, &comp), t, d, nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename Treap<KEY,VALUE,TRAITS>::Iterator  Treap<KEY,VALUE,TRAITS>::begin (TraversalType t, Direction d) const
{
    return Iterator (Node::GetFirstTraversal (fHead, t, d, nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename    Treap<KEY,VALUE,TRAITS>::Iterator  Treap<KEY,VALUE,TRAITS>::end () const
{
    return Iterator (nullptr);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename    Treap<KEY,VALUE,TRAITS>::Iterator	Treap<KEY,VALUE,TRAITS>::GetFirst (TraversalType t, Direction d) const
{
    return Iterator (Node::GetFirstTraversal (fHead, t, d, nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename    Treap<KEY,VALUE,TRAITS>::Iterator	Treap<KEY,VALUE,TRAITS>::GetLast (TraversalType t, Direction d) const
{
    return Iterator (Node::GetFirstTraversal (fHead, t, OtherDir (d), nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
void    Treap<KEY,VALUE,TRAITS>::Update (const Iterator& it, const ValueType& newValue)
{
    Require (not it.Done ());
    const_cast<Treap<KEY, VALUE, TRAITS>::Node*> (it.GetNode ())->fEntry.SetValue (newValue);
}

template <typename KEY, typename VALUE, typename TRAITS>
void	Treap<KEY,VALUE,TRAITS>::Remove (const Iterator& it)
{
    Require (not it.Done ());
    RemoveNode (const_cast<Treap<KEY,VALUE,TRAITS>::Node*> (it.GetNode ()));
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
		n = (*comparisonResult < 0) ? n->GetChild (Direction::kLeft) : n->GetChild (Direction::kRight);
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
		newNode->SetChild (Direction::kLeft, DuplicateBranch (branchTop->GetChild (Direction::kLeft)));
		newNode->SetChild (Direction::kRight, DuplicateBranch (branchTop->GetChild (Direction::kRight)));
	}
	return newNode;
}


template <typename KEY, typename VALUE, typename TRAITS>
void	Treap<KEY,VALUE,TRAITS>::Balance ()
{
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
	size_t	kMaxPriority = size_t (-1);
	size_t	maxHeight = size_t (log (double (GetLength ()))/log (2.0))+1;
	size_t	bucketSize = kMaxPriority/maxHeight;

	std::function<Node*(int startIndex, int endIndex, size_t curHeight)>	BalanceNode = [&BalanceNode, &nodeList, &bucketSize, &maxHeight] (int startIndex, int endIndex,  size_t curHeight) -> Node*
	{
		Require (startIndex <= endIndex);
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

		if (curIdx == startIndex) {
			n->SetChild (Direction::kLeft, nullptr);
		}
		else {
			n->SetChild (Direction::kLeft, BalanceNode (startIndex, curIdx-1, curHeight+1));

            size_t  priorLow =  bucketSize*(maxHeight - curHeight -1);
            size_t  priorHigh = bucketSize*(maxHeight - curHeight) -1;

			n->GetChild (Direction::kLeft)->SetPriority (RandomSize_t (priorLow,  priorHigh));

			Assert (n->GetChild (Direction::kLeft)->GetChild (Direction::kLeft) == nullptr or n->GetChild (Direction::kLeft)->GetPriority () >  n->GetChild (Direction::kLeft)->GetChild (Direction::kLeft)->GetPriority ());
			Assert (n->GetChild (Direction::kLeft)->GetChild (Direction::kRight) == nullptr or n->GetChild (Direction::kLeft)->GetPriority () > n->GetChild (Direction::kLeft)->GetChild (Direction::kRight)->GetPriority ());
		}
		if (curIdx == endIndex) {
			n->SetChild (Direction::kRight, nullptr);
		}
		else {
			n->SetChild (Direction::kRight, BalanceNode (curIdx+1, endIndex, curHeight+1));

            size_t  priorLow =  bucketSize*(maxHeight - curHeight -1);
            size_t  priorHigh = bucketSize*(maxHeight - curHeight) -1;

			n->GetChild (Direction::kRight)->SetPriority (RandomSize_t (priorLow,  priorHigh));

			Assert (n->GetChild (Direction::kRight)->GetChild (Direction::kLeft) == nullptr or n->GetChild (Direction::kRight)->GetPriority () >  n->GetChild (Direction::kRight)->GetChild (Direction::kLeft)->GetPriority ());
			Assert (n->GetChild (Direction::kRight)->GetChild (Direction::kRight) == nullptr or n->GetChild (Direction::kRight)->GetPriority () > n->GetChild (Direction::kRight)->GetChild (Direction::kRight)->GetPriority ());
		}

		return n;
	};
	if (fHead != nullptr) {
		fHead = BalanceNode (0, GetLength ()-1, 1);
		fHead->SetPriority (RandomSize_t (bucketSize*(maxHeight-1), bucketSize*maxHeight -1));
		Assert (fHead->GetChild (Direction::kLeft) == nullptr or fHead->GetPriority () > fHead->GetChild (Direction::kLeft)->GetPriority ());
		Assert (fHead->GetChild (Direction::kRight) == nullptr or fHead->GetPriority () > fHead->GetChild (Direction::kRight)->GetPriority ());
		fHead->SetParent (nullptr);
	}

	delete[] nodeList;
}

template <typename KEY, typename VALUE, typename TRAITS>
FindPrioritization	Treap<KEY,VALUE,TRAITS>::GetFindPrioritization ()
{
 	return TRAITS::kOptimizeOnFindChance;
}


template <typename KEY,  typename VALUE, typename TRAITS>
void    Treap<KEY, VALUE, TRAITS>::Invariant () const
{
#if     qDebug
    Invariant_ ();
#endif
}


#if qDebug

template <typename KEY, typename VALUE, typename TRAITS>
void	Treap<KEY,VALUE,TRAITS>::ValidateBranch (Node* n, size_t& count)
{
	RequireNotNull (n);
	++count;

     for (Direction d  : { Direction::kLeft, Direction::kRight} ) {
	    Node* child = n->GetChild (d);
        if (child != nullptr) {
            int comp = TRAITS::Comparer::Compare (n->fEntry.GetKey (), child->fEntry.GetKey ());
            Assert ((comp == 0) or ((comp > 0) == (d == Direction::kLeft)));
            Assert (n->GetPriority () >= child->GetPriority ());
            Assert (child->GetParent () == n);
            ValidateBranch (child, count);
        }
    }
}

template <typename KEY, typename VALUE, typename TRAITS>
void	Treap<KEY,VALUE,TRAITS>::Invariant_ () const
{
	size_t	count = 0;

	if (fHead != nullptr) {
		ValidateBranch (fHead, count);
	}
	Assert (count == fLength);
}

template <typename KEY, typename VALUE, typename TRAITS>
void	Treap<KEY,VALUE,TRAITS>::ListAll (Iterator it) const
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
size_t	Treap<KEY,VALUE,TRAITS>::CalcNodeHeight (Node* n, size_t height, size_t* totalHeight)
{
	if (n == nullptr) {
		if (totalHeight != nullptr) {
			*totalHeight += height;
		}
		return height;
	}

	size_t	newHeight = std::max (
		CalcNodeHeight (n->GetChild (Direction::kLeft), height+1, totalHeight),
		CalcNodeHeight (n->GetChild (Direction::kRight), height+1, totalHeight));

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
        FindPrioritization::kDefault> >  NoDupAddTree;
    DuplicateAddBehaviorTest<Treap<int, int>, NoDupAddTree> (testDataLength, verbose, 1);

   typedef    Treap<int, int, TreapTraits<
        KeyValue<int,int>,
        ADT::DefaultComp<int>,
        ADT::eInvalidRemoveIgnored,
        FindPrioritization::kDefault> > InvalidRemoveIgnoredTree;
    InvalidRemoveBehaviorTest<Treap<int, int>, InvalidRemoveIgnoredTree> (verbose, 1);

    typedef    Treap<string, string, TreapTraits<
        SharedStringKeyValue,
        CaseInsensitiveCompare,
        ADT::eDefaultPolicy,
        FindPrioritization::kDefault> > SharedCaseInsensitiveString;
    StringTraitOverrideTest<SharedCaseInsensitiveString> (verbose, 1);

    typedef Treap<HashKey<string>, string>   HashedString;
    HashedStringTest<HashedString> (verbose, 1);


    SimpleIteratorTest< Treap<size_t, size_t> > (testDataLength, verbose, 1);
    PatchingIteratorTest<Tree_Patching<Treap<size_t, size_t> > > (testDataLength, verbose, 1);
}

#endif

    }   // namespace BinaryTree
}   // namespace ADT
