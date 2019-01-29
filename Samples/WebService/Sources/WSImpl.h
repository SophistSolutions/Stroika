/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _StroikaSample_WebServices_WSImpl_h_
#define _StroikaSample_WebServices_WSImpl_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Containers/Sequence.h"

#include "IWSAPI.h"

/**
 */

namespace StroikaSample::WebServices {

    /**
     *  \brief: WSImpl is the module providing the concrete C++ implementation (fully using C++ objects, exceptions etc) for the web service API.
     *
     *  This is straight C++ application logic, with (hopefully) zero logic relating to marshalling, or HTTP etc.
     */
    class WSImpl : public IWSAPI {
    public:
        virtual Number plus (Number lhs, Number rhs) const override;
        virtual Number minus (Number lhs, Number rhs) const override;
        virtual Number times (Number lhs, Number rhs) const override;
        virtual Number divide (Number lhs, Number rhs) const override;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "WSImpl.inl"

#endif /*_StroikaSample_WebServices_WSImpl_h_*/
