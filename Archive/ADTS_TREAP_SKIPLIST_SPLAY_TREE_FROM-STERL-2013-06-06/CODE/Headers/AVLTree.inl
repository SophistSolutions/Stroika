#pragma once

#include <algorithm>
#include <stack>

#if qDebug
	#include <iostream>
    #include "../Shared/Headers/ContainerValidation.h"
#endif

namespace   ADT {
    namespace   BinaryTree {



template  <typename KEYVALUE>
 class	AVLNode : public TreeNode<KEYVALUE> {
    public:
        typedef KEYVALUE    KeyValue;
        typedef typename KEYVALUE::KeyType  KeyType;
        typedef typename KEYVALUE::ValueType  ValueType;
        typedef TreeNode<KEYVALUE>  inherited;

    public:
        AVLNode (const KeyType& key, const ValueType& val) :
            inherited (key, val),
            fBalance (0)
        {
        }

        AVLNode (const AVLNode& n) :
            inherited (n),
            fBalance (0)
        {
        }

        DECLARE_USE_BLOCK_ALLOCATION(AVLNode);

        // dumb shadowing
        nonvirtual  AVLNode*   GetParent () const
        {
            return static_cast<AVLNode*> (inherited::GetParent ());
        }

        AVLNode*   GetChild (Direction direction)
        {
              return static_cast<AVLNode*> (inherited::GetChild (direction));
        }

        static  AVLNode*   GetFirst (AVLNode* n)
        {
            return static_cast<AVLNode*> (inherited::GetFirst (n));
        }

        static  AVLNode*   GetLast (AVLNode* n)
        {
            return static_cast<AVLNode*> (inherited::GetLast (n));
        }

        nonvirtual  AVLNode*   GetSibling () const
        {
             return static_cast<AVLNode*> (inherited::GetSibling ());
        }

        nonvirtual  AVLNode*   Traverse (TraversalType t, Direction d) const
        {
              return static_cast<AVLNode*> (inherited::Traverse (t, d));
        }

        static      AVLNode*   GetFirstTraversal (const AVLNode* n, TraversalType t, Direction d, size_t* height)
        {
               return static_cast<AVLNode*> (inherited::GetFirstTraversal (n, t, d, height));
       }

        static      AVLNode*   Descend (AVLNode* n, int height, Direction d)
        {
            return static_cast<AVLNode*> (inherited::Descend (n, height, d));
        }


        static  void    Apply (const AVLNode* node, TraversalType t, Direction d,  const  std::function<void(const AVLNode& n)>& func)
        {
            inherited::Apply (node, t, d, [func] (const TreeNode<KEYVALUE>& n) { func (static_cast<const AVLNode&> (n)); });
        }

        static  const   AVLNode*   Find (const AVLNode* node, TraversalType t, Direction d,  const std::function<bool(const AVLNode& n)>& func)
        {
            return static_cast<const AVLNode*> (inherited::Find (node, t, d, [func] (const TreeNode<KEYVALUE>& n)->bool  {  return func (static_cast<const AVLNode&> (n)); }));
        }

        void    Inspect () const
        {
            std::cout << "(" << this->fEntry.GetKey () << "," << this->fEntry.GetValue () << "," << this->GetBalance () << ")";
        }


        // AVL Additions
        nonvirtual  short   GetBalance () const
        {
            return fBalance;
        }

        nonvirtual  void    SetBalance (short b)
        {
            Require (b >= -1 and b <= 1);
            fBalance = b;
        }

        nonvirtual  bool    IsUnbalanced () const
        {
            return (fBalance < -1 or fBalance > 1);
        }

    //   nonvirtual  void    CalcBalanceStateInBalancedBranch (bool recurse);


   private:
        short	fBalance;
};


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
    Require (rotateDir == Direction::kLeft or rotateDir == Direction::kRight);

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
typename AVLTree<KEY,VALUE,TRAITS>::Node*  AVLTree<KEY,VALUE,TRAITS>::DoubleRotate (Node* n, Direction rotateDir)
{
	RequireNotNull (n);
    Require (rotateDir == Direction::kLeft or rotateDir == Direction::kRight);

    Direction   otherDir = OtherDir (rotateDir);

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
    else if ((grandkid->GetBalance () == -1) == (rotateDir == Direction::kLeft)) {
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
	    Direction childDir = Direction::kBadDir;
		if (comp == 0) {
            if (TRAITS::kPolicy & ADT::eDuplicateAddThrowException) {
                throw DuplicateAddException ();
            }
            childDir = Direction::kLeft;
		}
		else if (comp < 0) {
            childDir = Direction::kLeft;
		}
		else {
            childDir = Direction::kRight;
		}

		Assert (childDir != Direction::kBadDir);
		Assert (nearest->GetChild (childDir) == nullptr);
		nearest->SetChild (childDir, n);

		Node*   current = nearest;
		while (current != nullptr) {
		    if (current->GetBalance () == 0) {
		        current->SetBalance (current->GetBalance () + (childDir == Direction::kLeft) ? -1 : 1);
		        childDir = Node::GetChildDir (current);
		        current = current->GetParent ();
		    }
		    else {
		        bool    doubleLean = ((current->GetBalance () < 0) == (childDir == Direction::kLeft));
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
        if ((child->GetBalance () == -1) == (leanDir == Direction::kLeft)) {
            Rotate (n, OtherDir (leanDir));
            n->SetBalance (0);
            child->SetBalance (0);
        }
        else {
            DoubleRotate (n, leanDir);
        }
    }
    else {
        if (child->GetBalance () == 0) {
             Node* newTop = Rotate (n, OtherDir (leanDir));
             newTop->SetBalance (newTop->GetBalance ());
             Assert (child == newTop);
             child->SetBalance ((leanDir == Direction::kLeft) ? 1 : -1);
        }
        else if (n->GetBalance () == child->GetBalance ()) {
           Node* newTop = Rotate (n, OtherDir (leanDir));
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

	if (n->GetChild (Direction::kLeft) == nullptr and n->GetChild (Direction::kRight) == nullptr) {
	    SwapNodes (n, nullptr);
	}
	else if (n->GetChild (Direction::kLeft) == nullptr) {
		SwapNodes (n, n->GetChild (Direction::kRight));
		if (parent == nullptr) {
            fHead->SetBalance (n->GetBalance () -1);
		}
	}
	else if (n->GetChild (Direction::kRight) == nullptr) {
		SwapNodes (n, n->GetChild (Direction::kLeft));
		if (parent == nullptr) {
            fHead->SetBalance (n->GetBalance () +1);
		}
	}
	else {
        Direction d = (FlipCoin ()) ? Direction::kRight : Direction::kLeft;
        Node* minNode = (d == Direction::kRight)
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
		minNode->SetChild (OtherDir (d), n->GetChild (OtherDir (d)));
	}

	if (childDir != Direction::kBadDir) {
		Node*   current = parent;
		while (current != nullptr) {
		    if (current->GetBalance () == 0) {
		        current->SetBalance ((childDir == Direction::kLeft) ? 1 : -1);
		        break;
		    }
		    else {
		        bool    doubleLean = ((current->GetBalance () > 0) == (childDir == Direction::kLeft));
		        if (doubleLean) {
                    current = RebalanceNode (current, OtherDir (childDir), -1);
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
typename AVLTree<KEY,VALUE,TRAITS>::Iterator AVLTree<KEY,VALUE,TRAITS>::Iterate (TraversalType t, Direction d) const
{
    return Iterator (Node::GetFirstTraversal (fHead, t, d, nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename AVLTree<KEY,VALUE,TRAITS>::Iterator AVLTree<KEY,VALUE,TRAITS>::Iterate (const KeyType& key, TraversalType t, Direction d) const
{
    int	comp;
    return Iterator (Node::GetFirstTraversal (FindNode (key, &comp), t, d, nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename AVLTree<KEY,VALUE,TRAITS>::Iterator  AVLTree<KEY,VALUE,TRAITS>::begin (TraversalType t, Direction d) const
{
    return Iterator (Node::GetFirstTraversal (fHead, t, d, nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename    AVLTree<KEY,VALUE,TRAITS>::Iterator  AVLTree<KEY,VALUE,TRAITS>::end () const
{
    return Iterator (nullptr);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename    AVLTree<KEY,VALUE,TRAITS>::Iterator	AVLTree<KEY,VALUE,TRAITS>::GetFirst (TraversalType t, Direction d) const
{
    return Iterator (Node::GetFirstTraversal (fHead, t, d, nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename    AVLTree<KEY,VALUE,TRAITS>::Iterator	AVLTree<KEY,VALUE,TRAITS>::GetLast (TraversalType t, Direction d) const
{
    return Iterator (Node::GetFirstTraversal (fHead, t, OtherDir (d), nullptr), t, d);
}

template <typename KEY, typename VALUE, typename TRAITS>
void    AVLTree<KEY,VALUE,TRAITS>::Update (const Iterator& it, const ValueType& newValue)
{
    Require (not it.Done ());
    const_cast<AVLTree<KEY, VALUE, TRAITS>::Node*> (it.GetNode ())->fEntry.SetValue (newValue);
}

template <typename KEY, typename VALUE, typename TRAITS>
void	AVLTree<KEY,VALUE,TRAITS>::Remove (const Iterator& it)
{
    Require (not it.Done ());
    RemoveNode (const_cast<AVLTree<KEY,VALUE,TRAITS>::Node*> (it.GetNode ()));
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
		n = (*comparisonResult <= 0) ? n->GetChild (Direction::kLeft) : n->GetChild (Direction::kRight);
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
void	AVLTree<KEY,VALUE,TRAITS>::Balance ()
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

	std::function<Node*(int startIndex, int endIndex, int& curNodeHeight)>	BalanceNode = [&BalanceNode, &nodeList] (int startIndex, int endIndex, int& curNodeHeight) -> Node*
	{
		Require (startIndex <= endIndex);

		if (startIndex == endIndex) {
			Node* n = nodeList[startIndex];
			n->SetChild (Direction::kLeft, nullptr);
			n->SetChild (Direction::kRight, nullptr);
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
        n->SetChild (Direction::kLeft, (curIdx == startIndex) ? nullptr : BalanceNode (startIndex, curIdx-1, lht));
        n->SetChild (Direction::kRight, (curIdx == endIndex) ? nullptr : BalanceNode (curIdx+1, endIndex, rht));

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
void	AVLTree<KEY,VALUE,TRAITS>::Invariant () const
{
    #if qDebug
        Invariant_ ();
    #endif
}

#if qDebug

template <typename KEY, typename VALUE, typename TRAITS>
int	AVLTree<KEY,VALUE,TRAITS>::ValidateBranch (Node* n, size_t& count)
{
	RequireNotNull (n);
	++count;
	int lHeight = 0;
	int rHeight = 0;
	if (n->GetChild (Direction::kLeft) != nullptr) {
		Assert (TRAITS::Comparer::Compare (n->fEntry.GetKey (), n->GetChild (Direction::kLeft)->fEntry.GetKey ()) >= 0);
		Assert (n->GetChild (Direction::kLeft)->GetParent () == n);
		lHeight = ValidateBranch (n->GetChild (Direction::kLeft), count);
	}
	if (n->GetChild (Direction::kRight) != nullptr) {
		// we cannot do strict < 0, because rotations can put on either side
		Assert (TRAITS::Comparer::Compare (n->fEntry.GetKey (), n->GetChild (Direction::kRight)->fEntry.GetKey ()) <= 0);
		Assert (n->GetChild (Direction::kRight)->GetParent () == n);
		rHeight = ValidateBranch (n->GetChild (Direction::kRight), count);
	}

	Assert (std::abs (rHeight-lHeight) <= 1);
if ((rHeight-lHeight) != n->GetBalance ()) {
std::cout << "for " << n->fEntry.GetValue () << "; balance = " << n->GetBalance () << "; lheight = " << lHeight << "; rHeight = " << rHeight << std::endl;
}

	Assert ((rHeight-lHeight) == n->GetBalance ());

	return 1 + std::max (lHeight, rHeight);
}


template <typename KEY, typename VALUE, typename TRAITS>
void	AVLTree<KEY,VALUE,TRAITS>::Invariant_ () const
{
	size_t	count = 0;

	if (fHead != nullptr) {
		ValidateBranch (fHead, count);
	}
	Assert (count == fLength);
}

template <typename KEY, typename VALUE, typename TRAITS>
void	AVLTree<KEY,VALUE,TRAITS>::ListAll (Iterator it) const
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


    SimpleIteratorTest< AVLTree<size_t, size_t> > (testDataLength, verbose, 1);
    PatchingIteratorTest<Tree_Patching<AVLTree<size_t, size_t> > > (testDataLength, verbose, 1);
}

#endif


    }   // namespace BinaryTree
}   // namespace ADT
