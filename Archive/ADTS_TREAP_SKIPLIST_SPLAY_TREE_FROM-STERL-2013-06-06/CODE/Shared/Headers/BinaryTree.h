#pragma once

#include <algorithm>

#include "Config.h"

#include "ADT.h"
#include "BlockAllocated.h"


namespace   ADT {
    namespace   BinaryTree {
        enum class  Direction {
            kLeft,
            kRight,
            kBadDir,
        };

        // TraversalType + Direction = full traversal description
        enum class  TraversalType {
            kPreOrder,
            kInOrder,
            kPostOrder,
            kBreadthFirst,
        };


        inline  Direction   OtherDir (Direction d)
        {
            Require (d == Direction::kLeft or d == Direction::kRight);
            return ((d == Direction::kLeft) ? Direction::kRight : Direction::kLeft);
        }

        inline  int AsInt (Direction d)
        {
            return (d == Direction::kLeft) ? 0 : 1;
        }


        static  bool    FlipCoin ();


        template  <typename KEYVALUE>
        class	TreeNode  {
            public:
                typedef KEYVALUE    KeyValue;
                typedef typename KEYVALUE::KeyType  KeyType;
                typedef typename KEYVALUE::ValueType  ValueType;

            public:
                TreeNode (const KeyType& key, const ValueType& val);
                TreeNode (const TreeNode& n);

                DECLARE_USE_BLOCK_ALLOCATION(TreeNode);

                // tree TreeNode core routines

                nonvirtual  TreeNode*   GetParent () const;
                nonvirtual  void        SetParent (TreeNode* p);

                nonvirtual  TreeNode*   GetChild (Direction direction) const;
                nonvirtual  void        SetChild (Direction direction, TreeNode* n);

                nonvirtual  bool    IsChild (Direction direction) const;

                nonvirtual  TreeNode*   GetSibling () const;
                nonvirtual  TreeNode*   Traverse (TraversalType t, Direction d) const;
                static      TreeNode*   GetFirstTraversal (const TreeNode* n, TraversalType t, Direction d, size_t* height);
                static      TreeNode*   Descend (TreeNode* n, int height, Direction d); // find first TreeNode height levels down, biased towards direction d

                static  Direction     GetChildDir (const TreeNode* n);  // returns which side of parent TreeNode is on, or kBadDir if TreeNode is null or parent is null
                static  void          SetChild_Safe (TreeNode* parent, TreeNode* n, Direction d);

                static  TreeNode*   GetFirst (const TreeNode* n, size_t* height = nullptr);
                static  TreeNode*   GetLast (const TreeNode* n, size_t* height = nullptr);

                static  TreeNode*   RebalanceBranch (TreeNode* oldTop, size_t length); // returns new top. length = number of TreeNodes in oldTop, including itself


                static  void  Apply (const TreeNode* node, TraversalType t, Direction d,  const  std::function<void(const TreeNode& n)>& func);

                static  const TreeNode*   Find (const TreeNode* node, TraversalType t, Direction d,  const std::function<bool(const TreeNode& n)>& func);

#if qDebug
                nonvirtual  void    Inspect () const;
#endif

                KeyValue	fEntry; // we look at this directly because we want to see internal fields be const reference rather than value

            private:
                TreeNode*   fChildren[2];
                TreeNode*	fParent;
        };


        template <typename NODE>
        class	TreeIterator : public std::iterator<std::bidirectional_iterator_tag, typename NODE::KeyType>  {
            public:
                typedef NODE        Node;
                typedef typename  Node::KeyValue    KeyValue;

            public:
                TreeIterator (Node* node = nullptr, TraversalType t = TraversalType::kInOrder, Direction d = Direction::kLeft);
                TreeIterator (const TreeIterator<NODE>& from);

                nonvirtual	TreeIterator<NODE>& operator= (const TreeIterator<NODE>& from);

            public:
                nonvirtual	bool	Done () const;
                nonvirtual	bool	More (KeyValue* current = nullptr, bool advance = true);

                nonvirtual	const KeyValue&	Current () const;

                nonvirtual	void	Invariant () const;

                // forward iterator requirements
                nonvirtual	const KeyValue& operator*() const;
                nonvirtual	const KeyValue* operator->() const;

                nonvirtual	TreeIterator&    operator++();
                nonvirtual	TreeIterator&    operator--();


                // equal iff fCurrent are equal, and traversal+direction are equal. This implies that all Done iterators are equal
                nonvirtual  bool    operator== (const TreeIterator<NODE>& rhs) const;
                nonvirtual  bool    operator!= (const TreeIterator<NODE>& rhs) const;

                nonvirtual  const NODE*   GetNode () const;

            protected:
                #if		qDebug
                    nonvirtual	void	Invariant_ () const;
                #endif

            protected:
                const   Node*	fCurrent;
                bool            fSuppressMore;      // set at construction, and if you iterate outside of More call (using operator++)
                TraversalType   fTraversal;
                Direction       fDirection;
        };

        template <typename TREE>    class   TreeIterator_Patch;

        template <typename TREE>
        class   Tree_Patching : public TREE {
            public:
                typedef typename    TREE::KeyType	KeyType;
                typedef typename    TREE::ValueType ValueType;
                typedef typename    TREE::KeyValue  KeyValue;
                typedef typename    TREE::Node Node;
                typedef TreeIterator_Patch< Tree_Patching<TREE> > Iterator;

           public:
                Tree_Patching ();
                Tree_Patching (const Tree_Patching& s);
                ~Tree_Patching ();

                Tree_Patching&	operator= (const Tree_Patching& t);

                nonvirtual	Iterator Iterate (TraversalType t = TraversalType::kInOrder, Direction d = Direction::kLeft) const;
                nonvirtual	Iterator Iterate (const KeyType& key, TraversalType t = TraversalType::kInOrder, Direction d = Direction::kLeft) const;

                nonvirtual  Iterator  begin (TraversalType t = TraversalType::kInOrder, Direction d = Direction::kLeft) const;
                nonvirtual  Iterator  end () const;

                nonvirtual	Iterator	GetFirst (TraversalType t = TraversalType::kInOrder, Direction d = Direction::kLeft) const;
                nonvirtual	Iterator	GetLast (TraversalType t = TraversalType::kInOrder, Direction d = Direction::kLeft) const;

                // these are the routines that call patch code in the iterator
                nonvirtual	void	Remove (const KeyType& key);
                nonvirtual	void	Remove (const Iterator& it);
                nonvirtual	void	RemoveAll ();

                nonvirtual  void    Invariant () const;

            protected:
                #if		qDebug
                nonvirtual	void	Invariant_ () const;
                #endif

           private:
                typedef TREE inherited;

                Iterator*    fIterators;

                friend  Iterator;
        };

        template <typename TREE>
        class   TreeIterator_Patch : public TreeIterator<typename TREE::Node>
        {
             public:
                typedef typename  TREE::Node  Node;
                typedef typename  Node::KeyValue    KeyValue;

            public:
                TreeIterator_Patch ();    // required for forward iterator
                TreeIterator_Patch (const TreeIterator_Patch<TREE>& from);
                TreeIterator_Patch (TREE& data, const TreeIterator<typename TREE::Node>& it);

                ~TreeIterator_Patch ();

                nonvirtual  TreeIterator_Patch<TREE>& operator= (const TreeIterator_Patch<TREE>& from);

                nonvirtual	bool	More (KeyValue* current = nullptr, bool advance = true);
                nonvirtual	TreeIterator_Patch&    operator++();
                nonvirtual	TreeIterator_Patch&    operator--();

                nonvirtual  void    RemovePatch (Node* n);    // called before remove
                nonvirtual  void    RemoveAllPatch ();        // called before removeall

                nonvirtual	void	Invariant () const;

            protected:
                 nonvirtual  void    RemoveFromChain ();

               #if		qDebug
                    nonvirtual	void	Invariant_ () const;
                #endif

            protected:
                TREE*                   fData;
                TreeIterator_Patch*     fNext;
                bool                    fSuppressAdvance;   // set when patching

            private:
                typedef TreeIterator<typename TREE::Node> inherited;

                friend   TREE;
        };






    }   // namespace BinaryTree
}   // namespace ADT




#include "BinaryTree.inl"


