/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Variant_INI_Reader_h_
#define _Stroika_Foundation_DataExchange_Variant_INI_Reader_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Containers/Collection.h"

#include "../../VariantValue.h"

#include "../Reader.h"

#include "Profile.h"

/**
 *  \file
 *
 * TODO:
 *      @todo   Quoted characters (see http://en.wikipedia.org/wiki/INI_file)
 *
 *      @todo   VERY PRIMITIVE IMPL (no error checking/validation) - or at least little
 *
 *      @todo   Add these references to docs:
 *              http://en.wikipedia.org/wiki/INI_file
 *              http://stackoverflow.com/questions/190629/what-is-the-easiest-way-to-parse-an-ini-file-in-java
 *
 *      @todo   Config params should take choice about quoting (always, never)???
 *
 *      @todo   Characterset / BOM
 *
 */

namespace Stroika::Foundation::DataExchange::Variant::INI {

    /**
     *  \brief Reader for INI format files
     *
     *  \note INI format - https://en.wikipedia.org/wiki/INI_file
     *        Example from wikipedia
     *           ; last modified 1 April 2001 by John Doe
     *           [owner]
     *           name=John Doe
     *           organization=Acme Widgets Inc.
     *
     *           [database]
     *           ; use IP address in case network name resolution is not working
     *           server=192.0.2.62
     *           port=143
     *           file="payroll.dat"
     *
     *  The section headers (e.g. [database]) mark the start of sections. Each section is essential a name-value pair map.
     *
     *  Often there is a single unnamed section. And often there are named sections. These are collected together in what we call here
     *  a Profile.
     *
     *  \par Example Usage
     *      \code
     *          stringstream tmp;
     *          tmp << "NAME=\"Ubuntu\"" << endl;
     *          tmp << "VERSION=\"13.10, Saucy Salamander\"" << endl;
     *          tmp << "ID=ubuntu" << endl;
     *          tmp << "ID_LIKE=debian" << endl;
     *          tmp << "PRETTY_NAME=\"Ubuntu 13.10\"" << endl;
     *          tmp << "VERSION_ID=\"13.10\"" << endl;
     *          tmp << "HOME_URL=\"http://www.ubuntu.com/\"" << endl;
     *          tmp << "SUPPORT_URL=\"http://help.ubuntu.com/\"" << endl;
     *          tmp << "BUG_REPORT_URL=\"http://bugs.launchpad.net/ubuntu/\"" << endl;
     *          Variant::INI::Profile p = Variant::INI::Reader ().ReadProfile (tmp);
     *          EXPECT_TRUE (p.fNamedSections.empty ());
     *          EXPECT_TRUE (p.fUnnamedSection.fProperties.LookupValue ("NAME") == "Ubuntu");
     *          EXPECT_TRUE (p.fUnnamedSection.fProperties.LookupValue ("SUPPORT_URL") == "http://help.ubuntu.com/");
     *      \endcode
     */
    class Reader : public Variant::Reader {
    private:
        using inherited = Variant::Reader;

    private:
        class Rep_;

    public:
        Reader ();

    public:
        /**
         *  \brief though can read directly as VariantValue, reading as a Profile object maybe handier for this type of file.
         */
        nonvirtual Profile ReadProfile (const Streams::InputStream::Ptr<byte>& in);
        nonvirtual Profile ReadProfile (const Streams::InputStream::Ptr<Characters::Character>& in);
        nonvirtual Profile ReadProfile (const Traversal::Iterable<Characters::Character>& in);
        nonvirtual Profile ReadProfile (istream& in);
        nonvirtual Profile ReadProfile (wistream& in);
    };
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Reader.inl"

#endif /*_Stroika_Foundation_DataExchange_Variant_INI_Reader_h_*/
