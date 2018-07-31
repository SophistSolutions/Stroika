/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_StructuredStreamEvents_IConsumer_h_
#define _Stroika_Foundation_DataExchange_StructuredStreamEvents_IConsumer_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"
#include "../../Containers/Mapping.h"

#include "Name.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 *
 */

namespace Stroika::Foundation::DataExchange::StructuredStreamEvents {

    using Characters::String;
    using Containers::Mapping;

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
     *
     *  \note   What we call here an "Element" really corresponds more closely to a "Node" in XML. We essentuially include attributes
     *          (and may soon include processing instructions or other ignored thigns) as special type elements).
     *
     *          @todo then when you configure the SAX parser, tell it (consmer interface returning a set of types of things it wants)
     *          to optimize so just the right types of elements sent.
     *
     *  @see ObjectReaderRegistry for examples of one way to use this
     */
    class IConsumer {
    public:
        /**
         *  \note   The default implementation ignores this.
         */
        virtual void StartDocument ();

    public:
        /**
         *  \note   The default implementation ignores this.
         */
        virtual void EndDocument ();

    public:
        /**
         *  \note   The default implementation ignores this.
         *
         *  \note   Some XML-based SAX-event-sink classes have a StartElement() taking a map of attributes. Instead,
         *          we represent those as sub-elements, with names having the 'IsAttribute' field true.
         *
         *          The reason for this departure is to help harmonize SAX-like parsing of JSON and XML.
         */
        virtual void StartElement (const Name& name);

    public:
        /**
         *  \note   The default implementation ignores this.
         */
        virtual void EndElement (const Name& name);

    public:
        /**
         *  \note   The default implementation ignores this.
         *
         *  \note   Callers will report zero to many chunks of text
         *          data. They do not necessarily gather it up into one big block.
         */
        virtual void TextInsideElement (const String& text);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "IConsumer.inl"

#endif /*_Stroika_Foundation_DataExchange_StructuredStreamEvents_IConsumer_h_*/
