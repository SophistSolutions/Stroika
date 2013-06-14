#pragma once

#include <vector>


#include "../Shared/Headers/ADT.h"
#include "../Shared/Headers/BinaryTree.h"
#include "../Shared/Headers/BlockAllocated.h"


/*
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
class ShuffleTree {
	public:
        typedef KEY	    KeyType;
        typedef VALUE   ValueType;

    public:
		ShuffleTree ();
		ShuffleTree (const ShuffleTree& t);
		~ShuffleTree ();

		nonvirtual	ShuffleTree& operator= (const ShuffleTree& t);

		/*
			Basic find operation. If pass in nullptr for val then only tests inclusion, otherwise fills val with value linked to key.
			In some cases (such as using a counter) you want full Node information rather than just the value -- see FindNode below for
			how to do this.
			Note that Find is not a const method for a shuffle tree, as it can reorder the tree
		*/
		nonvirtual	bool	Find (const KeyType& key, ValueType* val = nullptr);

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
        class	Node  {
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

                static  Node*   RebalanceBranch (Node* oldTop, size_t length); // returns new top. length = number of nodes in oldTop, including itself

                typename	TRAITS::KeyValue	fEntry; // we look at this directly because we want to see internal fields be const reference rather than value

            private:
                Node*   fChildren[2];
                Node*	fParent;
        };
		Node*	fHead;


		/*
			Find node with matching key. In cases of duplicate values, return first found.
		 */
		nonvirtual	Node*	FindNode (const KeyType& key, int* comparisonResult);

		/*
			These return the first and last entries in the tree (defined as the first and last entries that would be returned via
			iteration, assuming other users did not alter the tree. Note that these routines require no key compares, and are thus very fast.
		*/
		nonvirtual	Node*	GetFirst () const;
		nonvirtual	Node*	GetLast () const;

        // move the node to the top of the tree, making future searches for it faster
		nonvirtual	void    Prioritize (Node* n);

		// swap places of left or right child with n. A left rotation makes the right child the new parent, and a right rotation makes the left child the new parent
		nonvirtual	Node*   Rotate (Node* n, Direction rotateDir);

		nonvirtual  void    SwapNodes (Node* parent, Node* child);

		nonvirtual	void	AddNode (Node* n);
		nonvirtual	void	RemoveNode (Node* n);

	public:
		#if qDebug
			nonvirtual	void	ListAll () const;
			static		void	ValidateBranch (Node* n, size_t& count);
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
void    ShuffleTreeValidationSuite (size_t testDataLength = 20, bool verbose = false);
#endif

    }   // namespace BinaryTree
}   // namespace ADT

#include "ShuffleTree.inl"


