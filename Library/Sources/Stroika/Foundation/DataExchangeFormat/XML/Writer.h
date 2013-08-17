/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchangeFormat_XML_Writer_h_
#define _Stroika_Foundation_DataExchangeFormat_XML_Writer_h_    1

#include    "../../StroikaPreComp.h"

#include    "../../Characters/String.h"
#include    "../../Configuration/Common.h"
#include    "../../Memory/VariantValue.h"
#include    "../../Streams/BinaryOutputStream.h"
#include    "../../Streams/TextOutputStream.h"

#include    "../Writer.h"
#include    "Common.h"



/*
* TODO:
*   @todo   Probably wrong, and certainly incomplete, but it is now at the point of being testable.
*
*   @todo   See about <?xml ... heading and char encoding - see class ssiue in todo below
*
*   @todo   See dataexchangeformat/todo.md - we may need class for writer - so can fill in paarms like incliuding
*           top-level <?xml...stuff and charset
*/



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchangeFormat {
            namespace   XML {


                using   Characters::String;

                /**
                 *  @todo
                 * add options for stuff like - special for xml - assumed-outer-doc, stuff abotu namespaces, and st
                 *... todo namespaces - store in string (elt name a:b) and fill in namespace object accordingly.
                 *
                 * The arguemnt VariantValue must be composed of any combination of these types:
                 *          o   Memory::VariantValue::eBoolean
                 *          o   Memory::VariantValue::eInteger
                 *          o   Memory::VariantValue::eFloat
                 *          o   Memory::VariantValue::eString
                 *          o   Memory::VariantValue::eMap
                 *          o   Memory::VariantValue::eArray
                 *  or it can be the type:
                 *          o   Memory::VariantValue::eNull
                 *
                 *  Other types are illegal an XML and will trigger a 'Require' failure.
                 */
                class   Writer : public DataExchangeFormat::Writer {
                private:
                    typedef DataExchangeFormat::Writer  inherited;

                private:
#if     !qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes
                public:
#endif
                    class   Rep_;

                public:
                    Writer ();

                public:
                    //@todo - this should take OPTIONAL<STRING>...
                    //
                    // NOTE - defaults to "Document"
                    // if empty - then Write requiers
                    //              Require (v.GetType () == Memory::VariantValue::Type::eMap);

                    nonvirtual  String  GetDocumentElementName () const;
                    nonvirtual  void    SetDocumentElementName (const String& n);

                private:
                    nonvirtual  shared_ptr<Rep_>    GetRep_ () const;
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

#endif  /*_Stroika_Foundation_DataExchangeFormat_XML_Writer_h_*/
