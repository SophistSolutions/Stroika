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
 *      @todo    gross hacks - replciatigng basic objectreaders to get this limping along...

 */



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
                    using ReaderFromVoidStarFactory = function<shared_ptr<ObjectReader::IContextReader> (void*)>;

                public:
                    void    Add (type_index forType, const ReaderFromVoidStarFactory& readerFactory)
                    {
                        fFactories_.Add (forType, readerFactory);
                    }
                    template    <typename T>
                    void    Add (const function<shared_ptr<ObjectReader::IContextReader> (T*)>& readerFactory)
                    {
                        Add (typeid (T), [readerFactory] (void* data) { return readerFactory (reinterpret_cast<T*> (data)); });
                    }

                public:
                    shared_ptr<ObjectReader::IContextReader>    MakeContextReader (type_index ti, void* data)
                    {
                        ReaderFromVoidStarFactory  factory = *fFactories_.Lookup (ti); // must be found or caller/assert error
                        return factory (data);
                    }
                    template    <typename T>
                    shared_ptr<ObjectReader::IContextReader>    MakeContextReader (T* data)
                    {
                        return MakeContextReader (typeid (T), data);
                    }

                private:
                    Mapping<type_index, ReaderFromVoidStarFactory> fFactories_;
                };



                template    <typename   T>
                class   ComplexObjectReader2 : public ComplexObjectReader<T> {
                public:
                    ComplexObjectReader2 (ObjectReaderRegistry* objReg, Mapping<String, pair<type_index, size_t>> maps, T* vp)
                        : ComplexObjectReader<T>(vp)
                        , fObjRegistry  (*objReg)
                        , fFieldNameToTypeMap (maps)
                    {
                    }
                    virtual void    HandleChildStart (StructuredStreamEvents::ObjectReader::Context& r, const StructuredStreamEvents::Name& name) override
                    {
                        Optional<pair<type_index, size_t>>   ti = fFieldNameToTypeMap.Lookup (name.fLocalName);
                        if (ti) {
                            Byte*   operatingOnObj = reinterpret_cast<Byte*> (this->fValuePtr);
                            Byte*   operatingOnObjField = operatingOnObj + ti->second;
                            this->_PushNewObjPtr (r, fObjRegistry.MakeContextReader (ti->first, operatingOnObjField));
                        }
                        else if (fThrowOnUnrecongizedelts) {
                            ThrowUnRecognizedStartElt (name);
                        }
                        else {
                            this->_PushNewObjPtr (r, make_shared<IgnoreNodeReader> ());
                        }
                    }

                    ObjectReaderRegistry&                       fObjRegistry;
                    Mapping<String, pair<type_index, size_t>>   fFieldNameToTypeMap;            // @todo fix to be mapping on Name but need op< etc defined
                    bool                                        fThrowOnUnrecongizedelts;       // else ignroe
                };
                template    <typename T>
                ObjectReaderRegistry::ReaderFromVoidStarFactory mkComplexObjectReader2Factory (ObjectReaderRegistry* objReg, const Mapping<String, pair<type_index, size_t>>& fieldname2Typeamps)
                {
                    return [objReg, fieldname2Typeamps] (void* data) -> shared_ptr<ObjectReader::IContextReader> { return make_shared<ComplexObjectReader2<T>> (objReg, fieldname2Typeamps, reinterpret_cast<T*> (data)); };
                }



                template    <typename ELEMENT_TYPE>
                struct ListOfObjectReader2: public ComplexObjectReader<vector<ELEMENT_TYPE>> {
                    bool                    readingAT_;
                    ELEMENT_TYPE            curTReading_;
                    ObjectReaderRegistry&   fObjRegistry;
                    String                  fName;

                    ListOfObjectReader2 (ObjectReaderRegistry* objReg, const String& name, vector<ELEMENT_TYPE>* v)
                        : ComplexObjectReader<vector<ELEMENT_TYPE>> (v)
                                , readingAT_ (false)
                                , fObjRegistry  (*objReg)
                                , fName  (name)
                    {
                    }
                    virtual void HandleChildStart (ObjectReader::Context& r, const StructuredStreamEvents::Name& name) override
                    {
                        if (name.fLocalName == fName) {
                            if (readingAT_) {
                                Containers::ReserveSpeedTweekAdd1 (*this->fValuePtr);
                                this->fValuePtr->push_back (curTReading_);
                                readingAT_ = false;
                            }
                            readingAT_ = true;
                            curTReading_ = ELEMENT_TYPE (); // clear because dont' want to keep values from previous elements
                            this->_PushNewObjPtr (r, fObjRegistry.MakeContextReader<ELEMENT_TYPE> (&curTReading_));
                        }
                        else {
                            ThrowUnRecognizedStartElt (name);
                        }
                    }
                    virtual void HandleEndTag (ObjectReader::Context& r) override
                    {
                        if (readingAT_) {
                            Containers::ReserveSpeedTweekAdd1 (*this->fValuePtr);
                            this->fValuePtr->push_back (curTReading_);
                            readingAT_ = false;
                        }
                        ComplexObjectReader<vector<ELEMENT_TYPE>>::HandleEndTag (r);
                    }
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
