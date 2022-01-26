#pragma once

#include <iterator>
#include <vector>

#include "../Shared/Headers/BlockAllocated.h"
#include "../Shared/Headers/ADT.h"

/*
	From Wikipedia:
	A skip list is a data structure for storing a sorted list of items using a hierarchy of linked lists that connect increasingly sparse subsequences of the items.
	These auxiliary lists allow item lookup with efficiency comparable to balanced binary search trees (that is, with number of probes proportional to log n instead of n).
	see http://en.wikipedia.org/wiki/Skip_list

	SkipLists have the following desireable features
	fast find, reasonably fast add and remove (about 20-30% as many comparisons as finds)

	robust about order of additions

	space efficient (1.33 links per value, as opposed to tree structures requiring 2 links per value)

	ability to add more than one entry with the same key

	ability to reorder links into nearly optimal configuration. You can Balance the node structure in a skip list in a single pass (order N).
	This reduces the total comparisons in a search by between 20 and 40%.

	possible to efficiently parallelize (not yet attempted -- see http://www.1024cores.net/home/parallel-computing/concurrent-skip-list)

	SkipLists support fast forward iteration (linked list traversal). They do not support backwards iteration.

	In principle you can use different probabilies for having more than one link. The optimal probability for finds is 1/4, and that also produces a list
	that is more space efficient than a traditional binary tree, as it has only 1.33 nodes per entry, compared with a binary tree using 2.


	Testing SkipList of 100000 entries, sorted add, link creation probability of 0.25
	  Unoptimized SkipList
	  total links = 133298; avg link height = 1.33298; max link height= 9
	  find avg comparisons = 28.8035; expected = 33.2193
	  add  avg comparisons = 37.5224; expected = 35.5526
	  remove  avg comparisons = 38.1671; expected = 35.5526

	  After optimizing links
	  total links = 133330; avg link height = 1.3333; max link height= 9
	  find avg comparisons = 18.1852; expected = 33.2193
	  find reduction = 36.8646%


   The "expected" above is from wikipedia, and is calculated as (log base 1/p n)/p.
 */

namespace   ADT {

template <typename KEY,  typename VALUE, typename TRAITS  >
class   SkipListIterator;

template <typename KEY,  typename VALUE, typename TRAITS  >
class   SkipListIterator_Patch;


template <typename KEY,
		  typename VALUE,
		  typename TRAITS=ADT::Traits<
				KeyValue<KEY,VALUE>,
				ADT::DefaultComp<KEY>,
				ADT::eDefaultPolicy > >
class SkipList {
	public:
        typedef KEY	KeyType;
        typedef VALUE  ValueType;
        typedef typename TRAITS::KeyValue    KeyValue;
        typedef SkipListIterator<KEY,VALUE,TRAITS> Iterator;

	public:
		SkipList ();
		SkipList (const SkipList& s);
		~SkipList ();

		nonvirtual	SkipList&	operator= (const SkipList& t);

		/*
			Basic find operation. If pass in nullptr for val then only tests inclusion, otherwise fills val with value linked to key.
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

        /*
            Iterator support
        */

 		nonvirtual	Iterator Iterate () const;

 		// returns the first entry equal to, or the smallest entry with key larger than the passed in key
 		nonvirtual	Iterator Iterate (const KeyType& key) const;

        nonvirtual  Iterator  begin () const;
        nonvirtual  Iterator  end () const;

		nonvirtual	Iterator	GetFirst () const;  // synonym for begin (), Iterate ()
		nonvirtual	Iterator	GetLast () const;   // returns iterator to largest key

        nonvirtual  void    Update (const Iterator& it, const ValueType& newValue);
        nonvirtual	void	Remove (const Iterator& it);


		// calling this will result in maximal search performance until further adds or removes
		// call when list is relatively stable in size, and it will set links to near classic log(n/2) search time
		// relatively fast to call, as is order N (single list traversal)
		nonvirtual	void	Balance ();

		// make the node faster on finds, possibly slowing other node searches down
		nonvirtual	void    Prioritize (const KeyType& key);

		nonvirtual  void    Invariant () const;

		#if qDebug
			nonvirtual	void	ListAll () const;
		#endif

		#if qKeepADTStatistics
			// height is highest link height, also counts total links if pass in non-null totalHeight
			nonvirtual	size_t	CalcHeight (size_t* totalHeight = nullptr) const;
		#endif


 		static	size_t	GetLinkHeightProbability ();	// percent chance. We use 25%, which appears optimal

    protected:
		enum {
			kMaxLinkHeight = sizeof (size_t)*8,
		};

        struct	Node {
 			Node (const KeyType& key, const ValueType& val);

			DECLARE_USE_BLOCK_ALLOCATION(Node);

			KeyValue	            fEntry;
			std::vector<Node*>		fNext;		// for a skiplist, you have an array of next pointers, rather than just one
		};

		/*
			These return the first and last entries in the tree (defined as the first and last entries that would be returned via
			iteration, assuming other users did not alter the tree.  Note that these routines require no key compares, and are thus very fast.
		*/
		nonvirtual	Node*	FirstNode () const;
		nonvirtual	Node*	LastNode () const;

		/*
			Find node for key in skiplist, else nullptr. In cases of duplicate values, return first found
		 */
		nonvirtual	Node*	FindNode (const KeyType& key)  const;

		// this is specialized for the case of adding or removing elements, as it caches
		// all links that will need to be updated for the new element or the element to be removed
		nonvirtual	Node*	FindNearest (KeyType key, std::vector<Node*>& links)  const;
		nonvirtual	Node*	FindNearest (const Iterator& it, std::vector<Node*>& links)  const;

		nonvirtual	void	AddNode (Node* n, const std::vector<Node*>&  linksToPatch, size_t minLinkHeight = 0);
		nonvirtual	void	RemoveNode (Node* n, const std::vector<Node*>& linksToPatch);

		#if		qDebug
		nonvirtual	void	Invariant_ () const;
        #endif

	private:
		nonvirtual	void	ShrinkHeadLinksIfNeeded ();
		nonvirtual	void	GrowHeadLinksIfNeeded (size_t newSize, Node* nodeToPointTo);
		nonvirtual	size_t	DetermineLinkHeight ()  const;

		std::vector<Node*>	fHead;
		size_t	fLength;

		#if qKeepADTStatistics
			public:
				mutable	size_t	fCompares;
				mutable	size_t	fRotations; // skiplists don't really do rotations, but we treak link patching as same thing
		#endif

        friend  class   SkipListIterator<KEY, VALUE, TRAITS>;
   };

#if qDebug
template <typename KEYTYPE>
void    SkipListValidationSuite (size_t testDataLength = 20, bool verbose = false);
#endif

template <typename KEY,
		  typename VALUE,
		  typename TRAITS=ADT::Traits<
				KeyValue<KEY,VALUE>,
				ADT::DefaultComp<KEY>,
				ADT::eDefaultPolicy > >
class   SkipList_Patch : public SkipList<KEY, VALUE, TRAITS> {
    public:
        typedef KEY	KeyType;
        typedef VALUE  ValueType;
        typedef typename TRAITS::KeyValue    KeyValue;
        typedef SkipListIterator_Patch<KEY,VALUE,TRAITS> Iterator;

   public:
		SkipList_Patch ();
		SkipList_Patch (const SkipList_Patch& s);
		~SkipList_Patch ();

		SkipList_Patch&	operator= (const SkipList_Patch& t);

  		nonvirtual	Iterator Iterate () const;
 		nonvirtual	Iterator Iterate (const KeyType& key) const;

        nonvirtual  Iterator  begin () const;
        nonvirtual  Iterator  end () const;

		nonvirtual	Iterator	GetFirst () const;
		nonvirtual	Iterator	GetLast () const;

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
        typedef SkipList<KEY, VALUE, TRAITS> inherited;

        Iterator*    fIterators;

        friend  class    SkipListIterator_Patch<KEY, VALUE, TRAITS>;
};




template <typename KEY, typename VALUE,  typename TRAITS >
class	SkipListIterator : public std::iterator<std::forward_iterator_tag, KEY>  {
    public:
        typedef typename  SkipList<KEY, VALUE, TRAITS>::Node  SkipListNode;
        typedef typename  SkipList<KEY, VALUE, TRAITS>::KeyValue    KeyValue;

	public:
		SkipListIterator (SkipListNode* node = nullptr);
		SkipListIterator (const SkipListIterator<KEY, VALUE, TRAITS>& from);

		nonvirtual	SkipListIterator<KEY, VALUE, TRAITS>& operator= (const SkipListIterator<KEY, VALUE, TRAITS>& from);

	public:
		nonvirtual	bool	Done () const;
		nonvirtual	bool	More (KeyValue* current = nullptr, bool advance = true);
		nonvirtual	const KeyValue&	Current () const;

		nonvirtual	void	Invariant () const;

		// forward iterator requirements
        nonvirtual	const KeyValue& operator*() const;
        nonvirtual	const KeyValue* operator->() const;
        nonvirtual	SkipListIterator&    operator++();

        // equal iff fCurrent are equal. This implies that all Done iterators are equal
        nonvirtual  bool    operator== (const SkipListIterator<KEY, VALUE, TRAITS>& rhs) const;
        nonvirtual  bool    operator!= (const SkipListIterator<KEY, VALUE, TRAITS>& rhs) const;

    protected:
        #if		qDebug
            nonvirtual	void	Invariant_ () const;
        #endif

	protected:
		const   SkipListNode*	fCurrent;
		bool    fSuppressMore;      // set at construction, and if you iterate outside of More call (using operator++)

        friend  class   SkipList<KEY, VALUE, TRAITS>;
};

template <typename KEY, typename VALUE,  typename TRAITS>
class   SkipListIterator_Patch : public SkipListIterator<KEY, VALUE, TRAITS>
{
     public:
        typedef typename  SkipList_Patch<KEY, VALUE, TRAITS>::Node  SkipListNode;
        typedef typename  SkipList<KEY, VALUE, TRAITS>::KeyValue    KeyValue;

    public:
        SkipListIterator_Patch ();    // required for forward iterator
        SkipListIterator_Patch (const SkipListIterator_Patch<KEY, VALUE, TRAITS>& from);
        SkipListIterator_Patch (SkipList_Patch<KEY, VALUE, TRAITS>& data, SkipListNode* node);

        ~SkipListIterator_Patch ();

        nonvirtual  SkipListIterator_Patch<KEY, VALUE, TRAITS>& operator= (const SkipListIterator_Patch<KEY, VALUE, TRAITS>& from);

		nonvirtual	bool	More (KeyValue* current = nullptr, bool advance = true);
        nonvirtual	SkipListIterator_Patch&    operator++();


        nonvirtual  void    RemovePatch (typename SkipList<KEY, VALUE, TRAITS>::Node* n);    // called before remove
        nonvirtual  void    RemoveAllPatch ();                                      // called before removeall

		nonvirtual	void	Invariant () const;

    protected:
         nonvirtual  void    RemoveFromChain ();

       #if		qDebug
            nonvirtual	void	Invariant_ () const;
        #endif

    protected:
        SkipList_Patch<KEY, VALUE, TRAITS>*   fData;
        SkipListIterator_Patch*   fNext;
		bool    fSuppressAdvance;   // set when patching

    private:
        typedef SkipListIterator<KEY, VALUE, TRAITS> inherited;

        friend  class   SkipList_Patch<KEY, VALUE, TRAITS>;
};


}   // namespace ADT

#include "SkipList.inl"
