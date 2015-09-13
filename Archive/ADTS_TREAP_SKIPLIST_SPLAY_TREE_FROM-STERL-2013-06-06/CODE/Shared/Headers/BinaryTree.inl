#include <algorithm>
#include <stack>

#if qDebug
	#include <iostream>
    #include <vector>
#endif

namespace ADT {
    namespace BinaryTree {

inline  bool    FlipCoin ()
{
	static	size_t	sCounter = 0;
	return (++sCounter & 1);
}




/*
TreeNode
*/
template <typename KEYVALUE>
TreeNode<KEYVALUE>::TreeNode (const KeyType& key, const ValueType& val)	:
	fEntry (key, val),
	fParent (nullptr)
{
    fChildren[AsInt (Direction::kLeft)] = nullptr;
    fChildren[AsInt (Direction::kRight)] = nullptr;
}

template <typename KEYVALUE>
TreeNode<KEYVALUE>::TreeNode (const TreeNode& n)	:
	fEntry (n.fEntry),
	fParent (nullptr)
{
    fChildren[AsInt (Direction::kLeft)] = nullptr;
    fChildren[AsInt (Direction::kRight)] = nullptr;
}


template <typename KEYVALUE>
TreeNode<KEYVALUE>*   TreeNode<KEYVALUE>::GetParent () const
{
    return fParent;
}

template <typename KEYVALUE>
void    TreeNode<KEYVALUE>::SetParent (TreeNode* p)
{
    fParent = p;
}

 template <typename KEYVALUE>
TreeNode<KEYVALUE>*   TreeNode<KEYVALUE>::GetChild (Direction direction) const
 {
    Require (direction == Direction::kLeft or direction == Direction::kRight);
    return (fChildren[AsInt (direction)]);
}

template <typename KEYVALUE>
 void   TreeNode<KEYVALUE>::SetChild (Direction direction, TreeNode* n)
{
    Require (direction == Direction::kLeft or direction == Direction::kRight);
    fChildren[AsInt (direction)] = n;
    if (n != nullptr) {
        n->fParent = this;
    }
}
template <typename KEYVALUE>
bool    TreeNode<KEYVALUE>::IsChild (Direction direction) const
{
    Require (direction == Direction::kLeft or direction == Direction::kRight);
    return (fParent != nullptr and fParent->GetChild (direction) == this);
}

template <typename KEYVALUE>
Direction     TreeNode<KEYVALUE>::GetChildDir (const TreeNode* n)
{
    if (n != nullptr and n->GetParent () != nullptr) {
        if (n == n->GetParent ()->GetChild (Direction::kLeft)) {
            return Direction::kLeft;
        }
        if (n == n->GetParent ()->GetChild (Direction::kRight)) {
            return Direction::kRight;
        }
    }
    return Direction::kBadDir;
}

template <typename KEYVALUE>
void    TreeNode<KEYVALUE>::SetChild_Safe (TreeNode* parent, TreeNode* n, Direction d)
{
   Require (parent == nullptr or d == Direction::kLeft or d == Direction::kRight);
   if (parent == nullptr) {
       if (n != nullptr) {
           n->fParent = nullptr;
       }
   }
   else {
       parent->SetChild (d, n);
   }
}

template <typename KEYVALUE>
TreeNode<KEYVALUE>*	TreeNode<KEYVALUE>::GetFirst (const TreeNode* n, size_t* height)
{
    if (height != nullptr) { *height = 0; }
 	while (n != nullptr and n->GetChild (Direction::kLeft) != nullptr) {
        if (height != nullptr) { (*height)++; }
		n = n->GetChild (Direction::kLeft);
	}
	return const_cast<TreeNode*> (n);
}

template <typename KEYVALUE>
TreeNode<KEYVALUE>*	TreeNode<KEYVALUE>::GetLast (const TreeNode* n, size_t* height)
{
    if (height != nullptr) { *height = 0; }
  	while (n != nullptr and n->GetChild (Direction::kRight) != nullptr) {
        if (height != nullptr) { (*height)++; }
		n = n->GetChild (Direction::kRight);
	}
	return const_cast<TreeNode*> (n);
}

template <typename KEYVALUE>
TreeNode<KEYVALUE>*    TreeNode<KEYVALUE>::GetSibling () const
{
    Direction   d = GetChildDir (this);
    if (d == Direction::kBadDir) {
        // we must be the head
        return nullptr;
    }
    return GetParent ()->GetChild (OtherDir (d));
}

template <typename KEYVALUE>
TreeNode<KEYVALUE>*    TreeNode<KEYVALUE>::Descend (TreeNode* n, int height, Direction d)
{
    Require (height >= 0);

    if (n == nullptr) {
        return nullptr;
    }
    if (height == 0) {
        return n;
    }
    TreeNode*   child = Descend (n->GetChild (d), height-1, d);
    if (child == nullptr) {
        child = Descend (n->GetChild (OtherDir (d)), height-1, d);
    }
    return child;
}

template <typename KEYVALUE>
TreeNode<KEYVALUE>*   TreeNode<KEYVALUE>::Traverse (TraversalType t, Direction d) const
{
    Direction   od = OtherDir (d);
    switch (t) {
        case TraversalType::kPreOrder:   // us, then d child, then other d child
            {
                if (GetChild (d) != nullptr) {
                    return GetChild (d);
                }
                else if (GetChild (od) != nullptr) {
                    return GetChild (od);
                }
                else {
                     // we are a leaf. Walk back up parents until we are no longer the last child to iterate, and other child is non-null.
                    const TreeNode*       n = this;
                    Direction   cd = n->GetChildDir (n);
                    while ((cd == od) or (cd == d and n->GetSibling () == nullptr)) {
                        n = n->GetParent ();
                        cd = GetChildDir (n);
                    }
                    AssertNotNull (n);
                    return n->GetSibling ();
                }
            }
            break;
        case TraversalType::kInOrder:    // d child, then us, then other d child (sorted order)
            {
                 if (GetChild (od) != nullptr) {
                    return ((d == Direction::kLeft) ? GetFirst (GetChild (od)) : GetLast (GetChild (od)));
                 }
                 else if (IsChild (d)) {
                     return GetParent ();
                 }
                 else {
                     // walk up the tree until have d parent.
                     TreeNode*  n = GetParent ();
                     while (n != nullptr and GetChildDir (n) == od) {
                        n = n->GetParent ();
                    }
                    return ((n == nullptr) ? nullptr : n->GetParent ());
                 }
            }
            break;
        case TraversalType::kPostOrder:  // d child, then other d child, then us
            {
                if (GetChildDir (this) == d) {
                    TreeNode*   sibling = GetSibling ();
                    if (sibling == nullptr) {
                        return GetParent ();
                    }

                    TreeNode* n = sibling;
                    while (true) {
                        if (n->GetChild (d) != nullptr) {
                            n = n->GetChild (d);
                        }
                        else if (n->GetChild (OtherDir (d)) != nullptr) {
                            n = n->GetChild (OtherDir (d));
                        }
                        else {
                            break;
                        }
                    }
                    return (n);
                }
                else {
                    return GetParent ();
                }
            }
            break;
        case TraversalType::kBreadthFirst:  // done by TreeNode height, lowest to highest, traverse each row by Direction d
            {
                Direction cd = GetChildDir (this);
                if ((cd == d) and (GetSibling () != nullptr)) {
                    return GetSibling ();
                }
                TreeNode* This = const_cast<TreeNode*> (this);


                int height = 0;
                TreeNode*   n = This;

                while (n != nullptr) {
                    while ((GetChildDir (n) == OtherDir (d)) or ((n->GetParent () != nullptr) and (n->GetSibling () == nullptr))) {
                        height++;
                        n = n->GetParent ();
                    }
                    AssertNotNull (n);
                    n = n->GetParent ();
                    height++;
                    if (height > 0 and n != nullptr) {
                        TreeNode* newN = Descend (n->GetChild (OtherDir (d)), height-1, d);
                        if (newN != nullptr) {
                            n = newN;
                            break;
                        }
                    }
                    else {
                        n = nullptr;
                    }
                }


                if (n == nullptr) {
                    // okay, finished a tier, time to find first on next tier
                    height = 0;
                    n = This;
                    while (n->GetParent () != nullptr) {
                        ++height;
                        n = n->GetParent ();
                    }
                   n = Descend (n, height+1, d);
                }
                return n;
            }
            break;
        default:    AssertNotReached ();
   }

    return nullptr;
}


template <typename KEYVALUE>
TreeNode<KEYVALUE>*   TreeNode<KEYVALUE>::GetFirstTraversal (const TreeNode* n, TraversalType t, Direction d, size_t* height)
{
    if (height != nullptr) {  *height = 0;  }
    if (n == nullptr) {
        return nullptr;
    }
    if (height != nullptr) { (*height)++;   }

    TreeNode* N = const_cast<TreeNode*> (n);

    switch (t) {
        case TraversalType::kPreOrder:      return N;
        case TraversalType::kInOrder:       return (d == Direction::kLeft) ? GetFirst (n, height) : GetLast (n, height);
        case TraversalType::kBreadthFirst:  return N;
        case TraversalType::kPostOrder:
            {
                TreeNode* tmp = N;
                while (true) {
                    if (tmp->GetChild (d) != nullptr) {
                        tmp = tmp->GetChild (d);
                        if (height != nullptr) { (*height)++;   }
                       }
                    else if (tmp->GetChild (OtherDir (d)) != nullptr) {
                        tmp = tmp->GetChild (OtherDir (d));
                        if (height != nullptr) { (*height)++;   }
                    }
                    else {
                        break;
                    }
                }
                return (tmp);
            }
            break;
        default:    AssertNotReached ();
    }

    return nullptr;
}

template <typename KEYVALUE>
TreeNode<KEYVALUE>*   TreeNode<KEYVALUE>::RebalanceBranch (TreeNode* oldTop, size_t length)
{
    if (oldTop == nullptr) {
        Assert (length == 0);
        return nullptr;
    }

    // technique discussed in Galperin and Rivest, see http://www.akira.ruc.dk/~keld/teaching/algoritmedesign_f07/Artikler/03/Galperin93.pdf


    // turns the tree into a linked list (by fRight pointers) in sort order
    // currently slightly slower than building an array, but no mallocs
    // conceptually, flatten is just plain old iteration, but tree iteration is naturally recursive, unfortunately
	std::function<TreeNode*(TreeNode*,TreeNode*)>	Flatten = [&Flatten] (TreeNode* x, TreeNode* y)
	{
        x->fParent = nullptr;

        TreeNode* cRight = x->fChildren[AsInt (Direction::kRight)];
        TreeNode* cLeft = x->fChildren[AsInt (Direction::kLeft)];

        x->SetChild (Direction::kLeft, nullptr);

        x->fChildren[AsInt (Direction::kRight)] = (cRight == nullptr) ? y : Flatten (cRight, y);
        return (cLeft == nullptr) ? x :  Flatten (cLeft, x);
	};

    // takes the results of flatten (ordered linked list, by right child pointers) and constructs the proper
    // balanced tree out of it
	std::function<TreeNode*(size_t,TreeNode*)>	RebuildBranch = [&RebuildBranch] (size_t n, TreeNode* x)
	{
	    RequireNotNull (x);
	    Require (n > 0);

        size_t  tmp = (n-1)/2;
        TreeNode* r = (tmp == 0) ? x : RebuildBranch (tmp, x);
        AssertNotNull (r);

        tmp = n/2;
        TreeNode* s = (tmp == 0) ? r->GetChild (Direction::kRight) : RebuildBranch (tmp, r->GetChild (Direction::kRight));
        AssertNotNull (s);

        r->SetChild (Direction::kRight, s->GetChild (Direction::kLeft));
        s->SetChild (Direction::kLeft, r);
        return s;
	};

    TreeNode*   oldParent = oldTop->GetParent ();
    Direction   oldDir = GetChildDir (oldTop);

	TreeNode    dummy = *oldTop;
    TreeNode* result = RebuildBranch (length, Flatten (oldTop, &dummy));
    result = result->GetChild (Direction::kLeft);  // original result was our dummy last
    AssertNotNull (result);

    SetChild_Safe (oldParent, result, oldDir);
    return result;
}

#if qDebug
template <typename KEYVALUE>
void    TreeNode<KEYVALUE>::Inspect () const
{
    std::cout << "(" << fEntry.GetKey () << "," << fEntry.GetValue () << ")";
}



template <typename KEYVALUE>
const TreeNode<KEYVALUE>*   TreeNode<KEYVALUE>::Find (const TreeNode* node, TraversalType t, Direction d,  const std::function<bool(const TreeNode& n)>& func)
{
   std::function<const TreeNode*(const TreeNode*, TraversalType, Direction)>	FindNode = [&FindNode, &func] (const TreeNode* n, TraversalType tt, Direction dd) -> const TreeNode*
    {
        if (n != nullptr) {
            if (tt == TraversalType::kPreOrder) {
                if (func (*n)) { return n; }
             }

            const TreeNode* tmp = FindNode (n->GetChild (dd), tt, dd);
            if (tmp != nullptr) {
                return tmp;
            }

            if (tt == TraversalType::kInOrder) {
               if (func (*n)) { return n; }
            }

            tmp = FindNode (n->GetChild (OtherDir (dd)), tt, dd);
            if (tmp != nullptr) {
                return tmp;
            }

            if (tt == TraversalType::kPostOrder) {
                if (func (*n)) { return n; }
            }
        }
        return nullptr;
    };

    std::function<const TreeNode*(const TreeNode*, Direction)>	FindNodeBreadthFirst = [&func] (const TreeNode* n,  Direction dd) -> const TreeNode*
    {
        if (n != nullptr) {
            std::vector<const TreeNode*> v;
            v.push_back (n);
            size_t i = 0;
            while (i < v.size ()) {
                const TreeNode* curN = v[i++];
                if (curN != nullptr) {
                    if (func (*curN)) { return curN; }

                    v.push_back (curN->GetChild (dd));
                    v.push_back (curN->GetChild (OtherDir (dd)));
                 }
           }
        }
        return nullptr;
    };

    if (t == TraversalType::kBreadthFirst) {
        return FindNodeBreadthFirst (node, d);
    }
    else {
        return FindNode (node, t, d);
    }
}

template <typename KEYVALUE>
void        TreeNode<KEYVALUE>::Apply (const TreeNode* node, TraversalType t, Direction d,  const std::function<void(const TreeNode& item)>& func)
{
   std::function<void(const TreeNode*, TraversalType, Direction)>	ApplyNode = [&ApplyNode, &func] (const TreeNode* n, TraversalType tt, Direction dd)
    {
        if (n != nullptr) {
            if (tt == TraversalType::kPreOrder) {
                func (*n);
             }

            ApplyNode (n->GetChild (dd), tt, dd);

            if (tt == TraversalType::kInOrder) {
               func (*n);
            }

            ApplyNode (n->GetChild (OtherDir (dd)), tt, dd);

            if (tt == TraversalType::kPostOrder) {
               func (*n);
            }
        }
    };

    std::function<void(const TreeNode*, Direction)>	ApplyNodeBreadthFirst = [&func] (const TreeNode* n,  Direction dd)
    {
        if (n != nullptr) {
            std::vector<const TreeNode*> v;
            v.push_back (n);
            size_t i = 0;
            while (i < v.size ()) {
                const TreeNode* curN = v[i++];
                if (curN != nullptr) {
                    func (*curN);
                    v.push_back (curN->GetChild (dd));
                    v.push_back (curN->GetChild (OtherDir (dd)));
                 }
           }
        }
    };

    if (t == TraversalType::kBreadthFirst) {
        ApplyNodeBreadthFirst (node, d);
    }
    else {
        ApplyNode (node, t, d);
    }
}

#endif

/*
TreeIterator
*/
template <typename NODE>
TreeIterator<NODE>::TreeIterator (const TreeIterator<NODE>& from) :
    fCurrent (from.fCurrent),
    fSuppressMore (from.fSuppressMore),
    fTraversal (from.fTraversal),
    fDirection (from.fDirection)
{
}

template <typename NODE>
TreeIterator<NODE>::TreeIterator (NODE* node, TraversalType t, Direction d) :
    fCurrent (node),
    fSuppressMore (true),
    fTraversal (t),
    fDirection (d)
{
}


template <typename NODE>
void    TreeIterator<NODE>::Invariant () const
{
#if     qDebug
    Invariant_ ();
#endif
}

template <typename NODE>
TreeIterator<NODE>&  TreeIterator<NODE>::operator= (const TreeIterator<NODE>& rhs)
{
    Invariant ();

    fCurrent = rhs.fCurrent;
    fSuppressMore = rhs.fSuppressMore;

    Invariant ();
    return (*this);
}

template <typename NODE>
bool    TreeIterator<NODE>::Done () const
{
    Invariant ();
    return bool (fCurrent == nullptr);
}

template <typename NODE>
bool    TreeIterator<NODE>::More (KeyValue* current, bool advance)
{
    Invariant ();

    if (advance and fCurrent != nullptr) {
        if (fSuppressMore) {
            fSuppressMore = false;
        }
        else {
            fCurrent =  fCurrent->Traverse (fTraversal, fDirection);
        }
    }
    Invariant ();
    if (current != nullptr and not Done ()) {
        *current = fCurrent->fEntry;
    }
    return (not Done ());
}

template <typename NODE>
const   typename  TreeIterator<NODE>::KeyValue&   TreeIterator<NODE>::Current () const
{
    Require (not Done ());
    Invariant ();
    AssertNotNull (fCurrent);
    return (fCurrent->fEntry);
}

template <typename NODE>
const typename TreeIterator<NODE>::KeyValue&  TreeIterator<NODE>::operator* () const
{
     Require (not Done ());
     Invariant ();

     return Current ();
}

template <typename NODE>
const typename TreeIterator<NODE>::KeyValue* TreeIterator<NODE>::operator-> () const
{
    Require (not Done ());
    Invariant ();

    return &fCurrent->fEntry;
}

template <typename NODE>
TreeIterator<NODE>&    TreeIterator<NODE>::operator++ ()
{
    Invariant ();

    if (fCurrent != nullptr) {
        fCurrent =  fCurrent->Traverse (fTraversal, fDirection);
        fSuppressMore = true;
   }

    return *this;
}

template <typename NODE>
TreeIterator<NODE>&    TreeIterator<NODE>::operator-- ()
{
    Invariant ();

    if (fCurrent != nullptr) {
        fCurrent =  fCurrent->Traverse (fTraversal, OtherDir (fDirection));
        fSuppressMore = true;
   }

    return *this;
}

template <typename NODE>
const  NODE*   TreeIterator<NODE>::GetNode () const
{
    return fCurrent;
}

template <typename NODE>
bool    TreeIterator<NODE>::operator== (const TreeIterator<NODE>& rhs) const
{
     Invariant ();
     return (fCurrent == rhs.fCurrent and ((fCurrent == nullptr) or (fTraversal == rhs.fTraversal and fDirection == rhs.fDirection)));
}

template <typename NODE>
bool    TreeIterator<NODE>::operator!= (const TreeIterator<NODE>&rhs) const
{
    Invariant ();
    return (fCurrent != rhs.fCurrent or ((fCurrent != nullptr) and (fTraversal != rhs.fTraversal or fDirection != rhs.fDirection)));
}

#if     qDebug
template <typename NODE>
void    TreeIterator<NODE>::Invariant_ () const
{
}
#endif



/*
Tree_Patching
*/

template <typename TREE>
Tree_Patching<TREE>::Tree_Patching () :
    fIterators (nullptr)
{
}

template <typename TREE>
Tree_Patching<TREE>::Tree_Patching (const Tree_Patching& s) :
    TREE (s),
    fIterators (nullptr)
{
}

template <typename TREE>
Tree_Patching<TREE>::~Tree_Patching ()
{
    Require (fIterators == nullptr);
}

template <typename TREE>
Tree_Patching<TREE>&	Tree_Patching<TREE>::operator= (const Tree_Patching<TREE>& t)
{
    Require (fIterators == nullptr);   // perhaps could support by letting iterators do a find after the assignment, based on their old KEY
    TREE::operator= (t);
    return *this;
}

template <typename TREE>
typename Tree_Patching<TREE>::Iterator Tree_Patching<TREE>::Iterate (TraversalType t, Direction d) const
{
    return Iterator (*const_cast<Tree_Patching*>(this), inherited::Iterate (t, d));
}

template <typename TREE>
typename Tree_Patching<TREE>::Iterator Tree_Patching<TREE>::Iterate (const KeyType& key, TraversalType t, Direction d) const
{
    return Iterator (*const_cast<Tree_Patching*>(this), inherited::Iterate (key, t, d));
}

template <typename TREE>
typename Tree_Patching<TREE>::Iterator Tree_Patching<TREE>::begin (TraversalType t, Direction d) const
{
    return Iterator (*const_cast<Tree_Patching*>(this), inherited::begin (t, d));
}

template <typename TREE>
typename Tree_Patching<TREE>::Iterator Tree_Patching<TREE>::end () const
{
    return Iterator ();
}

template <typename TREE>
typename Tree_Patching<TREE>::Iterator Tree_Patching<TREE>::GetFirst (TraversalType t, Direction d) const
{
    return Iterator (*const_cast<Tree_Patching*>(this), inherited::GetFirst (t, d));
}

template <typename TREE>
typename Tree_Patching<TREE>::Iterator Tree_Patching<TREE>::GetLast (TraversalType t, Direction d) const
{
    return Iterator (*const_cast<Tree_Patching*>(this), inherited::GetLast (t, d));
}


template <typename TREE>
void	Tree_Patching<TREE>::Remove (const KeyType& key)
{
	int	comp;
	Node* n =  this->FindNode (key, &comp);

	if ((n == nullptr) or (comp != 0)) {
		if (not (TREE::TRAITS::kPolicy & ADT::eInvalidRemoveIgnored)) {
		    throw InvalidRemovalException ();
		}
	}
	else {
		Assert (n->fEntry.GetKey () == key);
        for (auto it = fIterators; it != nullptr; it = it->fNext) {
            it->RemovePatch (n);
        }
        this->RemoveNode (n);
	}
}

template <typename TREE>
void	Tree_Patching<TREE>::Remove (const Iterator& remIt)
{
    typedef typename Tree_Patching<TREE>::Node Node;
    Node*   target = const_cast<Node*>(remIt.fCurrent);
    for (auto it = fIterators; it != nullptr; it = it->fNext) {
        it->RemovePatch (target);
    }
    this->RemoveNode (target);
}


template <typename TREE>
void	Tree_Patching<TREE>::RemoveAll ()
{
    for (auto it = fIterators; it != nullptr; it = it->fNext) {
        it->RemoveAllPatch ();
    }
    inherited::RemoveAll ();
}

template <typename TREE>
void    Tree_Patching<TREE>::Invariant () const
{
    #if		qDebug
        Invariant_ ();
    #endif
}

#if		qDebug
    template <typename TREE>
    void	Tree_Patching<TREE>::Invariant_ () const
    {
        inherited::Invariant_ ();
        for (auto it = fIterators; it != nullptr; it = it->fNext) {
            Assert (it->fData == this);
         }
    }
#endif

/*
TreeIterator_Patch
*/
template <typename TREE>
TreeIterator_Patch<TREE>::TreeIterator_Patch () :
    TreeIterator<Node> (),
    fData (nullptr),
    fNext (nullptr),
    fSuppressAdvance (false)
{
    Assert (this->Done ());
}

template <typename TREE>
TreeIterator_Patch<TREE>::TreeIterator_Patch (const TreeIterator_Patch<TREE>& from) :
    TreeIterator<Node> (from),
    fData (from.fData),
    fNext ((from.fData == nullptr) ? nullptr : from.fData->fIterators),
    fSuppressAdvance (from.fSuppressAdvance)
{
    if (fData != nullptr) {
        fData->fIterators = this;
    }
}


template <typename TREE>
TreeIterator_Patch<TREE>::TreeIterator_Patch (TREE& data, const TreeIterator<typename TREE::Node>& it) :
    TreeIterator<Node> (it),
    fData (&data),
    fNext (data.fIterators),
    fSuppressAdvance (false)
{
    AssertNotNull (fData);
    fData->fIterators = this;

    Invariant ();
}

template <typename TREE>
TreeIterator_Patch<TREE>& TreeIterator_Patch<TREE>::operator= (const TreeIterator_Patch<TREE>& from)
{
    RemoveFromChain ();

    inherited::operator= (from);
    fSuppressAdvance = from.fSuppressAdvance;

    fData = from.fData;
    if (fData == nullptr) {
        fNext = nullptr;
    }
    else {
        fNext = fData->fIterators;
        fData->fIterators = this;
    }

    Invariant ();

    return (*this);
}

template <typename TREE>
TreeIterator_Patch<TREE>::~TreeIterator_Patch ()
{
    RemoveFromChain ();
}

template <typename TREE>
void    TreeIterator_Patch<TREE>::RemoveFromChain ()
{
    if (fData != nullptr) {
        if (fData->fIterators == this) {
            fData->fIterators = fNext;
        }
        else {
            for (auto it = fData->fIterators; it != nullptr; it = it->fNext) {
                 if (it->fNext == this) {
                    it->fNext = fNext;
                    break;
                }
            }
        }
    }
}

template <typename TREE>
bool    TreeIterator_Patch<TREE>::More (KeyValue* current, bool advance)
{
    Invariant ();

    if (advance and (this->fCurrent != nullptr) and fSuppressAdvance) {
        this->fSuppressMore = true;
        inherited::More (current, advance);
        fSuppressAdvance = false;
    }
    else {
        inherited::More (current, advance);
   }

    return (not this->Done ());
}


template <typename TREE>
TreeIterator_Patch<TREE>&    TreeIterator_Patch<TREE>::operator++()
{
    Invariant ();

    if (this->fCurrent != nullptr) {
        if (fSuppressAdvance) {
            fSuppressAdvance = false;
        }
        else {
            inherited::operator++ ();
        }
    }

    return *this;
}

template <typename TREE>
TreeIterator_Patch<TREE>&    TreeIterator_Patch<TREE>::operator--()
{
    Invariant ();

    if (this->fCurrent != nullptr) {
        if (fSuppressAdvance) {
            fSuppressAdvance = false;
        }
        else {
            inherited::operator-- ();
        }
    }

    return *this;
}


template <typename TREE>
void    TreeIterator_Patch<TREE>::RemovePatch (Node* n)
{
    RequireNotNull (n);

    if (this->fCurrent == n) {
         this->fCurrent =  this->fCurrent->Traverse (this->fTraversal, this->fDirection);
         this->fSuppressAdvance = true;
    }

    Invariant ();
}

template <typename TREE>
void    TreeIterator_Patch<TREE>::RemoveAllPatch ()
{
    this->fCurrent = nullptr;

    Invariant ();
}

template <typename TREE>
void	TreeIterator_Patch<TREE>::Invariant () const
{
    #if		qDebug
        Invariant_ ();
    #endif
}


#if		qDebug
template <typename TREE>
void	TreeIterator_Patch<TREE>::Invariant_ () const
{
    inherited::Invariant_ ();

    Assert ((fData != nullptr) or (fNext == nullptr));
    if (fData != nullptr) {
        auto it = fData->fIterators;
        for (; (it != nullptr and it != this); it = it->fNext)  ;
        Assert (it == this);
    }
}
#endif

    }   // namespace BinaryTree
}   // namespace ADT
