/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Variant_CharacterDelimitedLines_Reader_h_
#define _Stroika_Foundation_DataExchange_Variant_CharacterDelimitedLines_Reader_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Configuration/Common.h"
#include "../../../Containers/Sequence.h"
#include "../../../Containers/Set.h"
#include "../../../Streams/InputStream.h"

#include "../../VariantValue.h"

#include "../Reader.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
 */

namespace Stroika::Foundation::DataExchange::Variant::CharacterDelimitedLines {

    using Characters::Character;
    using Characters::String;
    using Containers::Sequence;
    using Containers::Set;
    using Traversal::Iterable;

    /**
     *  \brief This COULD be easily used to read CSV files, or tab-delimited files, for example.
     * 
     *  \par Example Usage
     *      Source file(/usr/share/mime/globs):
     *           # This file was automatically generated by the
     *           # update-mime-database command. DO NOT EDIT!
     *           application/x-doom-wad:*.wad
     *           application/x-cd-image:*.iso
     *           application/x-abiword:*.abw.crashed
     *...
     *      \code
     *          Bijection<InternetMediaType, FileSuffixType> results{};
     *          for (Sequence<String> line : DataExchange::Variant::CharacterDelimitedLines::Reader{{':'}}.ReadMatrix (IO::FileSystem::FileInputStream::New ("/usr/share/mime/globs"sv))) {
     *              if (line.length () == 2) {
     *                  String glob = line[1];
     *                  if (glob.StartsWith ('*')) {
     *                      glob = glob.SubString (1);
     *                  }
     *                  results.Add (InternetMediaType{line[0]}, glob);
     *              }
     *          }
     *      \endcode
     */
    class Reader : public Variant::Reader {
    private:
        using inherited = Variant::Reader;

    private:
        class Rep_;

    public:
        Reader (const Set<Character>& columnDelimiters);

    public:
        /**
         *  Alternative reading API for this file type, which maybe helpful, to read as a series of lines.
         */
        nonvirtual Iterable<Sequence<String>> ReadMatrix (const Streams::InputStream::Ptr<byte>& in) const;
        nonvirtual Iterable<Sequence<String>> ReadMatrix (const Streams::InputStream::Ptr<Character>& in) const;
        nonvirtual Iterable<Sequence<String>> ReadMatrix (const Traversal::Iterable<Characters::Character>& in);
        nonvirtual Iterable<Sequence<String>> ReadMatrix (istream& in);
        nonvirtual Iterable<Sequence<String>> ReadMatrix (wistream& in);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Reader.inl"

#endif /*_Stroika_Foundation_DataExchange_Variant_CharacterDelimitedLines_Reader_h_*/
