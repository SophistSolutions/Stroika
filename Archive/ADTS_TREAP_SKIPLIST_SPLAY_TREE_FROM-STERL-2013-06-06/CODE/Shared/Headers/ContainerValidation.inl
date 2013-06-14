#include <algorithm>

#include "../../Shared/Headers/HashKey.h"
#include "../Headers/ADT.h"

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

    Assert (t.GetFirst () == nullptr);
    Assert (t.GetLast () == nullptr);

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
        t.ValidateAll ();
        int comp = Comparer<KEYTYPE>::Compare (biggestKey, key);
        if (i == 1 or comp < 0) {
            biggestKey = key;
        }
    }
    Assert (t.GetFirst () != nullptr and t.GetFirst ()->fEntry.GetValue () == 1);
    Assert (t.GetLast () != nullptr and (Comparer<KEYTYPE>::Compare (t.GetLast ()->fEntry.GetKey (), biggestKey) == 0));

    for (size_t i = 1; i <= testLength; ++i) {
        KEYTYPE  key = ToKeyType<KEYTYPE> (i);
        Assert (t.Find (key, &val) and (val == i));
        t.Remove (key);
        Assert (not t.Find (key));
        Assert (t.GetLength () == testLength-i);
        t.ValidateAll ();
    }
    Assert (t.GetLength () == 0);
    Assert (t.GetFirst () == nullptr);
    Assert (t.GetLast () == nullptr);

    if (verbose) {
        cout << tab (tabs+1) << "Add and remove " << testLength << " items, backwards direction" << endl;
    }

    for (size_t i = testLength; i >= 1; --i) {
        KEYTYPE  key = ToKeyType<KEYTYPE> (i);
        Assert (not t.Find (key));
        t.Add (key, i);
        Assert (t.GetLength () == testLength-i+1);
        Assert (t.Find (key, &val) and (val == i));
        t.ValidateAll ();

        int comp = Comparer<KEYTYPE>::Compare (biggestKey, key);
        if (i == testLength or comp < 0) {
            biggestKey = key;
        }
    }
    Assert (t.GetFirst () != nullptr and t.GetFirst ()->fEntry.GetValue () == 1);
    Assert (t.GetLast () != nullptr and (Comparer<KEYTYPE>::Compare (t.GetLast ()->fEntry.GetKey (), biggestKey) == 0));

    for (size_t i = testLength; i >= 1; --i) {
        KEYTYPE  key = ToKeyType<KEYTYPE> (i);
        Assert (t.Find (key, &val) and (val == i));
        t.Remove (key);
        Assert (not t.Find (key));
        Assert (t.GetLength () == i-1);
        t.ValidateAll ();
    }
    Assert (t.GetLength () == 0);
    Assert (t.GetFirst () == nullptr);
    Assert (t.GetLast () == nullptr);
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
        Assert (t.GetFirst () == nullptr);
        Assert (t.GetLast () == nullptr);

        for (size_t i = 1; i <= testLength; ++i) {
            t.Add (1, i);
            Assert (t.GetLength () == i);
            Assert (t.Find (1));
            t.ValidateAll ();
        }
        Assert (t.GetFirst () != nullptr and t.GetFirst ()->fEntry.GetKey () == 1);
        Assert (t.GetLast () != nullptr and t.GetLast ()->fEntry.GetKey () == 1);
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
    t.ValidateAll ();
    Assert (t.GetFirst () != nullptr and (Comparer<KEYTYPE>::Compare (t.GetFirst ()->fEntry.GetKey (), smallestKey) == 0));
    Assert (t.GetLast () != nullptr and (Comparer<KEYTYPE>::Compare (t.GetLast ()->fEntry.GetKey (), biggestKey) == 0));

    t.ReBalance ();

    t.ValidateAll ();
    for (size_t i = 0; i <= testLength-1; ++i) {
        KEYTYPE  key = ToKeyType<KEYTYPE> (data[i]);
        Assert (t.Find (key, &val) and (val == i));
    }
    Assert (t.GetFirst () != nullptr and (Comparer<KEYTYPE>::Compare (t.GetFirst ()->fEntry.GetKey (), smallestKey) == 0));
    Assert (t.GetLast () != nullptr and (Comparer<KEYTYPE>::Compare (t.GetLast ()->fEntry.GetKey (), biggestKey) == 0));
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

    Assert (t.GetFirst () == nullptr);
    Assert (t.GetLast () == nullptr);
    KEYTYPE biggestKey;
    KEYTYPE smallestKey;
    for (size_t i = 0; i < data.size (); ++i) {
        KEYTYPE  key = ToKeyType<KEYTYPE> (data[i]);
        Assert (not t.Find (key));
        t.Add (key, i);
        Assert (t.GetLength () == i+1);
        Assert (t.Find (key, &val) and (val == i));
        t.ValidateAll ();
        int comp = Comparer<KEYTYPE>::Compare (biggestKey, key);
        if (i == 0 or comp < 0) {
            biggestKey = key;
        }
        comp = Comparer<KEYTYPE>::Compare (smallestKey, key);
        if (i == 0 or comp > 0) {
            smallestKey = key;
        }
    }
    Assert (t.GetFirst () != nullptr and (Comparer<KEYTYPE>::Compare (t.GetFirst ()->fEntry.GetKey (), smallestKey) == 0));
    Assert (t.GetLast () != nullptr and (Comparer<KEYTYPE>::Compare (t.GetLast ()->fEntry.GetKey (), biggestKey) == 0));

    std::random_shuffle (data.begin (), data.end ());
    for (size_t i = 0; i <= testLength-1; ++i) {
        KEYTYPE  v = ToKeyType<KEYTYPE> (data[i]);
        Assert (t.Find (v));
        t.Remove (v);
        Assert (not t.Find (v));
        Assert (t.GetLength () == testLength-i-1);
        t.ValidateAll ();
    }
    Assert (t.GetLength () == 0);
    Assert (t.GetFirst () == nullptr);
    Assert (t.GetLast () == nullptr);
}


template <typename CONTAINER>
void    StringTraitOverrideTest (bool verbose, int tabs)
{
    TestTitle   tt ("Testing deriving key from value and overriding compare", tabs, verbose);

    CONTAINER   t;

    t.Add ("entry 1");
    t.Add ("Entry 2");
    t.ValidateAll ();

    string val;
    Assert (t.Find ("entry 1", &val) and (val == "entry 1"));
    Assert (t.GetFirst ()->fEntry.GetValue () == string ("entry 1"));
    Assert (t.GetLast ()->fEntry.GetValue () == string ("Entry 2"));
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
    t.ValidateAll ();
    string val;
    Assert (Hash ("entry 1") == Hash ("entry 1"));
    Assert (t.Find (string ("entry 1"), &val) and (val == "entry 1"));
}
