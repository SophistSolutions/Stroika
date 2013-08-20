/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchangeFormat_XML_SAXReader_h_
#define _Stroika_Foundation_DataExchangeFormat_XML_SAXReader_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"
#include    "../../Characters/String.h"
#include    "../../Execution/ProgressMonitor.h"
#include    "../../Memory/VariantValue.h"
#include    "../../Streams/BinaryInputStream.h"

#include    "Common.h"



/**
 *  \file
 *
 * TODO:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchangeFormat {
            namespace   XML {


                using   Characters::String;


                /**
                 *  Override any of these methods you want to recieve notification of the given event. There is no need to call the
                 *  inherited version.
                 */
                class   SAXCallbackInterface {
                public:
                    virtual void    StartDocument ();
                    virtual void    EndDocument ();
                public:
                    virtual void    StartElement (const String& uri, const String& localName, const String& qname, const map<String, Memory::VariantValue>& attrs);
                    virtual void    EndElement (const String& uri, const String& localName, const String& qname);
                public:
                    virtual void    CharactersInsideElement (const String& text);
                };


#if     qHasLibrary_Xerces
                /*
                 * Parse will throw an exception if it encouters any errors parsing.
                 */
                void    SAXParse (const Streams::BinaryInputStream& in, SAXCallbackInterface& callback, Execution::ProgressMontior* progress = nullptr);

                //SCHEMA STUFF NYI - SEE RFLLIB XMLDB
                //void  SAXParse (istream& in, const Schema& schema, CallbackInterface& callback, Execution::ProgressMontior* progress = nullptr);
#endif


            }
        }
    }
}





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "SAXReader.inl"

#endif  /*_Stroika_Foundation_DataExchangeFormat_XML_SAXReader_h_*/
