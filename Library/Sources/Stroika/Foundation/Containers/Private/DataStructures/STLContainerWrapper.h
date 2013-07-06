/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_DataStructures_STLContainerWrapper_h_
#define _Stroika_Foundation_Containers_Private_DataStructures_STLContainerWrapper_h_

#include    "../../../StroikaPreComp.h"

#include    "../../../Configuration/Common.h"
#include    "../../../Memory/SmallStackBuffer.h"

#include    "../../Common.h"



/**
 *  \file
 *
 *  TODO:
 *
 *  Description:
 *      This module genericly wraps STL containers (such as map, vector etc), and facilitates
 *      using them as backends for Stroika containers.
 *
 *      @todo   Replace Contains() with Lookup () - as we did for LinkedList<T>
 *
 *      @todo   Redo Contains1 versus Contains using partial template specialization of STLContainerWrapper - easy
 *              cuz such a trivial class. I can use THAT trick to handle the case of forward_list too. And GetLength...
 *
 *      @todo   Add specail subclass of ForwardIterator that tracks PREVPTR - and use to cleanup stuff
 *              that uses forward_list code...
 *
 *      @todo   VERY INCOMPLETE Patch support. Unclear if/how I can do patch support generically - perhaps using
 *              some methods only called by array impls, and some only by returing iteratore on erase impls, etc,
 *              or perhaps with template specialization.
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {
                namespace   DataStructures {


                    /**
                     *  Use this to wrap an underlying stl container (like std::vector or stl:list, etc) to adapt
                     *  it for Stroika containers.
                     *
                     *  This code is NOT threadsafe. It assumes a wrapper layer provides thread safety, but it
                     *  DOES provide 'deletion'/update safety.
                     */
                    template    <typename STL_CONTAINER_OF_T>
                    class   STLContainerWrapper : public STL_CONTAINER_OF_T {
                    private:
                        typedef STL_CONTAINER_OF_T  inherited;

                    public:
                        typedef typename STL_CONTAINER_OF_T::value_type value_type;

                    public:
                        class   ForwardIterator;

                    public:
                        nonvirtual  bool    Contains (value_type item) const;

                    public:
                        template <typename PREDICATE>
                        nonvirtual  bool    FindIf (PREDICATE pred) const;
                    };


                    /**
                     *      STLContainerWrapper::ForwardIterator is a private utility class designed
                     *  to promote source code sharing among the patched iterator implementations.
                     */
                    template    <typename STL_CONTAINER_OF_T>
                    class   STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator {
                    public:
                        typedef STLContainerWrapper<STL_CONTAINER_OF_T>  CONTAINER_TYPE;

                    public:
                        typedef typename STLContainerWrapper<STL_CONTAINER_OF_T>::value_type value_type;

                    public:
                        explicit ForwardIterator (CONTAINER_TYPE* data);
                        explicit ForwardIterator (const ForwardIterator& from);

                    public:
                        nonvirtual  bool    Done () const;

                    public:
                        template    <typename VALUE_TYPE>
                        nonvirtual  bool    More (VALUE_TYPE* current, bool advance);

                    public:
                        /**
                         * Only legal to call if underlying iterator is random_access
                         */
                        nonvirtual  size_t  CurrentIndex () const;

                    public:
                        /**
                         */
                        nonvirtual  void    RemoveCurrent ();

                    public:
                        CONTAINER_TYPE*                     fData;
                        typename CONTAINER_TYPE::iterator   fStdIterator;
                    };


                }
            }
        }
    }
}


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "STLContainerWrapper.inl"

#endif  /*_Stroika_Foundation_Containers_Private_DataStructures_STLContainerWrapper_h_ */
