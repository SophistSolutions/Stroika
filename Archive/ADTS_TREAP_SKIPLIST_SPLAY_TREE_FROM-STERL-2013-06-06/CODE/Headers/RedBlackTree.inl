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
 class	RedBlackTreeNode : public TreeNode<KEYVALUE> {
    public:
        typedef KEYVALUE    KeyValue;
        typedef typename KEYVALUE::KeyType  KeyType;
        typedef typename KEYVALUE::ValueType  ValueType;
        typedef TreeNode<KEYVALUE>  inherited;

    public:
        RedBlackTreeNode (const KeyType& key, const ValueType& val) :
            inherited (key, val),
            fColor (Color::kBlack)
        {
        }

        RedBlackTreeNode (const RedBlackTreeNode& n) :
            inherited (n),
            fColor (n.fColor)
        {
        }

        DECLARE_USE_BLOCK_ALLOCATION(RedBlackTreeNode);

        // dumb shadowing
        nonvirtual  RedBlackTreeNode*   GetParent () const
        {
            return static_cast<RedBlackTreeNode*> (inherited::GetParent ());
        }

        RedBlackTreeNode*   GetChild (Direction direction)
        {
              return static_cast<RedBlackTreeNode*> (inherited::GetChild (direction));
        }

        static  RedBlackTreeNode*   GetFirst (RedBlackTreeNode* n)
        {
            return static_cast<RedBlackTreeNode*> (inherited::GetFirst (n));
        }

        static  RedBlackTreeNode*   GetLast (RedBlackTreeNode* n)
        {
            return static_cast<RedBlackTreeNode*> (inherited::GetLast (n));
        }

        nonvirtual  RedBlackTreeNode*   GetSibling () const
        {
             return static_cast<RedBlackTreeNode*> (inherited::GetSibling ());
        }

        nonvirtual  RedBlackTreeNode*   Traverse (TraversalType t, Direction d) const
        {
              return static_cast<RedBlackTreeNode*> (inherited::Traverse (t, d));
        }

        static      RedBlackTreeNode*   GetFirstTraversal (const RedBlackTreeNode* n, TraversalType t, Direction d, size_t* height)
        {
               return static_cast<RedBlackTreeNode*> (inherited::GetFirstTraversal (n, t, d, height));
       }

        static      RedBlackTreeNode*   Descend (RedBlackTreeNode* n, int height, Direction d)
        {
            return static_cast<RedBlackTreeNode*> (inherited::Descend (n, height, d));
        }

        static  void    Apply (const RedBlackTreeNode* node, TraversalType t, Direction d,  const  std::function<void(const RedBlackTreeNode& n)>& func)
        {
            inherited::Apply (node, t, d, [func] (const TreeNode<KEYVALUE>& n) { func (static_cast<const RedBlackTreeNode&> (n)); });
        }

        static  const   RedBlackTreeNode*   Find (const RedBlackTreeNode* node, TraversalType t, Direction d,  const std::function<bool(const RedBlackTreeNode& n)>& func)
        {
            return static_cast<const RedBlackTreeNode*> (inherited::Find (node, t, d, [func] (const TreeNode<KEYVALUE>& n)->bool  {  return func (static_cast<const RedBlackTreeNode&> (n)); }));
        }

        void    Inspect () const
        {
            std::cout << "(" << this->fEntry.GetKey () << "," << this->fEntry.GetValue () << "," << ((GetColor () == Color::kRed) ? "R" : "B") << ")";
        }

        // Red-Black Tree additions
        nonvirtual  Color   GetColor () const
        {
            return fColor;
        }

        nonvirtual  void    SetColor (Color c)
        {
            fColor = c;
        }

        // NOTE: any leaf (nullptr) is kBlack
        static  bool   IsRed (RedBlackTreeNode* n)
        {
            return (n != nullptr and n->GetColor () == Color::kRed);
        }


   private:
        Color       fColor;
};





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
typename RedBlackTree<KEY,VALUE,TRAITS>::Node*	RedBlackTree<KEY,VALUE,TRAITS>::AddNode (Node* n)
{
	RequireNotNull (n);

    bool    allowDuplicateAdds = not (TRAITS::kPolicy & ADT::eDuplicateAddThrowException);
	int	comp;
	Node* nearest =  FindNode (n->fEntry.GetKey (), &comp, allowDuplicateAdds);
	if (nearest == nullptr) {
	    Assert (fHead == nullptr);
	    fHead = n;
	    n->SetColor (Color::kBlack);
    }
	else {
		n->SetParent (nearest);
		if (comp == 0) {
		    if (not allowDuplicateAdds) {
		        throw DuplicateAddException ();
		    }
			Assert (nearest->GetChild (Direction::kLeft) == nullptr);
		    nearest->SetChild (Direction::kLeft, n);
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
 		Assert (node->GetColor () == Color::kBlack);
        return;
    }

    node->SetColor (Color::kRed);
    while (Node::IsRed (parent)) {
        Node* grandparent = parent->GetParent ();
        if (grandparent == nullptr) {
            parent = fHead;
            parent->SetColor (Color::kBlack);
            break;
        }
        Direction parentDir = Node::GetChildDir (parent);
        Node* uncle = grandparent->GetChild (OtherDir (parentDir));
        if (Node::IsRed (uncle)) {
            parent->SetColor (Color::kBlack);
            uncle->SetColor (Color::kBlack);
            node = grandparent;
            parent = node->GetParent ();
            grandparent->SetColor ((parent == nullptr) ? Color::kBlack : Color::kRed);
       }
        else {
            if (node->IsChild (OtherDir (parentDir))) {
                node = parent;
                Rotate (node, parentDir);
            }
            parent = node->GetParent ();
            AssertNotNull (parent);
            parent->SetColor (Color::kBlack);

            grandparent = parent->GetParent ();
            if (grandparent != nullptr) {
                grandparent->SetColor (Color::kRed);
                Rotate (grandparent, OtherDir (parentDir));
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
	if (n->GetChild (Direction::kLeft) == nullptr or n->GetChild (Direction::kRight) == nullptr) {
	    sub = n;
	}
	else {
	    sub = Node::GetFirst (n->GetChild (Direction::kRight));
	}
	AssertNotNull (sub);
	Node* x = (sub->GetChild (Direction::kLeft) != nullptr) ? sub->GetChild (Direction::kLeft) : sub->GetChild (Direction::kRight);
	Direction yDir = Node::GetChildDir (sub);

	if (yDir == Direction::kBadDir) {
        Assert (n->GetChild (Direction::kLeft) == nullptr or n->GetChild (Direction::kRight) == nullptr);
	    Assert (sub == fHead);
	    Assert (sub == n);
	    fHead = x;
	    if (fHead != nullptr) {
	        fHead->SetColor (Color::kBlack);
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
            if (sub->GetColor () == Color::kBlack) {
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
        else if (sub->GetColor () == Color::kBlack) {
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

    while (n != fHead and n->GetColor () == Color::kBlack) {
        AssertNotNull (n->GetParent ());     // else n == fHead

        Direction dir = Node::GetChildDir (n);
        if (dir == Direction::kBadDir) {
            fHead = n;
            break;
        }
        Direction oDir = OtherDir (dir);
        Node* sibling = n->GetParent ()->GetChild (oDir);
        AssertNotNull (sibling);    // reasons this is true are subtle, but otherwise we would be 2 off in our black node count, which cannot be

        if (Node::IsRed (sibling)) {
            sibling->SetColor (Color::kBlack);
            n->GetParent ()->SetColor (Color::kRed);
            Rotate (n->GetParent (), dir);
            sibling = n->GetParent ()->GetChild (oDir);
       }

        if (not Node::IsRed (sibling->GetChild (Direction::kLeft)) and not Node::IsRed (sibling->GetChild (Direction::kRight))) {
            sibling->SetColor (Color::kRed);
           n = n->GetParent ();
        }
        else {
            if (not Node::IsRed (sibling->GetChild (oDir))) {
                AssertNotNull (sibling->GetChild (dir));
                sibling->GetChild (dir)->SetColor (Color::kBlack);
                sibling->SetColor (Color::kRed);
                Rotate (sibling, oDir);
                sibling = n->GetParent ()->GetChild (oDir);
            }
            sibling->SetColor (n->GetParent ()->GetColor ());
            n->GetParent ()->SetColor (Color::kBlack);
            if (sibling->GetChild (oDir) != nullptr) {
                sibling->GetChild (oDir)->SetColor (Color::kBlack);
            }
            Rotate (n->GetParent (), dir);
            n = fHead;
        }

    }
    n->SetColor (Color::kBlack);
}

template <typename KEY, typename VALUE, typename TRAITS>
void	RedBlackTree<KEY,VALUE,TRAITS>::RemoveAll ()
{
	std::function<void(Node*)>	DeleteANode = [&DeleteANode] (Node* n)
	{
		if (n != nullptr) {
			DeleteANode (const_cast<Node*> (n->GetChild (Direction::kLeft)));
			DeleteANode (const_cast<Node*> (n->GetChild (Direction::kRight)));
			delete n;
		}
	};

	DeleteANode (fHead);

	fHead = nullptr;
	fLength = 0;
}


template <typename KEY, typename VALUE, typename TRAITS>
typename RedBlackTree<KEY,VALUE,TRAITS>::Iterator RedBlackTree<KEY,VALUE,TRAITS>::Iterate (TraversalType t, Direction d) const
{
    return Iterator (Node::GetFirstTraversal (fHead, t, d, nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename RedBlackTree<KEY,VALUE,TRAITS>::Iterator RedBlackTree<KEY,VALUE,TRAITS>::Iterate (const KeyType& key, TraversalType t, Direction d) const
{
    int	comp;
    return Iterator (Node::GetFirstTraversal (FindNode (key, &comp), t, d, nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename RedBlackTree<KEY,VALUE,TRAITS>::Iterator  RedBlackTree<KEY,VALUE,TRAITS>::begin (TraversalType t, Direction d) const
{
    return Iterator (Node::GetFirstTraversal (fHead, t, d, nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename    RedBlackTree<KEY,VALUE,TRAITS>::Iterator  RedBlackTree<KEY,VALUE,TRAITS>::end () const
{
    return Iterator (nullptr);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename    RedBlackTree<KEY,VALUE,TRAITS>::Iterator	RedBlackTree<KEY,VALUE,TRAITS>::GetFirst (TraversalType t, Direction d) const
{
    return Iterator (Node::GetFirstTraversal (fHead, t, d, nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename    RedBlackTree<KEY,VALUE,TRAITS>::Iterator	RedBlackTree<KEY,VALUE,TRAITS>::GetLast (TraversalType t, Direction d) const
{
    return Iterator (Node::GetFirstTraversal (fHead, t, OtherDir (d), nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
void    RedBlackTree<KEY,VALUE,TRAITS>::Update (const Iterator& it, const ValueType& newValue)
{
    Require (not it.Done ());
    const_cast<RedBlackTree<KEY, VALUE, TRAITS>::Node*> (it.GetNode ())->fEntry.SetValue (newValue);
}

template <typename KEY, typename VALUE, typename TRAITS>
void	RedBlackTree<KEY,VALUE,TRAITS>::Remove (const Iterator& it)
{
    Require (not it.Done ());
    RemoveNode (const_cast<RedBlackTree<KEY,VALUE,TRAITS>::Node*> (it.GetNode ()));
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
		n = (*comparisonResult <= 0) ? n->GetChild (Direction::kLeft): n->GetChild (Direction::kRight);
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
void	RedBlackTree<KEY,VALUE,TRAITS>::Balance ()
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
	//		n->SetColor ((curNodeHeight == maxHeight or (not (curNodeHeight & 1))) ? kRed : kBlack);
			n->SetColor ((curNodeHeight == maxHeight) ? Color::kRed : Color::kBlack);
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
        n->SetColor (Color::kBlack);

		return n;
	};
	if (fHead != nullptr) {
		fHead = BalanceNode (0, GetLength ()-1, 1);
		fHead->SetParent (nullptr);
		fHead->SetColor (Color::kBlack);
	}

	delete[] nodeList;
}



template <typename KEY, typename VALUE, typename TRAITS>
void	RedBlackTree<KEY,VALUE,TRAITS>::Invariant () const
{
    #if qDebug
        Invariant_ ();
    #endif
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
            if (nn->GetColor () == Color::kBlack) {
                blackNodeCount++;
            }
            nn = nn->GetParent ();
        }
        return blackNodeCount;
    };

    if (n->GetColor () == Color::kRed) {
        Assert (not Node::IsRed (n->GetChild (Direction::kLeft)));
        Assert (not Node::IsRed (n->GetChild (Direction::kRight)));
   }


     for (Direction d  : { Direction::kLeft, Direction::kRight} ) {
        Node* child = n->GetChild (d);
        if (child != nullptr) {
            int comp = TRAITS::Comparer::Compare (n->fEntry.GetKey (), child->fEntry.GetKey ());
            Assert ((comp == 0) or ((comp > 0) == (d == Direction::kLeft)));
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
void	RedBlackTree<KEY,VALUE,TRAITS>::Invariant_ () const
{
	size_t	count = 0;
	size_t  blackNodesToHead = 0;


	if (fHead != nullptr) {
 //std::cout << "Validating, count = " << fLength << std::endl << std::flush;
	    Assert (fHead->GetColor () == Color::kBlack);
		ValidateBranch (fHead, count, blackNodesToHead);
	}
	Assert (count == fLength);
}

template <typename KEY, typename VALUE, typename TRAITS>
void	RedBlackTree<KEY,VALUE,TRAITS>::ListAll (Iterator it) const
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
size_t	RedBlackTree<KEY,VALUE,TRAITS>::CalcNodeHeight (Node* n, size_t height, size_t* totalHeight)
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


    SimpleIteratorTest< RedBlackTree<size_t, size_t> > (testDataLength, verbose, 1);
    PatchingIteratorTest<Tree_Patching<RedBlackTree<size_t, size_t> > > (testDataLength, verbose, 1);
}

#endif


    }   // namespace BinaryTree
}   // namespace ADT
