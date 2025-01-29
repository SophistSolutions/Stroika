/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Variant_FormURLEncoded_Writer_h_
#define _Stroika_Foundation_DataExchange_Variant_FormURLEncoded_Writer_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Containers/Association.h"
#include "Stroika/Foundation/DataExchange/Variant/Writer.h"
#include "Stroika/Foundation/DataExchange/VariantValue.h"
#include "Stroika/Foundation/Streams/OutputStream.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::DataExchange::Variant::FormURLEncoded {

    using Containers::Association;
    using Traversal::Iterable;

    /**
     *  \brief Association (or VariantValue) to the output stream - following https://url.spec.whatwg.org/#application/x-www-form-urlencoded
     * 
     *  \see InternetMediaTypes::kWWWFormURLEncoded
     * 
     *  \par Example Usage
     *      \code
     *         
     *      \endcode
     */
    class Writer : public Variant::Writer {
    private:
        using inherited = Variant::Writer;

    private:
        class Rep_;

    public:
        /**
         */
        Writer ();

    public:
        /**
         *  @See Variant::Writer::Write, but overloaded to also take array of array of strings to write.
         * 
         *  \note Could have been called WriteMatrix (additional overloads) - but seemed best to emphasize connection
         *        to other Writers instead of similarity to Reader (where we call it ReadMatrix due to not being
         *        able to overload on return type).
         */
        using inherited::Write;
        nonvirtual void Write (const Association<String, String>& m, const Streams::OutputStream::Ptr<byte>& out);

    public:
        /**
         *  @See Variant::Writer::WriteAsBLOB, but overloaded to also take array of array of strings to write.
         */
        using inherited::WriteAsBLOB;
        [[nodiscard]] nonvirtual Memory::BLOB WriteAsBLOB (const Association<String, String>& m);

    private:
        nonvirtual shared_ptr<Rep_> GetRep_ () const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Writer.inl"

#endif /*_Stroika_Foundation_DataExchange_Variant_FormURLEncoded_Writer_h_*/
