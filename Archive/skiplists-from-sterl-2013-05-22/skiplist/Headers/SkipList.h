#pragma once

#include <vector>

#include "Comparer.h"
#include "Config.h"


/*
	From Wikipedia:
	A skip list is a data structure for storing a sorted list of items using a hierarchy of linked lists that connect increasingly sparse subsequences of the items. 
	These auxiliary lists allow item lookup with efficiency comparable to balanced binary search trees (that is, with number of probes proportional to log n instead of n).
	see http://en.wikipedia.org/wiki/Skip_list

	Finds are very comparison efficient in a skiplist. Add and remove require about twice as many comparisons as a find.
	SkipLists support fast forward iteration (linked list traversal).

	By setting SHOULDCOUNT to true, you can have a skiplist tally identical keys, rather than to multiple adds of identical keys.

	SkipLists are built probabilistically, but they can be optimized in a single pass. This reduces the total comparisons in a search by between 15 and 40%.
  
	Can currently set the probability of creating taller links. The lower the probability the more space efficient the SkipList, but at zero probability you 
	have a simple linked list, which will not perform well. We restrict the range to be between zero and .5. Empirical testing has suggested that .25 produces
	the best results. I haven't found a theoritic proof of this so I have left the in the ability to specify it. Here are the results for .25:
	
	SkipList of 10000 entries, with link creation probability of 0.25
	  total links = 13331; avg link height = 1.3331; max link height= 7
	  find avg comparisons = 14.9428; expected = 26.5754
	  find pointer comparisons = 11013; % = 7.3701%

   The "expected" above is from wikipedia, and is calculated as (log base 1/p n)/p.  It is disturbing that we perform so much lower than the calculation expects.
   The "pointer comparisons" above is a tweak where we avoid comparing keys where we can instead simply compare node pointers. We still count those comparisons 
   as in the average comparisons, but they are often much faster then comparing keys. 

*/


/*
	By default you store the key in a separate field. In cases where you can extract the key from the value you should
	use template specialization to make more space efficient. For example, a skiplist of strings, where the key and value are the
	same, can be supported as follows:

	template <>	
	struct	KeyValue_SkipList<string, string> {
		KeyValue_SkipList (string k,string v) : 
			fValue (v)  
			{
			}

			string	GetKey () const
			{
				return fValue;
			}

		string	fValue;
	};
*/
template <typename KEY, typename VALUE>
struct	KeyValue_SkipList {
	KeyValue_SkipList (KEY k, VALUE v);

	KEY	GetKey ()  const;

	KEY		fKey;
	VALUE	fValue;
};

/*
	When adding identical KeyValue, you can either add duplicate entries (the default behavior) or have the keys counted, which can be vastly more 
	efficient when there are many duplicates (SAT test scores for the nation). The Counter_SkipList drives the behavior. The default is to not tally
	items (SHOULDCOUNT=false), in which case a Counter_SkipList takes up no space, and does nothing but assert if methods are called. If SHOULDCOUNT
	is true, then it stores a count field, as well as the GetCount and ChangeCount methods.
*/
template <bool SHOULDCOUNT>
struct Counter_SkipList {
	size_t	GetCount () const 
	{ 
		AssertNotReached (); 
		return 0; 
	}
	size_t	ChangeCount (int inc) 
	{ 
		AssertNotReached ();
		return 0; 
	}
};




template <typename KEY, typename VALUE, typename COMP=Comparer<KEY>, bool SHOULDCOUNT=false >
class SkipList {
	public: 
		SkipList (double linkHeightProbability = 0.250);
		~SkipList ();

		/*
			Basic find operation. If pass in nullptr for val then only tests inclusion, otherwise fills val with value linked to key.
			In some cases (such as using a counter) you want full Node information rather than just the value -- see FindNode below for 
			how to do this.
		*/
		bool	Find (KEY key, VALUE* val = nullptr) const;

		/*
			You can add more than one item with the same key. If you add different values with the same key, it is unspecified which item will be returned on subsequent Find or Remove calls.
			If you set SHOULDCOUNT, then adding an item with the same key more than once means the total additions are counted, rather than added. Iteration over the links will show
			only one entry for each key in this case. It is an error to add two different values with the same key when SHOULDCOUNT
		*/
		void	Add (KEY key, VALUE val);

		void	Remove (KEY key);
		void	RemoveAll ();

 		size_t	GetLength () const;		// always equal to total Add minus total Remove
		size_t	GetKeyCount () const;	// usually this is same as GetLength, but when SHOULDCOUNT, it will be a <= number equal to amount of items you would iterate over

 		double	GetLinkHeightProbability ()  const;	// 0 <= chance <= .5;  Normally .5 or .25; 0 means singly linked list; 


		// calling this will result in maximal search performance until further adds or removes
		// call when list is relatively stable in size, and it will set links to classic log(n/2) search time
		// relatively fast to call, as is order N (single list traversal)
		enum { eLeaveProbabilityUnchanged = -1 };
		void	OptimizeLinks (double linkHeightProbability = eLeaveProbabilityUnchanged);	


		#if qSkipListDebug
			void	ListAll () const;
			void	ValidateAll () const;
		#endif

		#if qSkipListStatistics
			void	ComputeLinkHeightStats (double* linkHeight, size_t* maxLinkHeight);
		#endif

	public: 
		enum {
			kMaxLinkHeight = sizeof (size_t)*8,	
		};

		typedef	struct	Node {
			Node (KEY key, VALUE val);

			KeyValue_SkipList<KEY,VALUE>	fEntry;
			Counter_SkipList<SHOULDCOUNT>	fCounter;	// empty unless SHOULDCOUNT
			std::vector<Node*>				fNext;		// for a skiplist, you have an array of next pointers, rather than just one
		};
		std::vector<Node*>	fHead;

		/*
			Find node for key in skiplist, else nullptr. In cases of duplicate values, return first found unless SHOULDCOUNT, then return the
			unique node that supports item count (fCounter.GetCount)
		 */
		Node*	FindNode (KEY key)  const;

	private:
		void	ShrinkHeadLinksIfNeeded ();
		void	GrowHeadLinksIfNeeded (size_t newSize, Node* nodeToPointTo);
		size_t	DetermineLinkHeight ()  const;

		// used when doing a simple find, it returns the node matching the key
		Node*	FindLink (KEY key, std::vector<Node*>** startV, size_t linkIndex)  const;

		// these are specialized for the case of adding or removing elements, as they cache
		// all links that will need to be updated for the new element or the element to be removed
		Node*	FindNearest (KEY key, std::vector<Node*>* links)  const;
		Node*	CheckLink (KEY key, std::vector<Node*>* links, size_t linkIndex)  const;

		size_t	fLength;
		double	fLinkHeightProbability;

		Counter_SkipList<SHOULDCOUNT>	fKeyCount;	

		#if qSkipListStatistics
			public:
				size_t	fLinksChecked;
				size_t	fLinksPointerChecked;
		#endif
  };

#include "SkipList.inl"
