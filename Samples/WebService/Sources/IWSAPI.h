/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _StroikaSample_WebServices_IWSAPI_h_
#define _StroikaSample_WebServices_IWSAPI_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Collection.h"

#include "Model.h"

/**
 */

namespace StroikaSample::WebServices {
    using Stroika::Foundation::Characters::String;
    using Stroika::Foundation::Containers::Collection;

    using namespace Model;

    /**
     *  \brief: IWSAPI defines an abstract version of the web service API provided by this program.
     */
    class IWSAPI {
    protected:
        IWSAPI () = default;

    public:
        IWSAPI (const IWSAPI&) = delete;
        virtual ~IWSAPI ()     = default;

    public:
        virtual Collection<String> Variables_GET () const                                      = 0;
        virtual Number             Variables_GET (const String& variable) const                = 0;
        virtual void               Variables_DELETE (const String& variable) const             = 0;
        virtual void               Variables_SET (const String& variable, const Number& value) = 0;

    public:
        /**
         */
        virtual Number plus (Number lhs, Number rhs) const   = 0;
        virtual Number minus (Number lhs, Number rhs) const  = 0;
        virtual Number times (Number lhs, Number rhs) const  = 0;
        virtual Number divide (Number lhs, Number rhs) const = 0;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "IWSAPI.inl"

#endif /*_StroikaSample_WebServices_IWSAPI_h_*/
