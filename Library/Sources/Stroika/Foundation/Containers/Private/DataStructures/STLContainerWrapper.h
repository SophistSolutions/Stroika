/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */

#ifndef _Stroika_Foundation_Containers_Private_DataStructures_STLContainerWrapper_h_
#define _Stroika_Foundation_Containers_Private_DataStructures_STLContainerWrapper_h_


/*
 *  TODO:
 *
 */





/*
 *
 *  Description:
 *
 *
 */

#include    "../../../StroikaPreComp.h"

#include    "../../../Configuration/Common.h"

#include    "../../Common.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {
                namespace   DataStructures {


                    /**
                     *  Use this to wrap an underlying stl container (like std::vector or stl:list, etc) to adapt
                     *  it for Stroika containers.
                     */
                    template    <typename T, typename CONTAINER_OF_T>
                    class   STLContainerWrapper : public CONTAINER_OF_T {
                    public:
                        STLContainerWrapper ();
                        STLContainerWrapper (const STLContainerWrapper<T, CONTAINER_OF_T>& from);
                    public:
                        ~STLContainerWrapper ();

                    public:
                        nonvirtual  STLContainerWrapper<T>& operator= (const STLContainerWrapper<T, CONTAINER_OF_T>& rhs);

                        /*
                         * Methods to do the patching yourself. Iterate over all the iterators and
                         * perform patching.
                         */
                    public:
                        //  are there any iterators to be patched?
                        nonvirtual  bool    HasActiveIterators () const;
                        //  call after add
                        nonvirtual  void    PatchViewsAdd (typename CONTAINER_OF_T::iterator i) const;
                        //  call before remove
                        nonvirtual  void    PatchViewsRemove (typename CONTAINER_OF_T::iterator i) const;
                        //  call after removeall
                        nonvirtual  void    PatchViewsRemoveAll () const;
                        //  call after realloc could have happened
                        nonvirtual  void    PatchViewsRealloc () const;


                    public:
                        class IteratorPatchHelper;
                    private:
                        IteratorPatchHelper* fIterators;
                        friend  class   IteratorPatchHelper;
                    };


                    /*
                     *      STLContainerWrapper::IteratorPatchHelper is a private utility class designed
                     *  to promote source code sharing among the patched iterator implementations.
                     */
                    template    <typename T, typename CONTAINER_OF_T>
                    class   STLContainerWrapper::IteratorPatchHelper {
                    public:
                        IteratorPatchHelper (const STLContainerWrapper<T, CONTAINER_OF_T>& data);
                        IteratorPatchHelper (const IteratorPatchHelper<T, CONTAINER_OF_T>& from);
                    public:
                        ~IteratorPatchHelper ();

                    public:
                        nonvirtual  IteratorPatchHelper<T, CONTAINER_OF_T>& operator= (const IteratorPatchHelper<T, CONTAINER_OF_T>& rhs);

                    public:
                        //  call after add
                        nonvirtual  void    PatchAdd (typename CONTAINER_OF_T::iterator i);
                        //  call before remove
                        nonvirtual  void    PatchRemove (typename CONTAINER_OF_T::iterator i);
                        //  call after removeall
                        nonvirtual  void    PatchRemoveAll ();
                        //  call after realloc could have happened
                        nonvirtual  void    PatchRealloc ();

                    protected:
                        const STLContainerWrapper<T, CONTAINER_OF_T>*    fData;
                        IteratorPatchHelper*                            fNext;
                    };


                }
            }
        }
    }
}
#endif  /*_Stroika_Foundation_Containers_Private_DataStructures_STLContainerWrapper_h_ */


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "STLContainerWrapper.inl"

