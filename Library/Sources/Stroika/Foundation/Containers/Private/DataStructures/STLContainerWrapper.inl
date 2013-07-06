/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_DataStructures_STLContainerWrapper_inl_
#define _Stroika_Foundation_Containers_Private_DataStructures_STLContainerWrapper_inl_   1

#include    <algorithm>

#include    "../../../Debug/Assertions.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {
                namespace   DataStructures {


                    /*
                     ********************************************************************************
                     ******************* STLContainerWrapper<STL_CONTAINER_OF_T> ********************
                     ********************************************************************************
                     */
                    template    <typename STL_CONTAINER_OF_T>
                    inline  bool    STLContainerWrapper<STL_CONTAINER_OF_T>::Contains (value_type item) const
                    {
                        return this->find (item) != this->end ();
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    template <typename PREDICATE>
                    inline  bool    STLContainerWrapper<STL_CONTAINER_OF_T>::FindIf (PREDICATE pred) const
                    {
                        return std::find_if (this->begin (), this->end (), pred) != this->end ();
                    }


                    /*
                     ********************************************************************************
                     *********** STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator ***********
                     ********************************************************************************
                     */
                    template    <typename STL_CONTAINER_OF_T>
                    inline  STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::ForwardIterator (STLContainerWrapper<STL_CONTAINER_OF_T>* data)
                        : fData (data)
                        , fStdIterator (data->begin ())
                    {
                        RequireNotNull (data);
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::ForwardIterator (const ForwardIterator& from)
                        : fData (from.fData)
                        , fStdIterator (from.fStdIterator)
                    {
                        RequireNotNull (fData);
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  bool    STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::Done () const
                    {
                        AssertNotNull (fData);
                        return fStdIterator == fData->end ();
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    template    <typename VALUE_TYPE>
                    inline  bool    STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::More (VALUE_TYPE* current, bool advance)
                    {
                        bool    done    =   Done ();
                        if (advance) {
                            if (not done) {
                                fStdIterator++;
                                done = Done ();
                            }
                        }
                        if ((current != nullptr) and (not done)) {
                            *current = *fStdIterator;
                        }
                        return not done;
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  size_t    STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::CurrentIndex () const
                    {
                        AssertNotNull (fData);
                        return fStdIterator - fData->begin ();
                    }


                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Private_DataStructures_STLContainerWrapper_inl_ */
