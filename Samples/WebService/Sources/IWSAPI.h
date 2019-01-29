/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _StroikaSample_WebServices_IWSAPI_h_
#define _StroikaSample_WebServices_IWSAPI_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Model.h"

/**
 */

namespace StroikaSample::WebServices {
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
