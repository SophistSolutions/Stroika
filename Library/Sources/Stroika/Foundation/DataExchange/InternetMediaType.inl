/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_InternetMediaType_inl_
#define _Stroika_Foundation_DataExchange_InternetMediaType_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Execution/ModuleInit.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {


            /*
             ********************************************************************************
             ********************************** InternetMediaType ***************************
             ********************************************************************************
             */
            inline  InternetMediaType::InternetMediaType (const String& ct)
                : fType_ (ct)
            {
            }
            inline  bool InternetMediaType::empty () const
            {
                return fType_.empty ();
            }
            inline  void InternetMediaType::clear ()
            {
                fType_.clear ();
            }
            inline  bool    InternetMediaType::IsA (const InternetMediaType& moreGeneralType) const
            {
                return IsSubTypeOfOrEqualTo (*this, moreGeneralType);
            }
            inline  bool InternetMediaType::operator< (const InternetMediaType& rhs) const
            {
                return fType_ < rhs.fType_;
            }
            inline  bool InternetMediaType::operator== (const InternetMediaType& rhs) const
            {
                return fType_ == rhs.fType_;
            }
            inline  bool InternetMediaType::operator!= (const InternetMediaType& rhs) const
            {
                return fType_ != rhs.fType_;
            }
            template    <>
            inline  String  InternetMediaType::As () const
            {
                return fType_;
            }
            template    <>
            inline  wstring InternetMediaType::As () const
            {
                return fType_.As<wstring> ();
            }


            /*
             ********************************************************************************
             ********************************** TypeMatchesAny ******************************
             ********************************************************************************
             */
            template    <typename CONTAINER>
            bool    TypeMatchesAny (const CONTAINER& types, const InternetMediaType& type2SeeIfMatchesAny)
            {
                for (auto i = types.begin (); i != types.end (); ++i) {
                    if (i->IsA (type2SeeIfMatchesAny)) {
                        return true;
                    }
                }
                return false;
            }


        }
    }
}


namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            namespace   Private_ {
                struct  InternetMediaType_ModuleData_ {
                    InternetMediaType_ModuleData_ ();
                    ~InternetMediaType_ModuleData_ ();

                    const   InternetMediaType   kOctetStream_CT;

                    const   InternetMediaType   kImage_CT;
                    const   InternetMediaType   kImage_PNG_CT;
                    const   InternetMediaType   kImage_GIF_CT;

                    const   InternetMediaType   kText_CT;
                    const   InternetMediaType   kText_HTML_CT;
                    const   InternetMediaType   kText_XHTML_CT;
                    const   InternetMediaType   kText_PLAIN_CT;

                    const   InternetMediaType   kJSON_CT;

                    const   InternetMediaType   kPDF_CT;
                    const   InternetMediaType   kURL_CT;

                    const   InternetMediaType   kXML_CT;

                    const   InternetMediaType   kXSLT_CT;
                    const   InternetMediaType   kJavaArchive_CT;
                    const   InternetMediaType   kApplication_RTF_CT;
                };
            }
        }
    }
}

namespace   {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::DataExchange::Private_::InternetMediaType_ModuleData_>    _Stroika_Foundation_DataExchange_InternetMediaType_ModuleData_;   // this object constructed for the CTOR/DTOR per-module side-effects
}

namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            namespace   PredefinedInternetMediaType {

                inline  const   InternetMediaType&  OctetStream_CT ()                       {   return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kOctetStream_CT;  }

                inline  const   InternetMediaType&  Image_CT ()                             {   return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kImage_CT;    }
                inline  const   InternetMediaType&  Image_PNG_CT ()                         {   return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kImage_PNG_CT;    }
                inline  const   InternetMediaType&  Image_GIF_CT ()                         {   return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kImage_GIF_CT;    }

                inline  const   InternetMediaType&  Text_CT ()                              {   return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kText_CT; }
                inline  const   InternetMediaType&  Text_HTML_CT ()                         {   return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kText_HTML_CT;    }
                inline  const   InternetMediaType&  Text_XHTML_CT ()                        {   return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kText_XHTML_CT;   }
                inline  const   InternetMediaType&  Text_PLAIN_CT ()                        {   return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kText_PLAIN_CT;   }

                inline  const   InternetMediaType&  JSON_CT ()                              {   return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kJSON_CT; }

                inline  const   InternetMediaType&  PDF_CT ()                               {   return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kPDF_CT;  }

                inline  const   InternetMediaType&  URL_CT ()                               {   return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kURL_CT;  }

                inline  const   InternetMediaType&  XML_CT ()                               {   return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kXML_CT;  }

                inline  const   InternetMediaType&  XSLT_CT ()                              {   return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kXSLT_CT; }

                inline  const   InternetMediaType&  JavaArchive_CT ()                       {   return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kJavaArchive_CT;  }

                inline  const   InternetMediaType&  Application_RTF_CT ()                   {   return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kApplication_RTF_CT;  }

            }
        }
    }
}
#endif  /*_Stroika_Foundation_DataExchange_InternetMediaType_inl_*/
