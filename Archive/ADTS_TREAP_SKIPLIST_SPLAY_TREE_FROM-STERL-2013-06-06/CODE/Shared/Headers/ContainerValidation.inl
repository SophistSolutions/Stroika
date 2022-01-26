#include <algorithm>
#include <sstream>
#include <string>

#include "../../Shared/Headers/HashKey.h"
#include "../Headers/ADT.h"
#include "../Headers/BinaryTree.h"

// turn off while moving to iterators
#define qTestFirstLast  0

template <typename CONTAINER, typename T>
void    RunSuite (size_t testDataLength, int tabs, bool verbose)
{
    BasicAddRemoveTests<CONTAINER, T> (verbose, tabs+1, testDataLength);
    RandomOrderAddRemoveTests<CONTAINER, T> (verbose, tabs+1, testDataLength);
    OptimizeTests<CONTAINER, T> (verbose, tabs+1, testDataLength);
}

template <typename CONTAINER, typename KEYTYPE>
void    BasicAddRemoveTests (bool verbose, int tabs, size_t testLength)
{
    CONTAINER t;

    TestTitle   tt ("Simple Add and Remove Tests", tabs, verbose);
#if qTestFirstLast
    Assert (t.GetFirst () == nullptr);
    Assert (t.GetLast () == nullptr);
#endif
    if (verbose) {
        cout << tab (tabs+1) << "Add and remove " << testLength << " items, forward direction" << endl;
    }

    size_t  val = 0;
    KEYTYPE biggestKey;
    for (size_t i = 1; i <= testLength; ++i) {
        KEYTYPE  key = ToKeyType<KEYTYPE> (i);
        Assert (not t.Find (key));
        t.Add (key, i);
        Assert (t.GetLength () == i);
        Assert (t.Find (key, &val) and (val == i));
        t.Invariant ();
        int comp = Comparer<KEYTYPE>::Compare (biggestKey, key);
        if (i == 1 or comp < 0) {
            biggestKey = key;
        }
    }
#if qTestFirstLast
    Assert (t.GetFirst () != nullptr and t.GetFirst ()->fEntry.GetValue () == 1);
    Assert (t.GetLast () != nullptr and (Comparer<KEYTYPE>::Compare (t.GetLast ()->fEntry.GetKey (), biggestKey) == 0));
#endif
    for (size_t i = 1; i <= testLength; ++i) {
        KEYTYPE  key = ToKeyType<KEYTYPE> (i);
        Assert (t.Find (key, &val) and (val == i));
        t.Remove (key);
        Assert (not t.Find (key));
        Assert (t.GetLength () == testLength-i);
        t.Invariant ();
    }
    Assert (t.GetLength () == 0);
#if qTestFirstLast
    Assert (t.GetFirst () == nullptr);
    Assert (t.GetLast () == nullptr);
#endif
    if (verbose) {
        cout << tab (tabs+1) << "Add and remove " << testLength << " items, backwards direction" << endl;
    }

    for (size_t i = testLength; i >= 1; --i) {
        KEYTYPE  key = ToKeyType<KEYTYPE> (i);
        Assert (not t.Find (key));
        t.Add (key, i);
        Assert (t.GetLength () == testLength-i+1);
        Assert (t.Find (key, &val) and (val == i));
        t.Invariant ();

        int comp = Comparer<KEYTYPE>::Compare (biggestKey, key);
        if (i == testLength or comp < 0) {
            biggestKey = key;
        }
    }
#if qTestFirstLast
    Assert (t.GetFirst () != nullptr and t.GetFirst ()->fEntry.GetValue () == 1);
    Assert (t.GetLast () != nullptr and (Comparer<KEYTYPE>::Compare (t.GetLast ()->fEntry.GetKey (), biggestKey) == 0));
#endif
    for (size_t i = testLength; i >= 1; --i) {
        KEYTYPE  key = ToKeyType<KEYTYPE> (i);
        Assert (t.Find (key, &val) and (val == i));
        t.Remove (key);
        Assert (not t.Find (key));
        Assert (t.GetLength () == i-1);
        t.Invariant ();
    }
    Assert (t.GetLength () == 0);
#if qTestFirstLast
    Assert (t.GetFirst () == nullptr);
    Assert (t.GetLast () == nullptr);
#endif
}

template <typename CONTAINER, typename IGNOREBADREMOVECONTAINER>
void    InvalidRemoveBehaviorTest (bool verbose, int tabs)
{
     TestTitle   tt ("Invalid Remove Tests", tabs, verbose);
    {
        TestTitle   t1 ("Invalid Remove Throwing Exception Test", tabs+1, verbose);

        CONTAINER   t;

        bool    gotException = false;
        try {
            t.Remove (1);
        }
        catch (const InvalidRemovalException& exp) {
            gotException = true;
        }
        Assert (gotException);
    }

    {
        TestTitle   t2 ("Invalid Remove Ignored Test", tabs+1, verbose);
        IGNOREBADREMOVECONTAINER   t;
        bool    gotException = false;
        try {
            t.Remove (1);
        }
        catch (const DuplicateAddException& exp) {
            gotException = true;
        }
        Assert (not gotException);
    }
}

template <typename CONTAINER, typename NODUPADDCONTAINER>
void    DuplicateAddBehaviorTest (size_t testLength, bool verbose, int tabs)
{
    TestTitle   tt ("Duplicate Add Tests", tabs, verbose);

    {
        TestTitle   t1 ("Duplicate Add Allowed Test", tabs+1, verbose);

        CONTAINER   t;
        if (verbose) {
            cout << tab (tabs+2) << "Adding " << testLength << " items, all with same value" << endl;
        }
#if qTestFirstLast
        Assert (t.GetFirst () == nullptr);
        Assert (t.GetLast () == nullptr);
#endif

        for (size_t i = 1; i <= testLength; ++i) {
            t.Add (1, i);
            Assert (t.GetLength () == i);
            Assert (t.Find (1));
            t.Invariant ();
        }
#if qTestFirstLast
        Assert (t.GetFirst () != nullptr and t.GetFirst ()->fEntry.GetKey () == 1);
        Assert (t.GetLast () != nullptr and t.GetLast ()->fEntry.GetKey () == 1);
#endif
    }
    {
        TestTitle   t1 ("Duplicate Add Throwing Exception Test", tabs+1, verbose);
        NODUPADDCONTAINER   nda;
        bool    gotException = false;
        nda.Add (1, 1);
        try {
            nda.Add (1, 1);
        }
        catch (const DuplicateAddException& exp) {
            gotException = true;
        }
        Assert (gotException);
    }
}

template <typename CONTAINER, typename KEYTYPE>
void    OptimizeTests (bool verbose, int tabs, size_t testLength)
{
    CONTAINER   t;

    TestTitle   tt ("Balance Tests", tabs, verbose);

    vector<size_t>  data;
    data.reserve (testLength);
    for (size_t i = 0; i < testLength; ++i) {
        data.push_back (i);
    }
    std::random_shuffle (data.begin (), data.end ());

    size_t  val = 0;
    KEYTYPE biggestKey;
    KEYTYPE smallestKey;
    for (size_t i = 0; i < data.size (); ++i) {
        KEYTYPE  key = ToKeyType<KEYTYPE> (data[i]);
        t.Add (key, i);
        int comp = Comparer<KEYTYPE>::Compare (biggestKey, key);
        if (i == 0 or comp < 0) {
            biggestKey = key;
        }
        comp = Comparer<KEYTYPE>::Compare (smallestKey, key);
        if (i == 0 or comp > 0) {
            smallestKey = key;
        }
    }
    Assert (t.GetLength () == testLength);
    t.Invariant ();
#if qTestFirstLast
    Assert (t.GetFirst () != nullptr and (Comparer<KEYTYPE>::Compare (t.GetFirst ()->fEntry.GetKey (), smallestKey) == 0));
    Assert (t.GetLast () != nullptr and (Comparer<KEYTYPE>::Compare (t.GetLast ()->fEntry.GetKey (), biggestKey) == 0));
#endif
    t.Balance ();

    t.Invariant ();
    for (size_t i = 0; i <= testLength-1; ++i) {
        KEYTYPE  key = ToKeyType<KEYTYPE> (data[i]);
        Assert (t.Find (key, &val) and (val == i));
    }
#if qTestFirstLast
    Assert (t.GetFirst () != nullptr and (Comparer<KEYTYPE>::Compare (t.GetFirst ()->fEntry.GetKey (), smallestKey) == 0));
    Assert (t.GetLast () != nullptr and (Comparer<KEYTYPE>::Compare (t.GetLast ()->fEntry.GetKey (), biggestKey) == 0));
#endif
}

template <typename CONTAINER, typename KEYTYPE>
void    RandomOrderAddRemoveTests (bool verbose, int tabs, size_t testLength)
{
    CONTAINER   t;

    TestTitle   tt ("Random Order Add and Remove Tests", tabs, verbose);

    vector<size_t>  data;
    data.reserve (testLength);
    for (size_t i = 0; i < testLength; ++i) {
        data.push_back (i);
    }
    std::random_shuffle (data.begin (), data.end ());
    size_t  val = 0;

    if (verbose) {
        cout << tab (tabs+1) << "Adding and Remove " << testLength << " items, in random order" << endl;
    }
#if qTestFirstLast
    Assert (t.GetFirst () == nullptr);
    Assert (t.GetLast () == nullptr);
#endif
    KEYTYPE biggestKey;
    KEYTYPE smallestKey;
    for (size_t i = 0; i < data.size (); ++i) {
        KEYTYPE  key = ToKeyType<KEYTYPE> (data[i]);
        Assert (not t.Find (key));
        t.Add (key, i);
        Assert (t.GetLength () == i+1);
        Assert (t.Find (key, &val) and (val == i));
        t.Invariant ();
        int comp = Comparer<KEYTYPE>::Compare (biggestKey, key);
        if (i == 0 or comp < 0) {
            biggestKey = key;
        }
        comp = Comparer<KEYTYPE>::Compare (smallestKey, key);
        if (i == 0 or comp > 0) {
            smallestKey = key;
        }
    }
#if qTestFirstLast
    Assert (t.GetFirst () != nullptr and (Comparer<KEYTYPE>::Compare (t.GetFirst ()->fEntry.GetKey (), smallestKey) == 0));
    Assert (t.GetLast () != nullptr and (Comparer<KEYTYPE>::Compare (t.GetLast ()->fEntry.GetKey (), biggestKey) == 0));
#endif
    std::random_shuffle (data.begin (), data.end ());
    for (size_t i = 0; i <= testLength-1; ++i) {
        KEYTYPE  v = ToKeyType<KEYTYPE> (data[i]);
        Assert (t.Find (v));
        t.Remove (v);
        Assert (not t.Find (v));
        Assert (t.GetLength () == testLength-i-1);
        t.Invariant ();
    }
    Assert (t.GetLength () == 0);
#if qTestFirstLast
    Assert (t.GetFirst () == nullptr);
    Assert (t.GetLast () == nullptr);
#endif
}


template <typename CONTAINER>
void    StringTraitOverrideTest (bool verbose, int tabs)
{
    TestTitle   tt ("Testing deriving key from value and overriding compare", tabs, verbose);

    CONTAINER   t;

    t.Add ("entry 1");
    t.Add ("Entry 2");
    t.Invariant ();

    string val;
    Assert (t.Find ("entry 1", &val) and (val == "entry 1"));
#if qTestFirstLast
    Assert (t.GetFirst ()->fEntry.GetValue () == string ("entry 1"));
    Assert (t.GetLast ()->fEntry.GetValue () == string ("Entry 2"));
#endif
}

template <typename CONTAINER>
void    HashedStringTest (bool verbose, int tabs)
{
    class MyContainer : public CONTAINER {
        public:
            MyContainer () {}
            nonvirtual	void	Add (const string& s)
            {
                inherited::Add (HashKey<string> (s), s);
            }
        private:
            typedef	CONTAINER	inherited;
    } t;

    TestTitle   tt ("Testing hashed string key", tabs, verbose);
    t.Add ("entry 1");
    t.Invariant ();
    string val;
    Assert (Hash ("entry 1") == Hash ("entry 1"));
    Assert (t.Find (string ("entry 1"), &val) and (val == "entry 1"));
}


struct  CoutToString {
    CoutToString () :
        fOld (std::cout.rdbuf ())
    {
        std::cout.rdbuf (fStream.rdbuf ());
    }

    std::string     GrabContents ()
    {
        std::string     s = fStream.str ();
        fStream.clear ();
        return s;
    }

    ~CoutToString ()
    {
        std::cout.rdbuf (fOld);
    }
    std::ostringstream  fStream;
    std::streambuf*     fOld;
};

template <typename CONTAINER>
void    SimpleIteratorTest (size_t testLength,  bool verbose, int tabs)
{
    CONTAINER   t;

    using namespace ADT::BinaryTree;

    auto    GetDisplayName = [] (TraversalType tt, Direction d) -> string
    {
        string  s;
        switch (tt) {
            case TraversalType::kPreOrder:      s += "PreOrder ";       break;
            case TraversalType::kInOrder:       s += "InOrder ";        break;
            case TraversalType::kPostOrder:     s += "PostOrder ";      break;
            case TraversalType::kBreadthFirst:  s += "BreadthFirst ";   break;
            default:    AssertNotReached ();
        }
        switch (d) {
            case Direction::kLeft:  s += "Left";  break;
            case Direction::kRight: s += "Right"; break;
            default: AssertNotReached ();
        }
        return s;
    };

    TestTitle   title ("Simple Tree Iterator Tests", tabs, verbose);

    vector<typename CONTAINER::KeyType> keys;
    for (size_t i = 1; i <= testLength; ++i) {
        keys.push_back (i);
    }
    std::random_shuffle (keys.begin(), keys.end());

    for (auto it : keys) {
        t.Add (it);
    }

#if 1   // make a separate test suite for Find and Apply
const typename CONTAINER::Node* xxx = CONTAINER::Node::Find (t.Iterate (TraversalType::kPreOrder).GetNode (), TraversalType::kInOrder, Direction::kLeft,
                             [] (const typename CONTAINER::Node& n)->bool { return (n.fEntry.GetKey () == 4); }
                            );
AssertNotNull (xxx);
Assert (xxx->fEntry.GetValue () == 4);
#endif



    for (TraversalType tt : { TraversalType::kPreOrder, TraversalType::kInOrder, TraversalType::kPostOrder, TraversalType::kBreadthFirst }) {
        for (Direction d : { Direction::kLeft, Direction::kRight }) {
            if (verbose) {
               cout << tab (tabs) << "*** " << GetDisplayName (tt, d).c_str () << " ***" << endl;
             }
            string  itListing;
            {
                CoutToString cs;
                t.ListAll (t.Iterate (tt, d));
                itListing = cs.GrabContents ();
            }
            string  nodeListing;
            {
                CoutToString cs;
                std::cout << "[";

                CONTAINER::Node::Apply (t.Iterate (TraversalType::kPreOrder).GetNode (), tt, d,
                    [] (const typename CONTAINER::Node& n) { n.Inspect (); }
                );

                std::cout << "]" << std::endl;
                nodeListing = cs.GrabContents ();
            }
            if (nodeListing != itListing and verbose) {
                cout << tab (tabs) << "FAILED" << endl << itListing.c_str () << endl << nodeListing.c_str () << endl;
            }
            else if (verbose) {
                cout << tab (tabs) << "PASSED" << endl;
            }

            Assert (itListing == nodeListing);


            if (verbose) {
                cout << endl;
            }
        }
    }
}


template <typename CONTAINER>
void    PatchingIteratorTest (size_t testLength,  bool verbose, int tabs)
{
    CONTAINER   t;

    TestTitle   tt ("Patching Tree Iterator Tests", tabs, verbose);

    vector<typename CONTAINER::KeyType> keys;
    for (size_t i = 1; i <= testLength; ++i) {
        keys.push_back (i);
    }
    std::random_shuffle (keys.begin(), keys.end());

    for (auto it : keys) {
        t.Add (it);
    }


//cout << endl << endl;
        auto it = t.Iterate ();
//cout << tab (tabs) << "(" << it->GetKey () << ", " << it->GetValue () << ")" << endl;
        t.Remove (it);
//cout << endl << tab (tabs) << "(" << it->GetKey () << ", " << it->GetValue () << ")" << endl;
        t.Update (it, 99);
//cout << endl << tab (tabs) << "(" << it->GetKey () << ", " << it->GetValue ()  << ")" << endl;


++it;
//cout << endl << tab (tabs) << "(" << it->GetKey () << ", " << it->GetValue ()  << ")" << endl;
++it;
//cout << endl << tab (tabs) << "(" << it->GetKey () << ", " << it->GetValue () << ")" << endl;
//cout << tab (tabs);
         for (; it.More (); ) {
//cout << "(" << it->GetKey () << ", " << it->GetValue () << ")";
        }
//cout << endl  << tab (tabs);
        for (auto it1 = t.Iterate (5); it1.More () and (it1->GetKey () < 11); ) {
//cout << "(" << it1->GetKey () << ", " << it1->GetValue () << ")";
        }
//cout << endl;

}
