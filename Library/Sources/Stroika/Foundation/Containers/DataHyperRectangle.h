/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DataHyperRectangle_h_
#define _Stroika_Foundation_Containers_DataHyperRectangle_h_ 1

#include "../StroikaPreComp.h"

#include "../Common/Compare.h"
#include "../Execution/Synchronized.h"
#include "../Traversal/Iterable.h"
#include "Common.h"

/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *
 *  TODO:
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            using Configuration::ArgByValueType;
            using Traversal::Iterable;
            using Traversal::Iterator;


#if 0

			template <typename CELL_TYPE, typename INDEX_1_TYPE = unsigned int>
			class DataHyperRectangle : public Iterable<CELL_TYPE> {
			public:
				DataHyperRectangle (INDEX_1_TYPE sz1)
			public:
				CELL_TYPE operator[] (INDEX_1 i) const;
				SmartRefToT<CELL_TYPE> operator[] (INDEX_1 i);
			};


			/// aliases in concrete
			template <typename CELL_TYPE, typename INDEX_1_TYPE = unsigned int>
			class DataHyperRectangle_Sparse : DataHyperRectangle<CELL_TYPE, INDEX_1_TYPE> {
			public:
				DataHyperRectangle_Sparse (INDEX_1_TYPE, CELL_TYPE defaultValue = {})
			};


			template <typename CELL_TYPE, typename INDEX_1_TYPE = unsigned int>
			class DataHyperRectangle_Dense : DataHyperRectangle<CELL_TYPE, INDEX_1_TYPE> {
			public:
				DataHyperRectangle_Dense (INDEX_1_TYPE)
			};

			see matrix class for tempsmartref
#endif


            /**
             */
            template <typename T>
            class DataHyperRectangle : public Iterable<T> {
            private:
                using inherited = Iterable<T>;

            protected:
                class _IRep;

            protected:
                using _SharedPtrIRep = typename inherited::template SharedPtrImplementationTemplate<_IRep>;

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using ArchetypeContainerType = DataHyperRectangle<T>;

            public:
                /**
                 */
				DataHyperRectangle ();
				DataHyperRectangle (const DataHyperRectangle<T>& src) noexcept;
				DataHyperRectangle (DataHyperRectangle<T>&& src) noexcept;

            protected:
                explicit DataHyperRectangle (const _SharedPtrIRep& src) noexcept;
                explicit DataHyperRectangle (_SharedPtrIRep&& src) noexcept;

            public:
                /**
                 */
                nonvirtual DataHyperRectangle<T>& operator= (const DataHyperRectangle<T>& rhs) = default;
                nonvirtual DataHyperRectangle<T>& operator= (DataHyperRectangle<T>&& rhs) = default;

            protected:
                /**
                 */
                template <typename T2>
                using _SafeReadRepAccessor = typename inherited::template _SafeReadRepAccessor<T2>;

            protected:
                /**
                 */
                template <typename T2>
                using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<T2>;

            protected:
                nonvirtual void _AssertRepValidType () const;
            };

            using Traversal::IteratorOwnerID;

            /**
             *  \brief  Implementation detail for DataHyperRectangle<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the DataHyperRectangle<T> container API.
             */
            template <typename T>
            class DataHyperRectangle<T>::_IRep : public Iterable<T>::_IRep {
            protected:
                _IRep () = default;

            public:
                virtual ~_IRep () = default;

            protected:
                using _SharedPtrIRep = typename DataHyperRectangle<T>::_SharedPtrIRep;

            public:
                virtual _SharedPtrIRep CloneEmpty (IteratorOwnerID forIterableEnvelope) const = 0;
                virtual void Push (ArgByValueType<T> item)                                    = 0;
            };

            /**
             *      Syntactic sugar for Equals()
             *
             *  \note   This function uses Common::DefaultEqualsComparer<T>, which in turn uses operator==(T,T). To
             *          use a different comparer, call Equals() directly.
             */
            template <typename T>
            bool operator== (const DataHyperRectangle<T>& lhs, const DataHyperRectangle<T>& rhs);

            /**
             *      Syntactic sugar for not Equals()
             *
             *  \note   This function uses Common::DefaultEqualsComparer<T>, which in turn uses operator==(T,T). To
             *          use a different comparer, call Equals() directly.
             */
            template <typename T>
            bool operator!= (const DataHyperRectangle<T>& lhs, const DataHyperRectangle<T>& rhs);
        }
    }
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "DataHyperRectangle.inl"

#endif /*_Stroika_Foundation_Containers_DataHyperRectangle_h_ */
