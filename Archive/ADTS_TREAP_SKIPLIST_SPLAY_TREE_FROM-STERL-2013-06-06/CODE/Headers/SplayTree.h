#pragma once

#include <random>
#include <vector>

#include "../Shared/Headers/ADT.h"
#include "../Shared/Headers/BinaryTree.h"
#include "../Shared/Headers/BlockAllocated.h"




/*
	SplayTree is an aggressively self-adjusting binary tree. A classic splay tree always moves any found node to the top after a find or an add. This produces
	a larger number or rotations, but with non-uniform requests, the reduction in average node lookup distance can make up the lost time.

	SplayTree have the following desireable features
	fast find, add and remove.

	ability to add more than one entry with the same key

	ability to tune splay characteristics to match usage

	support backwards and forwards iteration (technically support in-order, pre-order and post-order, plus backward iteration which is reverse in-order)

	Drawbacks:
	difficult to efficiently parallelize, because of aggresive restructuring

	somewhat sensitive to order of additions. This can temporarily make the height of a splay tree very large. But find will very quickly reduce the height, and its
	average behavior is not particularly sensitive to addition order.

	SplayTrees have less overhead than many tree structures. They require at least 2 extra links per node for children, and often additional pointer to parent
	(not necessary but  makes some routines simpler and easier to parallelize). Overhead is thus either 2 or 3 links per node, considerably higher than SkipList (1.33),
	but less than other tree structures that record additional information with every node.

	Splaying (restructuring to make a node be the top node) is controlled by SplayType, and can very from never (simple binary search tree) to always (classic splaytree).
	In between SplayTree supports 3 built-in request distributions: uniform, normal, and Zipf. SplayTrees are usually not an optimal choice for uniform requests, but
	they do better with tighter request frequencies, such as normal and especially zipf. You can also set custom weights tuned to your own user's characteristics.
*/

namespace   ADT {
    namespace   BinaryTree {


typedef enum SplayType {
	eNeverSplay = -1,
	eAlwaysSplay = 0,

	eUniformDistribution,
	eNormalDistribution,
	eZipfDistribution,
	eCustomSplayType,	// set to eUniformDistribution unless user sets custom weights

	eDefaultSplayType = eUniformDistribution
} SplayType;


template    <typename KEYVALUE, typename COMP, int POLICY, SplayType SPLAYTYPE >
struct	SplayTraits : ADT::Traits<KEYVALUE, COMP, POLICY> {
    static	const SplayType	kSplayType = SPLAYTYPE;
};

template <typename KEY,
		  typename VALUE,
		  typename TRAITS=SplayTraits<
				KeyValue<KEY,VALUE>,
				ADT::DefaultComp<KEY>,
				ADT::eDefaultPolicy,
				SplayType::eDefaultSplayType >
		>
class SplayTree {
	public:
        typedef KEY	    KeyType;
        typedef VALUE   ValueType;
        typedef typename TRAITS::KeyValue    KeyValue;
        typedef TreeNode<KeyValue>  Node;
        typedef TreeIterator<Node> Iterator;

    public:
		SplayTree ();
		SplayTree (const SplayTree& t);
		~SplayTree ();

		nonvirtual	SplayTree& operator= (const SplayTree& t);

		/*
			Basic find operation. If pass in nullptr for val then only tests inclusion, otherwise fills val with value linked to key.
			In some cases (such as using a counter) you want full Node information rather than just the value -- see FindNode below for
			how to do this.
			Note that for a splay tree, unlike most containers, Find is not a const method
		*/
		nonvirtual	bool	Find (const KeyType& key, ValueType* val = nullptr);

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

		/*
			The chance that a node will splay to near the top of the tree.
			100 is a classic splay tree, while zero is a sorted binary tree. Numbers in between
			are used, along with the current height of the node, to decide whether to keep splaying.
			Which number works best depends on users usage, in particular who normalized the key requests are.
			A splay chance of 10% does a good job or guarding against randomly distributed requests. Very tightly bounded
			requests may want to reduce the splay chance further, to avoid unnecessary rotations near the top of the tree.
		*/
		static	SplayType	GetSplayType ();

		// for expert users. If you know the details of your distribution, you can set custom height weights optimal to usage. The weights
		// represent the chance in 10000 that a node at a height one greater than the weight index will splay. The last weight
		// is also used for any nodes with heights greater than the weight lists size.
		static	const std::vector<size_t>&	GetHeightWeights (SplayType st);
		static	void	SetCustomHeightWeights (const std::vector<size_t>& newHeightWeights);

        // note that for a SplayTree calling Balance will lose priority information and can be an anti-optimization
		void	Balance ();

        nonvirtual	void	Invariant () const;

	public:
		#if qDebug
            nonvirtual	void	Invariant_ () const;
			nonvirtual	void	ListAll (Iterator it) const;
		#endif

	protected:
		/*
			Find node with matching key in treap. In cases of duplicate values, return first found.
			Note that FindNode is const: it does not alter the tree structure. You can optionally pass in the height parameter, which returns
			the number of parent nodes of the found node. This can then be passed on to Splay if you wish to manually realter the tree.
		 */
		nonvirtual	Node*	FindNode (const KeyType& key, int* comparisonResult, size_t* height = nullptr)  const;

		/*
			These return the first and last entries in the tree (defined as the first and last entries that would be returned via
			iteration, assuming other users did not alter the tree. Similar to FindNode in that they do not alter the tree, and
			support an optional height argument. Note that these routines require no key compares, and are thus very fast.
		*/
		nonvirtual	Node*	GetFirst (size_t* height = nullptr) const;
		nonvirtual	Node*	GetLast (size_t* height = nullptr) const;

		/*
			Potentially move the node nearer to the top of the tree. If specify forced always bring to top of tree. Otherwise movement is
			determined by SplayType, with eNeverSplay never moving the node, eAlwaysSplay always moving to the top, and the others rolling
			dice to decide how far, if any, to move up the node. Usually deeply nested nodes are more likely to move up than those that are
			already near the top of the tree.
		 */
		nonvirtual	void Splay (Node* n, size_t nodeHeight, bool forced = false);

        // move the node to the top of the tree, making future searches for it faster
		nonvirtual	void    Prioritize (const Iterator& it);

		// force a node to be a leaf node via rotations, useful before delete
		nonvirtual	size_t ForceToBottom (Node* n);

		// swap places of left or right child with n. A left rotation makes the right child the new parent, and a right rotation makes the left child the new parent
		nonvirtual	Node* Rotate (Node* n, Direction rotateDir);

		nonvirtual	void	AddNode (Node* n);
		nonvirtual	void	RemoveNode (Node* n);

		nonvirtual	void	SwapNodes (Node* parent, Node* child);

		#if qDebug
			static		void	ValidateBranch (Node* n, size_t& count);
		#endif

	private:
		Node*	fHead;
		size_t		fLength;

		#if qKeepADTStatistics
			public:
				mutable	size_t	fCompares;
				mutable	size_t	fRotations;

				size_t	CalcHeight (size_t* totalHeight = nullptr) const;
		#endif

		static	Node*	DuplicateBranch (Node* branchTop);

		static	std::vector<size_t>	sAlwaysSplayDistribution;
		static	std::vector<size_t>	sUniformDistribution;
		static	std::vector<size_t>	sNormalDistribution;
		static	std::vector<size_t>	sZipfDistribution;
		static	std::vector<size_t>	sCustomSplayTypeDistribution;

	private:
        typedef	std::mt19937	Engine;
		static	Engine&	GetEngine ();
};

#if qDebug
template <typename KEYTYPE>
void    SplayTreeTreeValidationSuite (size_t testDataLength = 20, bool verbose = false);
#endif

    }   // namespace BinaryTree
}   // namespace ADT


#include "SplayTree.inl"
