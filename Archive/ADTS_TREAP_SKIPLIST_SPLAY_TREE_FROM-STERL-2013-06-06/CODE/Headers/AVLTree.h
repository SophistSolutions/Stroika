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


template <typename KEY,
		  typename VALUE,
		  typename TRAITS = ADT::Traits<
				KeyValue<KEY,VALUE>,
				ADT::DefaultComp<KEY>,
				ADT::eDefaultPolicy >
		>
class AVLTree {
	public:
        typedef KEY	   KeyType;
        typedef VALUE  ValueType;

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

 		nonvirtual	size_t	GetLength () const;		// always equal to total Add minus total Remove

		nonvirtual  void	ReBalance ();

	public:
        class	Node {
            public:
                Node (const KeyType& key, const ValueType& val);
                Node (const Node& n);

                DECLARE_USE_BLOCK_ALLOCATION(Node);

                // tree node core routines

                Node*   GetParent () const;
                void    SetParent (Node* p);

                Node*   GetChild (Direction direction);
                void    SetChild (Direction direction, Node* n);

                bool    IsChild (Direction direction);

                static  Direction     GetChildDir (Node* n);  // returns which side of parent node is on, or kBadDir if node is null or parent is null
                static  Direction     OtherDir (Direction dir);
                static  void          SetChild_Safe (Node* parent, Node* n, Direction d);

                static  Node*   GetFirst (Node* n);
                static  Node*   GetLast (Node* n);

                typename	TRAITS::KeyValue	fEntry; // we look at this directly because we want to see internal fields be const reference rather than value

                // AVL Additions
                short   GetBalance () const;
                void    SetBalance (short b);
                bool    IsUnbalanced () const;

                void    CalcBalanceStateInBalancedBranch (bool recurse);

            private:
                Node*   fChildren[2];
                Node*	fParent;
                short   fBalance;
        };
		Node*	fHead;


		/*
			Find node with matching key. In cases of duplicate values, return first found if ignoreMatch is false, otherwise
			treat matching keys as if the comparison returned <
		 */
		nonvirtual	Node*	FindNode (const KeyType& key, int* comparisonResult, bool ignoreMatch = false)  const;

		/*
			These return the first and last entries in the tree (defined as the first and last entries that would be returned via
			iteration, assuming other users did not alter the tree. Note that these routines require no key compares, and are thus very fast.
		*/
		nonvirtual	Node*	GetFirst () const;
		nonvirtual	Node*	GetLast () const;

    private:
        // swap places of left or right child with n. A left rotation makes the right child the new parent, and a right rotation makes the left child the new parent
		nonvirtual	Node*   Rotate (Node* n, Direction rotateDir);
		nonvirtual	Node*    DoubleRotate (Node* n, Direction rotateDir);

		nonvirtual  void    SwapNodes (Node* parent, Node* child);

		nonvirtual	void	AddNode (Node* n);
		nonvirtual	void	RemoveNode (Node* n);


        nonvirtual Node*    RebalanceNode (Node* n, Direction leanDir, int mod);    // mod is -1 for removal, +1 for insertion

	public:
		#if qDebug
			nonvirtual	void	ListAll () const;
			static		int 	ValidateBranch (Node* n, size_t& count);
			nonvirtual	void	ValidateAll () const;
		#endif

	private:
		size_t		fLength;

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
