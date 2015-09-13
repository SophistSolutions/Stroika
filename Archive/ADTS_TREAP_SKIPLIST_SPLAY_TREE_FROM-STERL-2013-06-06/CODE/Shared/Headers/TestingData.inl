#include <stdlib.h>
#include <string>

#include <algorithm>


template	<>
inline	void	FillTestData (std::vector<size_t>& data, size_t elementsToAdd, bool offset)
{
	data.reserve (elementsToAdd);
	for (size_t i = 0; i < elementsToAdd; ++i) {
		data.push_back (ComputeUniqueValue (i, offset));
	}
}


template	<>
inline	void	FillTestData (std::vector<std::string>& data, size_t elementsToAdd, bool offset)
{
	data.reserve (elementsToAdd);
	for (size_t i = 0; i < elementsToAdd; ++i) {
		char buf [100];
        sprintf(buf,"        %lu",(unsigned long)ComputeUniqueValue (i, offset));
        data.push_back (std::string (buf));
    }
}


template <typename T>
TestSet<T>::TestSet (size_t elementsToAdd) :
    fLength (elementsToAdd)
{
	FillTestData<T> (fData, elementsToAdd, 0);
	FillTestData<T> (fMissing, elementsToAdd, 1);

    fOrderedIndices.reserve (fData.size ());
    for (size_t i = 0; i < fData.size (); ++i) {
        fOrderedIndices.push_back (i);
    }

    fScrambledIndices = fOrderedIndices;
    std::random_shuffle (fScrambledIndices.begin(), fScrambledIndices.end());

    fUniformIndices.reserve (fData.size ());
    GetDistributedIndex (fData, &fUniformIndices, eUniformDist);

    fNormalIndices.reserve (fData.size ());
    GetDistributedIndex (fData, &fNormalIndices, eNormalizedDist);

    fZipfIndices.reserve (fData.size ());
    GetDistributedIndex (fData, &fZipfIndices, eZipfDist);
}

template <typename T>
size_t	TestSet<T>::GetLength () const
{
	return fLength;
}


template <typename T>
const std::vector<T>&	TestSet<T>::GetData () const
{
	return fData;
}

template <typename T>
const std::vector<T>&	TestSet<T>::GetMissing () const
{
	return fMissing;
}


template <typename T>
void	GetDistributedIndex (const std::vector<T>& v, std::vector<size_t>* indices, DataDistribution dd)
{
	RequireNotNull (indices);
	indices->clear ();
	indices->reserve (v.size ());

	switch (dd) {
		case eUniformDist:
			{
				for (size_t i = 0; i < v.size (); ++i) {
					indices->push_back (i);
				}
				std::random_shuffle (indices->begin(), indices->end());
			}
			break;
		case eNormalizedDist:
			{
				for (size_t i = 0; i < v.size (); ++i) {
					indices->push_back (NormallyDistributedRandomNumber (0, v.size ()-1));
				}
			}
			break;
		case eZipfDist:
			{
				ZipfDistribution	zipfD (v.size ());
				std::mt19937&	eng = GetRandomNumberEngine ();
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

template <typename T>
const std::vector<size_t>&	TestSet<T>::GetOrderedIndices () const
{
	Ensure (fOrderedIndices.size () == fData.size ());
	return fOrderedIndices;
}

template <typename T>
const std::vector<size_t>&	TestSet<T>::GetScrambledIndices () const
{
    Ensure (fScrambledIndices.size () == fData.size ());
    return fScrambledIndices;
}

template <typename T>
const T&  TestSet<T>::GetData (size_t index, DataDistribution dd) const
{
    Require (index < fData.size ());

    switch (dd) {
        case eUniformDist:
            break;
        case eNormalizedDist:
            index = fNormalIndices[index];
            break;
        case eZipfDist:
            index = fZipfIndices[index];
            break;
        default:
            AssertNotReached ();
    }
    Assert (index < fUniformIndices.size ());
    Assert (fUniformIndices[index] < fData.size ());
    return fData[fUniformIndices[index]];
}

template <typename T>
const T&  TestSet<T>::GetMissing (size_t index, DataDistribution dd) const
{
    Require (index < fData.size ());

    switch (dd) {
        case eUniformDist:
            break;
        case eNormalizedDist:
            index = fNormalIndices[index];
            break;
        case eZipfDist:
            index = fZipfIndices[index];
            break;
        default:
            AssertNotReached ();
    }
    Assert (index < fUniformIndices.size ());
    Assert (fUniformIndices[index] < fMissing.size ());
    return fMissing[fUniformIndices[index]];
}
