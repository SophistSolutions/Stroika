/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_PatchingDataStructures_STLContainerWrapper_h_
#define _Stroika_Foundation_Containers_Private_PatchingDataStructures_STLContainerWrapper_h_

#include    "../../../StroikaPreComp.h"

#include    "../DataStructures/STLContainerWrapper.h"

#include    "PatchableContainerHelper.h"



/**
 *  \file
 *
 *  TODO:
 *
 *  Description:
 *      This module genericly wraps STL containers (such as map, vector etc), and facilitates
 *      using them as backends for Stroika containers.
 *
 *  @todo   Add special subclass of ForwardIterator that tracks PREVPTR - and use to cleanup stuff
 *          that uses forward_list code...
 *
 *  @todo   VERY INCOMPLETE Patch support. Unclear if/how I can do patch support generically - perhaps using
 *          some methods only called by array impls, and some only by returing iteratore on erase impls, etc,
 *          or perhaps with template specialization.
 *
 *  @todo   Consider if this patching code - could be used to wrap/apply (most functionality) to
 *          All the contianers? Migrate this 2 stage patch strategy to PatchableContainerHelper???
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {
                namespace   PatchingDataStructures {


                    /**
                     *  Patching Support:
                     *
                     *      This class wraps a basic container (in this case DataStructures::LinkedList)
                     *  and adds in patching support (tracking a list of iterators - and managing thier
                     *  patching when appropriately wrapped changes are made to the data structure container.
                     *
                     *      This code leverages PatchableContainerHelper<> to do alot of the book-keeping.
                     *
                     *      This code is NOT threadsafe. It assumes a wrapper layer provides thread safety, but it
                     *  DOES provide 'deletion'/update safety.
                     *
                     *      Note: Disallow X(const X&), and operator=() (copy constructor/assignement operator), and
                     *  instead require use of X(X*,IteratorOwnerID) for copying - so we always get both values -
                     *  the source to copy from and the newOwnerID to copy INTO.
                     */
                    template    <typename STL_CONTAINER_OF_T, typename LOCKER>
                    class   STLContainerWrapper : public PatchableContainerHelper<DataStructures::STLContainerWrapper<STL_CONTAINER_OF_T>, LOCKER> {
                    private:
                        using   inherited   =   PatchableContainerHelper<DataStructures::STLContainerWrapper<STL_CONTAINER_OF_T>, LOCKER>;

                    public:
                        using   value_type  =   typename inherited::value_type;

                    public:
                        STLContainerWrapper ();
                        STLContainerWrapper (const STLContainerWrapper&) = delete;
                        STLContainerWrapper (STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>* rhs, IteratorOwnerID newOwnerID);

                    public:
                        nonvirtual  STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>& operator= (const STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>& rhs) = delete;

                    public:
                        class   ForwardIterator;

                    public:
                        using   UnpatchedForwardIterator = typename inherited::ForwardIterator;

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
                        nonvirtual  void    TwoPhaseIteratorPatcherPass1 (typename STL_CONTAINER_OF_T::iterator oldI, Memory::SmallStackBuffer<ForwardIterator*>* items2Patch) const;
                        static      void    TwoPhaseIteratorPatcherPass2 (const Memory::SmallStackBuffer<ForwardIterator*>* items2Patch, typename STL_CONTAINER_OF_T::iterator newI);

                    public:
                        /**
                         */
                        nonvirtual  void    TwoPhaseIteratorPatcherAll2FromOffsetsPass1 (Memory::SmallStackBuffer<size_t>* patchOffsets) const;
                        nonvirtual  void    TwoPhaseIteratorPatcherAll2FromOffsetsPass1 (Memory::SmallStackBuffer<size_t>* patchOffsets, typename STL_CONTAINER_OF_T::iterator incIfGreaterOrEqual) const;
                        nonvirtual  void    TwoPhaseIteratorPatcherAll2FromOffsetsPass2 (const Memory::SmallStackBuffer<size_t>& patchOffsets);

                    public:
                        nonvirtual  void    Invariant () const;

#if     qDebug
                    protected:
                        nonvirtual  void    _Invariant () const;
#endif

                    private:
                        friend  class   ForwardIterator;
                    };


                    /**
                     *      STLContainerWrapper::ForwardIterator is a private utility class designed
                     *  to promote source code sharing among the patched iterator implementations.
                     */
                    template    <typename STL_CONTAINER_OF_T, typename LOCKER>
                    class   STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>::ForwardIterator
                        : public DataStructures::STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator
                        , public PatchableContainerHelper<DataStructures::STLContainerWrapper<STL_CONTAINER_OF_T>, LOCKER>::PatchableIteratorMixIn {
                    private:
                        using   inherited_DataStructure =   typename DataStructures::STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator;
                        using   inherited_PatchHelper   =   typename PatchableContainerHelper<DataStructures::STLContainerWrapper<STL_CONTAINER_OF_T>, LOCKER>::PatchableIteratorMixIn;

                    public:
                        using   value_type  =   typename inherited::value_type;

                    public:
                        using   CONTAINER_TYPE  =    STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>;

                    public:
                        ForwardIterator (IteratorOwnerID ownerID, CONTAINER_TYPE* data);
                        ForwardIterator (const ForwardIterator& from);

                    public:
                        ~ForwardIterator ();

                    public:
                        nonvirtual  ForwardIterator& operator= (const ForwardIterator& rhs);

                    public:
                        template    <typename VALUE_TYPE>
                        nonvirtual  bool    More (VALUE_TYPE* current, bool advance);
                        template    <typename VALUE_TYPE>
                        nonvirtual  void    More (Memory::Optional<VALUE_TYPE>* current, bool advance);

                    public:
                        /**
                         */
                        nonvirtual  void    RemoveCurrent ();

                    public:
                        nonvirtual  void    SetCurrentLink (typename CONTAINER_TYPE::const_iterator  l)
                        {
                            inherited_DataStructure::SetCurrentLink (l);
                            fSuppressMore = false;
                        }

                    private:
                        /*
                         * OK to be private cuz CONTAINER_TYPE is a friend.
                         */
                        nonvirtual  void    TwoPhaseIteratorPatcherPass1 (typename STL_CONTAINER_OF_T::iterator oldI, Memory::SmallStackBuffer<ForwardIterator*>* items2Patch);
                        nonvirtual  void    TwoPhaseIteratorPatcherPass2 (typename STL_CONTAINER_OF_T::iterator newI);

                    public:
                        nonvirtual  void    Invariant () const;
#if     qDebug
                    protected:
                        nonvirtual  void    _Invariant () const;
#endif

                    public:
                        bool        fSuppressMore;  // for removealls

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
