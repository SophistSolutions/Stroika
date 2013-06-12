/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_PatchingDataStructures_STLContainerWrapper_h_
#define _Stroika_Foundation_Containers_Private_PatchingDataStructures_STLContainerWrapper_h_

#include    "../../../StroikaPreComp.h"

#include    "../DataStructures/STLContainerWrapper.h"



/**
 *  \file
 *
 *  TODO:
 *
 *  Description:
 *      This module genericly wraps STL containers (such as map, vector etc), and facilitates
 *      using them as backends for Stroika containers.
 *
 *  @todo   Redo Contains1 versus Contains using partial template specialization of STLContainerWrapper - easy
 *          cuz such a trivial class. I can use THAT trick to handle the case of forward_list too. And GetLength...
 *
 *  @todo   Add specail subclass of BasicForwardIterator that tracks PREVPTR - and use to cleanup stuff
 *          that uses forward_list code...
 *
 *  @todo   VERY INCOMPLETE Patch support. Unclear if/how I can do patch support generically - perhaps using
 *          some methods only called by array impls, and some only by returing iteratore on erase impls, etc,
 *          or perhaps with template specialization.
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {
                namespace   PatchingDataStructures {


                    /**
                     *  subclass of Foundation::Containers::STLContainerWrapper to support patching of owned iterators.
                     *
                     *  This code is NOT threadsafe. It assumes a wrapper layer provides thread safety, but it
                     *  DOES provide 'deletion'/update safety.
                     */
                    template    <typename STL_CONTAINER_OF_T>
                    class   STLContainerWrapper : public Foundation::Containers::Private::DataStructures::STLContainerWrapper<STL_CONTAINER_OF_T> {
                    private:
                        typedef Foundation::Containers::Private::DataStructures::STLContainerWrapper<STL_CONTAINER_OF_T>  inherited;

                    public:
                        typedef typename inherited::value_type value_type;

                    public:
                        STLContainerWrapper ();
                        STLContainerWrapper (const STLContainerWrapper<STL_CONTAINER_OF_T>& from);

                    public:
                        ~STLContainerWrapper ();

                    public:
                        nonvirtual  STLContainerWrapper<STL_CONTAINER_OF_T>& operator= (const STLContainerWrapper<STL_CONTAINER_OF_T>& rhs);

                    public:
                        class   BasicForwardIterator;

                    public:
                        /**
                         * Are there any iterators to be patched?
                         */
                        nonvirtual  bool    HasActiveIterators () const;

                    public:
                        /*
                         *  ONLY use for VECTOR types!
                         *
                         *  This updates ALL iterators based on prior and new offset.
                         */
                        template    <typename INSERT_VALUE_TYPE>
                        nonvirtual  void    insert_toVector_WithPatching (typename STL_CONTAINER_OF_T::iterator i, INSERT_VALUE_TYPE v);

                    public:
                        nonvirtual  void    erase_WithPatching (typename STL_CONTAINER_OF_T::iterator i);

                    public:
                        nonvirtual  void    clear_WithPatching ();

                    public:
                        /**
                         */
                        nonvirtual  void    TwoPhaseIteratorPatcherPass1 (typename STL_CONTAINER_OF_T::iterator oldI, Memory::SmallStackBuffer<BasicForwardIterator*>* items2Patch) const;
                        static      void    TwoPhaseIteratorPatcherPass2 (const Memory::SmallStackBuffer<BasicForwardIterator*>* items2Patch, typename STL_CONTAINER_OF_T::iterator newI);

                    public:
                        nonvirtual  void    Invariant () const;
#if     qDebug
                    protected:
                        nonvirtual  void    _Invariant () const;
#endif

                    private:
                        BasicForwardIterator*   fActiveIteratorsListHead_;

                    private:
                        friend  class   BasicForwardIterator;
                    };


                    /**
                     *      STLContainerWrapper::BasicForwardIterator is a private utility class designed
                     *  to promote source code sharing among the patched iterator implementations.
                     */
                    template    <typename STL_CONTAINER_OF_T>
                    class   STLContainerWrapper<STL_CONTAINER_OF_T>::BasicForwardIterator : public Foundation::Containers::Private::DataStructures::STLContainerWrapper<STL_CONTAINER_OF_T>::BasicForwardIterator {
                    private:
                        typedef typename Foundation::Containers::Private::DataStructures::STLContainerWrapper<STL_CONTAINER_OF_T>::BasicForwardIterator   inherited;

                    public:
                        typedef typename inherited::value_type value_type;

                    public:
                        typedef Foundation::Containers::Private::PatchingDataStructures::STLContainerWrapper<STL_CONTAINER_OF_T>   CONTAINER_TYPE;

                    public:
                        BasicForwardIterator (CONTAINER_TYPE* data);
                        BasicForwardIterator (const BasicForwardIterator& from);

                    public:
                        ~BasicForwardIterator ();

                    public:
                        nonvirtual  BasicForwardIterator& operator= (const BasicForwardIterator& rhs);

                    public:
                        template    <typename VALUE_TYPE>
                        nonvirtual  bool    More (VALUE_TYPE* current, bool advance);

                    public:
                        /**
                         */
                        nonvirtual  void    RemoveCurrent ();

                    private:
                        /*
                         * OK to be private cuz CONTAINER_TYPE is a friend.
                         */
                        nonvirtual  void    TwoPhaseIteratorPatcherPass1 (typename STL_CONTAINER_OF_T::iterator oldI, Memory::SmallStackBuffer<BasicForwardIterator*>* items2Patch);
                        nonvirtual  void    TwoPhaseIteratorPatcherPass2 (typename STL_CONTAINER_OF_T::iterator newI);

                    public:
                        nonvirtual  void    Invariant () const;
#if     qDebug
                    protected:
                        nonvirtual  void    _Invariant () const;
#endif

                    public:
                        CONTAINER_TYPE*         fData;
                        BasicForwardIterator*   fNextActiveIterator;
                        bool                    fSuppressMore;  // for removealls

                    private:
                        friend  CONTAINER_TYPE;
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

#endif  /*_Stroika_Foundation_Containers_Private_PatchingDataStructures_STLContainerWrapper_h_ */
