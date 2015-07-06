/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Writer_h_
#define _Stroika_Foundation_DataExchange_XML_Writer_h_    1

#include    "../../StroikaPreComp.h"

#include    "../../Characters/String.h"
#include    "../../Configuration/Common.h"
#include    "../../Streams/OutputStream.h"

#include    "../Writer.h"
#include    "../VariantValue.h"
#include    "Common.h"
#include    "SerializationConfiguration.h"



/*
 * TODO:
 *   @todo   Probably wrong, and certainly incomplete, but it is now at the point of being testable.
 *
 *   @todo   Add SerializationOptions (distingished from SerializationConfiguariton cuz thats used for
 *           both reader and writer).
 *           o   include?xml... header
 *           o   Pretty print (spacing/tabs etc)
 *           o   POSSIBLY option for characterset to write wiith?
 *
 *      @todo   fix thread-safety - cloning rep - lock for access config data
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            namespace   XML {


                using   Characters::String;

                /**
                 *  @todo
                 * add options for stuff like - special for xml - assumed-outer-doc, stuff abotu namespaces, and st
                 *... todo namespaces - store in string (elt name a:b) and fill in namespace object accordingly.
                 *
                 * The arguemnt VariantValue must be composed of any combination of these types:
                 *          o   VariantValue::eBoolean
                 *          o   VariantValue::eInteger
                 *          o   VariantValue::eFloat
                 *          o   VariantValue::eString
                 *          o   VariantValue::eMap
                 *          o   VariantValue::eArray
                 *  or it can be the type:
                 *          o   VariantValue::eNull
                 *
                 *  Other types are illegal an XML and will trigger a 'Require' failure.
                 */
                class   Writer : public DataExchange::Writer {
                private:
                    using   inherited   =   DataExchange::Writer;

                private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
                public:
#endif
                    class   Rep_;

                public:
                    Writer (const SerializationConfiguration& config = SerializationConfiguration ());

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
#include    "Writer.inl"

#endif  /*_Stroika_Foundation_DataExchange_XML_Writer_h_*/
