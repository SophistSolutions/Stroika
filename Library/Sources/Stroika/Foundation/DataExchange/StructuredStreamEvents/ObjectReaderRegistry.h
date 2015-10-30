/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReaderRegistry_h_
#define _Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReaderRegistry_h_    1

#include    "../../StroikaPreComp.h"

#include    <type_traits>
#include    <typeindex>

#include    "../../Characters/String.h"
#include    "../../Configuration/Enumeration.h"
#include    "../../Containers/Bijection.h"
#include    "../../Containers/Collection.h"
#include    "../../Containers/Mapping.h"
#include    "../../Containers/Sequence.h"
#include    "../../Containers/Set.h"
#include    "../../Containers/SortedCollection.h"
#include    "../../Containers/SortedMapping.h"
#include    "../../Containers/SortedSet.h"
#include    "../../Execution/Synchronized.h"
#include    "../../Memory/Common.h"
#include    "../../Memory/Optional.h"
#include    "../../Traversal/DiscreteRange.h"
#include    "../../Traversal/Range.h"

#include    "ObjectReader.h"



/**
 *
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 *  TODO:
 *
 *      @todo    */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            namespace   StructuredStreamEvents {


                using   Characters::String;
                using   Containers::Mapping;
                using   Containers::Sequence;
                using   Containers::Set;
                using   Memory::Byte;


                /**
                // Look back to DataExchange::ObjectVariantmapper, but for now - KISS
                 */
                class   ObjectReaderRegistry {
                public:
                    using ReaderFromVoidStar = function<shared_ptr<ObjectReader::IContextReader> (void*)>;


                public:
                    // @todo use ArgByValueType
                    template    <typename T>
                    void    Add (const function<shared_ptr<ObjectReader::IContextReader> (T*)>& readerFactory)
                    {
                        Add (typeof (T), [readerFactory] (void* data) { return readerFactory (reinterpret_cast<T*> (data)); });
                    }

                public:
                    void    Add (type_index forType, const ReaderFromVoidStar& readerFactory)
                    {
                        fFactories_.Add (forType, readerFactory);
                    }

                public:
                    template    <typename T>
                    shared_ptr<ObjectReader::IContextReader>    MakeContextReader (T* data)
                    {
                        ReaderFromVoidStar  factory = *fFactories_.Lookup (typeof (T)); // must be found or caller/assert error
                        return factory (data);
                    }
                    shared_ptr<ObjectReader::IContextReader>    MakeContextReader (type_index ti, void* data)
                    {
                        ReaderFromVoidStar  factory = *fFactories_.Lookup (ti); // must be found or caller/assert error
                        return factory (data);
                    }

                private:
                    Mapping<type_index, ReaderFromVoidStar> fFactories_;

                };



                template    <typename   T>
                class   ComplexObjectReader2 : public ComplexObjectReader<T> {
                    ComplexObjectReader2 (ObjectReaderRegistry& objReg, T* vp)
                        : ComplexObjectReader<T>(vp)
                        , fObjRegistry  (objReg)
                    {
                    }

                    virtual void    HandleChildStart (StructuredStreamEvents::ObjectReader::Context& r, const StructuredStreamEvents::Name& name) override
                    {
                        Optional<pair<type_index, size_t>>   ti = fFieldNameToTypeMap.Lookup (name.fLocalName);
                        if (ti) {
                            Byte*   operatingOnObj = reinterpret_cast<Byte*> (fValuePtr);
                            Byte*   operatingOnObjField = operatingOnObj + ti->second;
                            _PushNewObjPtr (r, fObjRegistry.MakeContextReader (ti->first, operatingOnObjField));
                        }
#if 0
                        if (name.fLocalName == L"When") {
                            _PushNewObjPtr (r, make_shared<BuiltinReader<Time::DateTime>> (&fValuePtr->when));
                        }
                        else if (name.fLocalName == L"WithWhom") {
                            _PushNewObjPtr (r, make_shared<PersonReader_> (&fValuePtr->withWhom));
                        }
#endif
                        else {
                            if (fThrowOnUnrecongizedelts) {
                                ThrowUnRecognizedStartElt (name);
                            }
                            else {
                                _PushNewObjPtr (r, make_shared<IgnoreNodeReader> ());
                            }
                        }
                    }

                    ObjectReaderRegistry&   fObjRegistry;

                    Mapping<String, pair<type_index, size_t>> fFieldNameToTypeMap;      // @todo fix to be mapping on Name but need op< etc defined
                    bool                fThrowOnUnrecongizedelts;       // else ignroe
                };


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ObjectReaderRegistry.inl"

#endif  /*_Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReaderRegistry_h_*/
