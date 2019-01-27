/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _StroikaSample_WebServices_Model_h_
#define _StroikaSample_WebServices_Model_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <complex>

#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"

/**
 * \brief: The Model module defines all the objects, which can appear in web service requests (on the request or response side).
 *
 *  These objects appear (are used) in the IWSAPI.
 *
 *  This module also provides ObjectVariantMapper objects which translate between DataExchange::VariantValue objects and the C++ objects defined here
 *  (which is what allows these objects to be marshalled through HTTP).
 */
namespace StroikaSample::WebServices::Model {

    using namespace Stroika::Foundation;

    using Number = std::complex<double>;

    /**
     * ObjectVariantMapper which maps all the model data objects to/from VariantValue objects.
     */
    extern const DataExchange::ObjectVariantMapper kMapper;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Model.inl"

#endif /*_StroikaSample_WebServices_Model_h_*/
