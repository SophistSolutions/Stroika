/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Reader_h_
#define _Stroika_Foundation_DataExchange_XML_Reader_h_   1

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"
#include    "../../Streams/InputStream.h"
#include    "../../Streams/TextInputStream.h"

#include    "../Reader.h"
#include    "../VariantValue.h"

#include    "SerializationConfiguration.h"


/**
 *  \file
 *
 * TODO:
 *      @todo   NotYetImplemented
 *
 *      @todo   fix thread-safety - cloning rep - lock for access config data
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            namespace   XML {


                /**
                 *  @todo
                 * add options for stuff like - special for xml - assumed-outer-doc, stuff abotu namespaces, and st
                 *... todo namespaces - store in string (elt name a:b) and fill in namespace object accordingly.

                 define common struct (shared modeule - in common.h of this I guess? - for serializeation package parmas
                  like "use this tag for arrays" (where empty is an option but hard to read back)", and namespace list, and
                  do or dont include xml processing instruction
                 */
                class Reader : public DataExchange::Reader {
                private:
                    using   inherited   =   DataExchange::Reader;

                private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
                public:
#endif
                    class   Rep_;

                public:
                    Reader (const SerializationConfiguration& config = SerializationConfiguration ());

                public:
                    nonvirtual  SerializationConfiguration  GetConfiguration () const;
                    nonvirtual  void                        SetConfiguration (const SerializationConfiguration& config);

                private:
                    nonvirtual  const Rep_&     GetRep_ () const;
                    nonvirtual  Rep_&           GetRep_ ();
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

#endif  /*_Stroika_Foundation_DataExchange_XML_Reader_h_*/
