#pragma once

#include <vector>

#include "../Shared/Headers/Config.h"
#include "../Shared/Headers/BlockAllocated.h"

#include "../Shared/Headers/ADT.h"
#include "../Shared/Headers/BinaryTree.h"

/*
	A treap is a sorted binary tree that is further constrained by assigning a (random) priority to each node and applying the heap constraint (parent nodes are always
	of higher priority than their children).

	Treap have the following desireable features
	fast find, add and remove. Add and remove require no more comparisons than find, although they do require additional tree rotations to maintain heap balance.

	robust about order of additions. There are possible pathological trees, but in practice no series of inputs would reliably reproduce (tree order not dependent on
	keys), and changes or worst case is statistically extremely unlikely.

	ability to add more than one entry with the same key

	ability to reorder links into  optimal configuration. Balance call requires no compares and is done in O(n). It roughly halves the tree height, and reduces
	compares during find by about 25%. It reduces compares for cases of add and remove as well.

	ability to efficiently implement set interface (intersection, union and difference)

	possible to efficiently parallelize (see http://www.cs.cmu.edu/~scandal/papers/treaps-spaa98.pdf)

	ability to reorder tree based on find usage, so that frequently accessed items are found first (calling Rebalance will destroy this characteristic).

	support backwards and forwards iteration (technically support in-order, pre-order and post-order, plus backward iteration which is reverse in-order)

	Drawbacks:
	Treaps have a fair amount of overhead. They require at least 2 extra links per node for children, and often additional pointer to parent (not necessary but
	makes some routines simpler and easier to parallelize). In addition they have a priority value, also of size_t, so equivalent to a node link. Overhead is thus
	either 3 or 4 links per node, considerably higher than SkipList (1.33) or some other trees (Red-Black has only 2-3 links plus a bit or byte for color information)

	Red-Black tree and AVL tree have better default find, and also less variance in time.

	Prioritize On Find:
		If you know that your users are going to not do randomly distributed finds, setting this can produce find results that
		exceed a perfectly balanced tree. You specify a chance (never, .5 or always) that after a successful find we try to readjust
		the tree to make the found entry nearer the top (even if the decision is to modify the tree it will only do it some of the
		time, with nodes nearer the top already being less likely to move than leaf nodes).
		Empirical testing with some non-uniform finds showed fairly small degradation of find speed when given worst case, evenly
		distributed finds, and significant speedups in cases of tightly distributed finds. Treaps can out-perform self-balancing trees
		such as Red-Black and AVL on such data (Splay Trees would be an alternative for such data).
		Note that using non-zero find optimization makes parallelizing much worse, as finds can modify the tree.
 */


namespace   ADT {
    namespace   BinaryTree {

template  <typename KEYVALUE>  class  TreapNode;    // need to forward declare when you subclass from TreeNode

enum class FindPrioritization {
	kNever,
	kStandard,
	kAlways,
	kDefault = kNever,
};

template    <typename KEYVALUE, typename COMP,int POLICY, FindPrioritization OPTFINDCHANCE>
struct	TreapTraits : ADT::Traits<KEYVALUE, COMP, POLICY> {
    static	const FindPrioritization	kPrioritizeOnFind = OPTFINDCHANCE;
};

template <typename KEY,
		  typename VALUE,
		  typename TRAITS=TreapTraits<
				KeyValue<KEY,VALUE>,
				ADT::DefaultComp<KEY>,
				ADT::eDefaultPolicy,
				FindPrioritization::kDefault> >
class Treap {
	public:
        typedef KEY	    KeyType;
        typedef VALUE   ValueType;
        typedef typename TRAITS::KeyValue    KeyValue;
        typedef TreapNode<KeyValue>  Node;
        typedef TreeIterator<Node> Iterator;

	public:
		Treap ();
		Treap (const Treap& t);
		~Treap ();

		nonvirtual  Treap&	operator= (const Treap& t);

		/*
			Basic find operation. If pass in nullptr for val then only tests inclusion, otherwise fills val with value linked to key.
			In some cases (such as using a counter) you want full Node information rather than just the value -- see FindNode below for
			how to do this.
			Notet at if Find prioritization is set to anything other than kNever, then Find is not really a const method
		*/
		nonvirtual  bool	Find (const KeyType& key, ValueType* val = nullptr) const;

		/*
			You can add more than one item with the same key. If you add different values with the same key, it is unspecified which item will be returned on subsequent Find or Remove calls.
		*/
		nonvirtual  void	Add (const KeyType& key, ValueType val);
		nonvirtual  void	Add (const KeyType& keyAndValue);	// convenient when key and value are the same, like a sorted list of names

		nonvirtual  void	Remove (const KeyType& key);
		nonvirtual  void	RemoveAll ();

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

 		nonvirtual  size_t	GetLength () const;		// always equal to total Add minus total Remove

		nonvirtual  void	Balance ();

		nonvirtual  void    Prioritize (const Iterator& it);

		static  FindPrioritization	GetFindPrioritization ();   // one of kNever, kStandard, kAlways


        nonvirtual	void	Invariant () const;
		#if qDebug
			void	ListAll (Iterator it) const;
			nonvirtual  void	Invariant_ () const;
		#endif

	protected:
		/*
			Find closest node for key in treap. In cases of duplicate values, return first found.
			If not found return the node that is smaller than key by the least amount. For cases that did
			not match, the returned node will always an external node (at least one nullptr leaf). Returns
			nullptr if tree is empty.
		 */
		nonvirtual  Node*	FindNode (const KeyType& key, int* comparisonResult)  const;

		/*
			These return the first and last entries in the tree (defined as the first and last entries that would be returned via
			iteration, assuming other users did not alter the tree.  Note that these routines require no key compares, and are thus very fast.
		*/
		nonvirtual	Node*	GetFirst () const;
		nonvirtual	Node*	GetLast () const;

		/*
			After insertions, a node will be in correct sort order, but not in correct heap order (parents must have
			higher priority than their children. This routine will do node rotations upwards until the tree is in proper heap order
		*/
		nonvirtual  void BubbleUp (Node* n);

		// the symmetrical case for removal, here we force to bottom of tree before removing, using rotations to keep tree in proper heap order
		nonvirtual  void ForceToBottom (Node* n);

		// swap places of left or right child with n. A left rotation makes the right child the new parent, and a right rotation makes the left child the new parent
		nonvirtual  Node* Rotate (Node* n, Direction rotateDir);

		nonvirtual  void    SwapPrioritiesIfNeeded (Node* n);

		static	Node*	DuplicateBranch (Node* branchTop);

		nonvirtual  void	AddNode (Node* n);
		nonvirtual  void	RemoveNode (Node* n);

    #if qDebug
        static	void	ValidateBranch (Node* n, size_t& count);
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
void    TreapValidationSuite (size_t testDataLength = 20, bool verbose = false);
#endif



    }   // namespace BinaryTree
}   // namespace ADT


#include "Treap.inl"
