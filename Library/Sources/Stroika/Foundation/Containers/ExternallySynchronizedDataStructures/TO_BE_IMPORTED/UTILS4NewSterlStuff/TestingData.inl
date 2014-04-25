#include <stdlib.h>
#include <string>

#include <algorithm>

template <typename T>
void    GetDistributedIndex (const std::vector<T>& v, std::vector<size_t>* indices, DataDistribution dd)
{
    RequireNotNull (indices);
    indices->clear ();
    indices->reserve (v.size ());

    switch (dd) {
        case eUniformDist: {
                for (size_t i = 0; i < v.size (); ++i) {
                    indices->push_back (i);
                }
                std::random_shuffle (indices->begin(), indices->end());
            }
            break;
        case eNormalizedDist: {
                for (size_t i = 0; i < v.size (); ++i) {
                    indices->push_back (NormallyDistributedRandomNumber (0, v.size () - 1));
                }
            }
            break;
        case eZipfDist: {
                ZipfDistribution    zipfD (v.size ());
                std::mt19937&   eng = GetRandomNumberEngine ();
                for (size_t i = 0; i < v.size (); ++i) {
                    indices->push_back (zipfD (eng));
                }
            }
            break;
        default:
            AssertNotReached ();
    }
    Ensure (indices->size () == v.size ());
}


template    <>
inline  void    FillTestData (std::vector<size_t>& data, size_t elementsToAdd, bool offset)
{
    data.reserve (elementsToAdd);
    for (size_t i = 0; i < elementsToAdd; ++i) {
        data.push_back (ComputeUniqueValue (i, offset));
    }
}


template    <>
inline  void    FillTestData (std::vector<std::string>& data, size_t elementsToAdd, bool offset)
{
    data.reserve (elementsToAdd);
    for (size_t i = 0; i < elementsToAdd; ++i) {
        char buf [100];
        sprintf(buf, "        %lu", (unsigned long)ComputeUniqueValue (i, offset));
        data.push_back (std::string (buf));
    }
}


template <typename T>
TestSet<T>::TestSet (size_t elementsToAdd, bool scrambled)
{
    FillTestData<T> (fData, elementsToAdd, 0);
    FillTestData<T> (fMissing, elementsToAdd, 1);

    if (scrambled) {
        std::random_shuffle (fData.begin(), fData.end());
        std::random_shuffle (fMissing.begin(), fMissing.end());
    }
}

template <typename T>
const std::vector<T>&   TestSet<T>::GetData () const
{
    return fData;
}

template <typename T>
const std::vector<T>&   TestSet<T>::GetMissing () const
{
    return fMissing;
}

template <typename T>
const std::vector<size_t>&  TestSet<T>::GetUniformIndices ()
{
    if (fUniformIndices.size () == 0) {
        fUniformIndices.reserve (fData.size ());
        GetDistributedIndex (fData, &fUniformIndices, eUniformDist);
    }
    return fUniformIndices;
}

template <typename T>
const std::vector<size_t>&  TestSet<T>::GetNormalIndices ()
{
    if (fNormalIndices.size () == 0) {
        fNormalIndices.reserve (fData.size ());
        GetDistributedIndex (fData, &fNormalIndices, eNormalizedDist);
    }
    return fNormalIndices;
}

template <typename T>
const std::vector<size_t>&  TestSet<T>::GetZipfIndices ()
{
    if (fZipfIndices.size () == 0) {
        fZipfIndices.reserve (fData.size ());
        GetDistributedIndex (fData, &fZipfIndices, eZipfDist);
    }
    return fZipfIndices;
}

template <typename T>
const std::vector<size_t>&  TestSet<T>::GetIndices (DataDistribution dd)
{
    switch (dd) {
        case eUniformDist:
            return GetUniformIndices ();
        case eNormalizedDist:
            return GetNormalIndices ();
        case eZipfDist:
            return GetZipfIndices ();
        default:
            break;
    }
    AssertNotReached ();
    return GetUniformIndices ();
}

template <typename T>
const std::vector<size_t>&  TestSet<T>::GetMissingIndices ()
{
    if (fMissingIndices.size () == 0) {
        fMissingIndices.reserve (fData.size ());
        GetDistributedIndex (fMissing, &fMissingIndices, eUniformDist);
    }
    return fMissingIndices;
}

template <typename T>
void    TestSet<T>::PreLoad ()
{
    volatile    __attribute__((__unused__)) const std::vector<size_t>&  ignored1 = GetUniformIndices ();
    volatile    __attribute__((__unused__))const std::vector<size_t>&   ignored2 = GetNormalIndices ();
    volatile    __attribute__((__unused__))const std::vector<size_t>&   ignored3 = GetZipfIndices ();
    volatile    __attribute__((__unused__))const std::vector<size_t>&   ignored4 = GetMissingIndices ();
}
