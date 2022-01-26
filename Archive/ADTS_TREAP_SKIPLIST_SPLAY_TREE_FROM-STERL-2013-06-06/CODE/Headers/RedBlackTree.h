#pragma once

#include <vector>

#include "../Shared/Headers/Config.h"
#include "../Shared/Headers/BlockAllocated.h"

#include "../Shared/Headers/ADT.h"
#include "../Shared/Headers/BinaryTree.h"


/*
pro: good locality so can persist
cons: can't prioritize
 */


namespace   ADT {
    namespace   BinaryTree {

template  <typename KEYVALUE>  class  RedBlackTreeNode;    // need to forward declare when you subclass from TreeNode

enum class  Color {
    kBlack = false,
    kRed = true
};

template <typename KEY,
		  typename VALUE,
		  typename TRAITS = ADT::Traits<
				KeyValue<KEY,VALUE>,
				ADT::DefaultComp<KEY>,
				ADT::eDefaultPolicy> >
class RedBlackTree {
	public:
        typedef KEY	    KeyType;
        typedef VALUE   ValueType;
        typedef typename TRAITS::KeyValue    KeyValue;
        typedef RedBlackTreeNode<KeyValue>  Node;
        typedef TreeIterator<Node> Iterator;

	public:
		RedBlackTree ();
		RedBlackTree (const RedBlackTree& t);
		~RedBlackTree ();

		RedBlackTree&	operator= (const RedBlackTree& t);

		/*
			Basic find operation. If pass in nullptr for val then only tests inclusion, otherwise fills val with value linked to key.
			In some cases (such as using a counter) you want full Node information rather than just the value -- see FindNode below for
			how to do this.
		*/
		bool	Find (const KeyType& key, ValueType* val = nullptr) const;

		/*
			You can add more than one item with the same key. If you add different values with the same key, it is unspecified which item will be returned on subsequent Find or Remove calls.
		*/
		void	Add (const KeyType& key, ValueType val);
		void	Add (const KeyType& keyAndValue);	// convenient when key and value are the same, like a sorted list of names

		void	Remove (const KeyType& key);
		void	RemoveAll ();

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


 		size_t	GetLength () const;		// always equal to total Add minus total Remove

		void	Balance ();

        nonvirtual	void	Invariant () const;

        #if qDebug
            nonvirtual	void	Invariant_ () const;
            nonvirtual	void	ListAll (Iterator it) const;
		#endif

	protected:
		/*
			Find closest node for key. In cases of duplicate values, return first found.
			If not found return the node that is smaller than key by the least amount. For cases that did
			not match, the returned node will always an external node (at least one nullptr leaf). Returns
			nullptr if tree is empty.
		 */
		nonvirtual  Node*	FindNode (const KeyType& key, int* comparisonResult, bool ignoreMatch = false)  const;

		/*
			These return the first and last entries in the tree (defined as the first and last entries that would be returned via
			iteration, assuming other users did not alter the tree.  Note that these routines require no key compares, and are thus very fast.
		*/
		nonvirtual  Node*	GetFirst () const;
		nonvirtual  Node*	GetLast () const;


		// swap places of left or right child with n. A left rotation makes the right child the new parent, and a right rotation makes the left child the new parent
		nonvirtual  Node* Rotate (Node* n, Direction rotateDir);

		static	Node*	DuplicateBranch (Node* branchTop);

        // returns the parent of the added node.
		nonvirtual  Node*	AddNode (Node* n);
		nonvirtual  void	RemoveNode (Node* n);

		nonvirtual  void	FixAfterInsert (Node* n);
		nonvirtual  void    FixAfterRemove (Node* n);

		#if qDebug
			static	void	ValidateBranch (Node* n, size_t& count, size_t& blackNodesToHead);
		#endif

	private:
		Node*	fHead;
		size_t	fLength;

		#if qKeepADTStatistics
			public:
				mutable	size_t	fCompares;
				mutable	size_t	fRotations;

				size_t	CalcHeight (size_t* totalHeight = nullptr) const;
				static	size_t	CalcNodeHeight (Node* n, size_t height, size_t* totalHeight);
		#endif
  };

#if qDebug
template <typename KEYTYPE>
void    RedBlackTreeValidationSuite (size_t testDataLength = 20, bool verbose = false);
#endif

    }   // namespace BinaryTree
}   // namespace ADT

#include "RedBlackTree.inl"
