/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
     *  \note   It MAYBE pointless and hopeless to unify SAX/JSON here... they are very different - but try and review these others
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
        virtual ~IConsumer () = default;

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
         *  \note Before Stroika v3.0d5 - **incompatible change** - StartElement() didn't take a map of attributes.
         *        Instead, it generated a 'sub-element' for each. To achieve this same effect, a callback may simply
         *        iterate over each attribute and call 'StartElement/TextInsideElement/EndElement' in its implementation.
         */
        virtual void StartElement (const Name& name, const Mapping<Name, String>& attributes);

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
