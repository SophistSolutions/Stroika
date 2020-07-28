/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_InternetMediaTypeRegistry_inl_
#define _Stroika_Foundation_DataExchange_InternetMediaTypeRegistry_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Execution/ModuleInit.h"
#include "../IO/FileSystem/PathName.h"

namespace Stroika::Foundation::DataExchange {

    /*
     ********************************************************************************
     ************************** InternetMediaTypeRegistry ***************************
     ********************************************************************************
     */
    inline optional<InternetMediaTypeRegistry::FileSuffixType> InternetMediaTypeRegistry::GetPreferredAssociatedFileSuffix (const InternetMediaType& ct) const
    {
        AssertNotNull (fFrontEndRep_);
        return fFrontEndRep_->GetPreferredAssociatedFileSuffix (ct);
    }
    inline Set<String> InternetMediaTypeRegistry::GetAssociatedFileSuffixes (const InternetMediaType& ct) const
    {
        AssertNotNull (fFrontEndRep_);
        return fFrontEndRep_->GetAssociatedFileSuffixes (ct);
    }
    inline optional<String> InternetMediaTypeRegistry::GetAssociatedPrettyName (const InternetMediaType& ct) const
    {
        AssertNotNull (fFrontEndRep_);
        return fFrontEndRep_->GetAssociatedPrettyName (ct);
    }
    inline optional<InternetMediaType> InternetMediaTypeRegistry::GetAssociatedContentType (const filesystem::path& fileSuffix) const
    {
        return GetAssociatedContentType (IO::FileSystem::FromPath (fileSuffix));
    }

}

namespace Stroika::Foundation::DataExchange::Private_ {
    struct InternetMediaType_ModuleData_ {
        InternetMediaType_ModuleData_ ();
        ~InternetMediaType_ModuleData_ () = default;

        const InternetMediaType::AtomType kText_Type;
        const InternetMediaType::AtomType kImage_Type;
        const InternetMediaType::AtomType kApplication_Type;

        const InternetMediaType kOctetStream_CT;

        const InternetMediaType kImage_PNG_CT;
        const InternetMediaType kImage_GIF_CT;
        const InternetMediaType kImage_JPEG_CT;

        const InternetMediaType kText_HTML_CT;
        const InternetMediaType kText_XHTML_CT;
        const InternetMediaType kApplication_XML_CT;
        const InternetMediaType kText_XML_CT;
        const InternetMediaType kText_PLAIN_CT;
        const InternetMediaType kText_CSV_CT;

        const InternetMediaType kJSON_CT;

        const InternetMediaType kPDF_CT;
        const InternetMediaType kURL_CT;

        const InternetMediaType kXML_CT;

        const InternetMediaType kXSLT_CT;
        const InternetMediaType kJavaArchive_CT;
        const InternetMediaType kApplication_RTF_CT;
        const InternetMediaType kApplication_Zip_CT;
    };
}

namespace {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::DataExchange::Private_::InternetMediaType_ModuleData_> _Stroika_Foundation_DataExchange_InternetMediaTypeRegistry_ModuleData_; // this object constructed for the CTOR/DTOR per-module side-effects
}

namespace Stroika::Foundation::DataExchange::PredefinedInternetMediaType::PRIVATE_ {

    inline const InternetMediaType::AtomType& Text_Type () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kText_Type; }
    inline const InternetMediaType::AtomType& Image_Type () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kImage_Type; }
    inline const InternetMediaType::AtomType& Application_Type () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kApplication_Type; }

    inline const InternetMediaType& OctetStream_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kOctetStream_CT; }

    inline const InternetMediaType& Image_PNG_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kImage_PNG_CT; }
    inline const InternetMediaType& Image_GIF_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kImage_GIF_CT; }
    inline const InternetMediaType& Image_JPEG_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kImage_JPEG_CT; }

    inline const InternetMediaType& Text_HTML_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kText_HTML_CT; }
    inline const InternetMediaType& Text_XHTML_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kText_XHTML_CT; }
    inline const InternetMediaType& ApplicationXML_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kApplication_XML_CT; }
    inline const InternetMediaType& Text_XML_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kText_XML_CT; }
    inline const InternetMediaType& Text_PLAIN_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kText_PLAIN_CT; }
    inline const InternetMediaType& Text_CSV_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kText_CSV_CT; }

    inline const InternetMediaType& JSON_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kJSON_CT; }

    inline const InternetMediaType& PDF_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kPDF_CT; }

    inline const InternetMediaType& URL_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kURL_CT; }

    inline const InternetMediaType& XML_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kXML_CT; }

    inline const InternetMediaType& XSLT_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kXSLT_CT; }

    inline const InternetMediaType& JavaArchive_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kJavaArchive_CT; }

    inline const InternetMediaType& Application_RTF_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kApplication_RTF_CT; }

    inline const InternetMediaType& Application_Zip_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kApplication_Zip_CT; }

}

#endif /*_Stroika_Foundation_DataExchange_InternetMediaType_inl_*/
