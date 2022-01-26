#pragma once

#include "../../Shared/Headers/Comparer.h"
#include "../../Shared/Headers/Exception.h"
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "../../Shared/Headers/KeyValue.h"

using namespace std;

#if qDebug



// the KEY parameter in CONTAINER must match the T parameter
// for example <MyTree<string, size_t>, string>
// is there a way to enforce that or better, to grab it from CONTAINER definition??
// runs BassicAddRemoveTests, RandomOrderAddRemoveTests, and OptimizeTests
// does NOT Run InvalidRemoveBehaviorTest or DuplicateAddBehaviorTest
template <typename CONTAINER, typename T>
void    RunSuite (size_t testDataLength, int tabs, bool verbose);


/*
    You can test different containers with the validation suite. But it assumes that it can create keys from size_t, as it
    loads data into containers in for loops:
    for (size_t i = 1; i <= 10; ++i) {
        myContainer.Add (key, data);
    }

    Because for testing the important component is the key, we make all the data be of type size_t, and simply load it with the value of i.
    For the key, we require that an appropriate ToKeyType be defined that maps size_t to the key type. So the for loop can read
    myContainer.Add (ToKeyType (i), i);
*/
template <typename T>
T  ToKeyType (size_t x);

template    <>
inline  size_t  ToKeyType (size_t x)
{
    return x;
}

template    <>
inline     string  ToKeyType (size_t x)
{
    char buf [100];
    sprintf(buf,"%lu",x);
    return string (buf);
}



template <typename CONTAINER, typename KEYTYPE>
void    BasicAddRemoveTests (bool verbose, int tabs, size_t testLength);

template <typename CONTAINER, typename KEYTYPE>
void    RandomOrderAddRemoveTests (bool verbose, int tabs, size_t testLength);

template <typename CONTAINER, typename KEYTYPE>
void    OptimizeTests (bool verbose, int tabs, size_t testLength);



template <typename CONTAINER, typename IGNOREBADREMOVECONTAINER>
void    InvalidRemoveBehaviorTest (bool verbose, int tabs);

template <typename CONTAINER, typename NODUPADDCONTAINER>
void    DuplicateAddBehaviorTest (size_t testLength, bool verbose, int tabs);

template <typename CONTAINER>
void    StringTraitOverrideTest (bool verbose, int tabs);

template <typename CONTAINER>
void    HashedStringTest (bool verbose, int tabs);


// trivial display utility

struct  TestTitle {
    TestTitle (const char* title, int tabs, bool verbose) ;
    ~TestTitle ();

    int     fTabs;
    bool    fVerbose;
};

struct	CaseInsensitiveCompare : public Comparer<string> {
    CaseInsensitiveCompare ()	{}

    static	int	Compare (const std::string& v1, const std::string& v2)
    {
        return strcasecmp (v1.c_str (), v2.c_str ());
    }
};

struct SharedStringKeyValue : public KeyValueInterface<string, string> {
    SharedStringKeyValue (const string& /*k*/,const string& v) :
        fValue (v)
        {
        }
    SharedStringKeyValue (string v) :
        fValue (v)
        {
        }
    SharedStringKeyValue (const char* v) :
        fValue (string (v))
        {
        }

    string	GetKey () const
    {
        return fValue;
    }
    string	GetValue () const
    {
        return fValue;
    }

    string	fValue;

};



// This should be moved to its own file if it is really needed. Didn't this used to exist in iomanip??
template<typename T, typename C>
class OManip {
    public:
        OManip (basic_ostream<C>& (*f) (basic_ostream<C>&, T), T v) :
            fFunction (f),
            fValue (v)
        {
        }

        void operator () (basic_ostream<C>& os) const
        {
            fFunction (os, fValue);
        }

    private:
       basic_ostream<C>& (*fFunction)  (basic_ostream<C>&, T);
       T fValue;
};

template<typename T, typename C>
basic_ostream<C>& operator<< (basic_ostream<C>& os,  const OManip<T, C>& manip)
{
   manip (os);
   return (os);
}


inline  OManip<int, char> tab (int n)
{
    auto    dotabs = [] (ostream& os, int count) -> ostream&
    {
        for (int i = 0; i < count; ++i) {
            os << '\t';
        }
        return os;
    };

   return (OManip<int, char> (dotabs, n));
}


#include "ContainerValidation.inl"

#endif
