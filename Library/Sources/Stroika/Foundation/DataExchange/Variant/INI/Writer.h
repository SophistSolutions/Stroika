/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Variant_INI_Writer_h_
#define _Stroika_Foundation_DataExchange_Variant_INI_Writer_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Configuration/Common.h"
#include "../../../Streams/OutputStream.h"

#include "../../VariantValue.h"

#include "../Writer.h"

#include "Profile.h"

/**
 *  \file
 */

namespace Stroika::Foundation::DataExchange::Variant::INI {

    /**
     */
    class Writer : public Variant::Writer {
    private:
        using inherited = Variant::Writer;

    private:
        class Rep_;

    public:
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
        nonvirtual void Write (const Profile& profile, const Streams::OutputStream::Ptr<byte>& out);
        nonvirtual void Write (const Profile& profile, const Streams::OutputStream::Ptr<Characters::Character>& out);
        nonvirtual void Write (const Profile& profile, ostream& out);
        nonvirtual void Write (const Profile& profile, wostream& out);

    public:
        /**
         *  @See Variant::Writer::WriteAsString, but overloaded to also take array of array of strings to write.
         */
        using inherited::WriteAsString;
        nonvirtual String WriteAsString (const Profile& profile);

    public:
        /**
         *  @See Variant::Writer::WriteAsBLOB, but overloaded to also take array of array of strings to write.
         */
        using inherited::WriteAsBLOB;
        nonvirtual Memory::BLOB WriteAsBLOB (const Profile& profile);

    private:
        nonvirtual shared_ptr<Rep_> GetRep_ () const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_DataExchange_Variant_INI_Writer_h_*/
