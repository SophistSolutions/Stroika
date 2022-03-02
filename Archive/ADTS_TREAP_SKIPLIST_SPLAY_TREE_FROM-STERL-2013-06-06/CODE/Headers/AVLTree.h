#pragma once

#include <random>
#include <vector>

#include "../Shared/Headers/BlockAllocated.h"
#include "../Shared/Headers/ADT.h"
#include "../Shared/Headers/BinaryTree.h"


/*
    self-balancing, low variance, often best choice
    can't prioritize
*/

namespace   ADT {
    namespace   BinaryTree {

template  <typename KEYVALUE>  class  AVLNode;    // need to forward declare when you subclass from TreeNode




template <typename KEY,
		  typename VALUE,
		  typename TRAITS = ADT::Traits<
				KeyValue<KEY,VALUE>,
				ADT::DefaultComp<KEY>,
				ADT::eDefaultPolicy >
		>
class AVLTree {
	public:
        typedef KEY	    KeyType;
        typedef VALUE   ValueType;
        typedef typename TRAITS::KeyValue    KeyValue;
        typedef AVLNode<KeyValue>  Node;
        typedef TreeIterator<Node> Iterator;

    public:
		AVLTree ();
		AVLTree (const AVLTree& t);
		~AVLTree ();

		nonvirtual	AVLTree& operator= (const AVLTree& t);

		/*
			Basic find operation. If pass in nullptr for val then only tests inclusion, otherwise fills val with value linked to key.
			In some cases (such as using a counter) you want full Node information rather than just the value -- see FindNode below for
			how to do this.
		*/
		nonvirtual	bool	Find (const KeyType& key, ValueType* val = nullptr) const;

		/*
			You can add more than one item with the same key. If you add different values with the same key, but it is unspecified which item will be returned on subsequent Find or Remove calls.
		*/
		nonvirtual	void	Add (const KeyType& key, const ValueType& val);
		nonvirtual	void	Add (const KeyType& keyAndValue);	// convenient when key and value are the same, like a sorted list of names

		nonvirtual	void	Remove (const KeyType& key);
		nonvirtual	void	RemoveAll ();


        /*
            Iterator support
        */
 		nonvirtual	Iterator Iterate (TraversalType t = TraversalType::kInOrder, Direction d = Direction::kLeft) const;

 		// returns the first entry equal to, or the smallest entry with key larger than the passed in key
 		nonvirtual	Iterator Iterate (const KeyType& key, TraversalType t = TraversalType::kInOrder, Direction d = Direction::kLeft) const;

        nonvirtual  Iterator  begin (TraversalType t = TraversalType::kInOrder, Direction d = Direction::kLeft) const;
        nonvirtual  Iterator  end () const;

		nonvirtual	Iterator	GetFirst (TraversalType t = TraversalType::kInOrder, Direction d = Direction::kLeft) const;  // synonym for begin (), Iterate ()
		nonvirtual	Iterator	GetLast (TraversalType t = TraversalType::kInOrder, Direction d = Direction::kLeft) const;   // returns iterator to largest key

        nonvirtual  void    Update (const Iterator& it, const ValueType& newValue);
        nonvirtual	void	Remove (const Iterator& it);

        nonvirtual	size_t	GetLength () const;		// always equal to total Add minus total Remove

		nonvirtual  void	Balance ();

        nonvirtual	void	Invariant () const;

        #if qDebug
            nonvirtual	void	Invariant_ () const;
			nonvirtual	void	ListAll (Iterator it) const;
		#endif

	protected:
		/*
			Find node with matching key. In cases of duplicate values, return first found if ignoreMatch is false, otherwise
			treat matching keys as if the comparison returned <
		 */
		nonvirtual	Node*	FindNode (const KeyType& key, int* comparisonResult, bool ignoreMatch = false)  const;

		/*
			These return the first and last entries in the tree (defined as the first and last entries that would be returned via
			in-order iteration, assuming other users did not alter the tree. Note that these routines require no key compares, and are thus very fast.
		*/
		nonvirtual	Node*	GetFirst () const;
		nonvirtual	Node*	GetLast () const;

        // swap places of left or right child with n. A left rotation makes the right child the new parent, and a right rotation makes the left child the new parent
		nonvirtual	Node*   Rotate (Node* n, Direction rotateDir);
		nonvirtual	Node*   DoubleRotate (Node* n, Direction rotateDir);

		nonvirtual  void    SwapNodes (Node* parent, Node* child);

		nonvirtual	void	AddNode (Node* n);
		nonvirtual	void	RemoveNode (Node* n);

        nonvirtual Node*    RebalanceNode (Node* n, Direction leanDir, int mod);    // mod is -1 for removal, +1 for insertion

		#if qDebug
			static		int 	ValidateBranch (Node* n, size_t& count);
		#endif


	private:
		Node*	fHead;
		size_t  fLength;

		#if qKeepADTStatistics
			public:
				mutable	size_t	fCompares;
				mutable	size_t	fRotations;

				size_t	CalcHeight (size_t* totalHeight = nullptr) const;
		#endif

		static	Node*	DuplicateBranch (Node* branchTop);
};

#if qDebug
template <typename KEYTYPE>
void    AVLTreeValidationSuite (size_t testDataLength = 20, bool verbose = false);
#endif


    }   // namespace BinaryTree
}   // namespace ADT
#include "AVLTree.inl"
