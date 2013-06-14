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


template <typename KEY,
		  typename VALUE,
		  typename TRAITS = ADT::Traits<
				KeyValue<KEY,VALUE>,
				ADT::DefaultComp<KEY>,
				ADT::eDefaultPolicy> >
class RedBlackTree {
	public:
        typedef  KEY	KeyType;
        typedef  VALUE  ValueType;

        typedef enum Color {
            kBlack = false,
            kRed = true
        } Color;

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

 		size_t	GetLength () const;		// always equal to total Add minus total Remove

		void	ReBalance ();


	public:
        class	Node {
            public:
                Node (const KeyType& key, const ValueType& val);
                Node (const Node& n);

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

                // Red-Black Tree additions
                Color   GetColor () const;
                void    SetColor (Color c);
                static  bool   IsRed (Node* n);   // NOTE: any leaf (nullptr) is kBlack

               DECLARE_USE_BLOCK_ALLOCATION(Node);

                 typename	TRAITS::KeyValue	fEntry;
           private:
                static  const   size_t  kMask = ((size_t) -1)-1;

                Node*	    fParent;
                Node*       fChildren[2];
                Color       fColor;
		};
		Node*	fHead;

		/*
			Find closest node for key. In cases of duplicate values, return first found.
			If not found return the node that is smaller than key by the least amount. For cases that did
			not match, the returned node will always an external node (at least one nullptr leaf). Returns
			nullptr if tree is empty.
		 */
		Node*	FindNode (const KeyType& key, int* comparisonResult, bool ignoreMatch = false)  const;

		/*
			These return the first and last entries in the tree (defined as the first and last entries that would be returned via
			iteration, assuming other users did not alter the tree.  Note that these routines require no key compares, and are thus very fast.
		*/
		Node*	GetFirst () const;
		Node*	GetLast () const;


		// swap places of left or right child with n. A left rotation makes the right child the new parent, and a right rotation makes the left child the new parent
		Node* Rotate (Node* n, Direction rotateDir);

		static	Node*	DuplicateBranch (Node* branchTop);

        // returns the parent of the added node.
		Node*	AddNode (Node* n);
		void	RemoveNode (Node* n);

		void	FixAfterInsert (Node* n);
		void    FixAfterRemove (Node* n);

	public:
		#if qDebug
			void	ListAll () const;
			static	void	ValidateBranch (Node* n, size_t& count, size_t& blackNodesToHead);
			void	ValidateAll () const;
		#endif

	private:
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
