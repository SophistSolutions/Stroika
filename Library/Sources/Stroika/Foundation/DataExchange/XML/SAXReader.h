/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_SAXReader_h_
#define _Stroika_Foundation_DataExchange_XML_SAXReader_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"
#include    "../../Containers/Mapping.h"
#include    "../../Characters/String.h"
#include    "../../Execution/ProgressMonitor.h"
#include    "../../Streams/BinaryInputStream.h"

#include    "../VariantValue.h"
#include    "Common.h"



/**
 *  \file
 *
 * TODO:
 *      @todo   Redo SAXParse() using factory style - like we have for IO::Networking::Transfer (curl or winhttp)
 *              then have ability to check for registered impl (and select impl) for SAX backend)
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            namespace   XML {


                using   Characters::String;
                using   Containers::Mapping;

                /**
                 *  Override any of these methods you want to recieve notification of the given event. There is no need to call the
                 *  inherited version.
                 */
                class   SAXCallbackInterface {
                public:
                    virtual void    StartDocument ();
                    virtual void    EndDocument ();
                public:
                    virtual void    StartElement (const String& uri, const String& localName, const String& qname, const Mapping<String, VariantValue>& attrs);
                    virtual void    EndElement (const String& uri, const String& localName, const String& qname);
                public:
                    virtual void    CharactersInsideElement (const String& text);
                };


                /**
                 * Parse will throw an exception if it encouters any errors parsing.
                 */
                void    SAXParse (const Streams::InputStream<Memory::Byte>& in, SAXCallbackInterface& callback, Execution::ProgressMonitor::Updater progress = nullptr);

                //SCHEMA STUFF NYI - SEE RFLLIB XMLDB
                //void  SAXParse (istream& in, const Schema& schema, CallbackInterface& callback, Execution::ProgressMontior* progress = nullptr);


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

#endif  /*_Stroika_Foundation_DataExchange_XML_SAXReader_h_*/
