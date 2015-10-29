/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_StructuredDataStreamConsumer_h_
#define _Stroika_Foundation_DataExchange_StructuredDataStreamConsumer_h_ 1

#include    "../StroikaPreComp.h"

#include    "../Containers/Mapping.h"
#include    "../Characters/String.h"

#include    "VariantValue.h"



/**
 *  \file
 *
 * TODO:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {


            using   Characters::String;
            using   Containers::Mapping;


            /**
             *  Override any of these methods you want to recieve notification of the given event. There is no need to call the
             *  inherited version.
             *
             *  \note INTENDED as base for EITHER XML or JSON SAX(stream) based parsing.
             *
             *      @todo SEE   http://code.google.com/p/json-simple/
             *      @todo SEE   http://jackson.codehaus.org/Tutorial
             *      @todo SEE Stax API
             *
             *  \note   It MAYBE pointless and hopeless to unify SAX/JSON here... they are very differnt - but try and review these others
             */
            class   IStructuredDataStreamConsumer {
            public:
                /**
                 *  \note   The default implementation ignores this.
                 */
                virtual void    StartDocument ();

            public:
                /**
                 *  \note   The default implementation ignores this.
                 */
                virtual void    EndDocument ();

            public:
                /**
                 *  \note   The default implementation ignores this.
                 */
                virtual void    StartElement (const String& uri, const String& localName, const Mapping<String, VariantValue>& attrs);

            public:
                /**
                 *  \note   The default implementation ignores this.
                 */
                virtual void    EndElement (const String& uri, const String& localName);

            public:
                /**
                 *  \note   The default implementation ignores this.
                 */
                virtual void    CharactersInsideElement (const String& text);
            };


        }
    }
}





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "StructuredDataStreamConsumer.inl"

#endif  /*_Stroika_Foundation_DataExchange_StructuredDataStreamConsumer_h_*/
