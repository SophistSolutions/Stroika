/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Resource_h_
#define _Stroika_Foundation_DataExchange_XML_Resource_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/DataExchange/XML/Common.h"
#include "Stroika/Foundation/IO/Network/URI.h"

namespace Stroika::Foundation::DataExchange::XML::Resource {

    using Characters::String;
    using Containers::Sequence;
    using IO::Network::URI;
    using Traversal::Iterable;

    /**
     *  This is a 'name' of something that can be #included, or referred to from a DTD, Schema etc. Typically think
     *  of this like a 'URL'.
     */
    struct Name {
        optional<URI>    fNamespace;
        optional<String> fPublicID;
        optional<String> fSystemID;

        bool operator== (const Name&) const = default;
#if qCompilerAndStdLib_explicitly_defaulted_threeway_warning_Buggy
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdefaulted-function-deleted\"")
#endif
        auto operator<=> (const Name&) const = default;
#if qCompilerAndStdLib_explicitly_defaulted_threeway_warning_Buggy
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdefaulted-function-deleted\"")
#endif

        /**
         */
        nonvirtual String ToString () const;
    };

    /**
     *  This is a resource which is the data associated with a name (to be looked up by the resolver, typically during a parse).
     *  Think of this as a file someplace, with a name, and some data in it.
     */
    struct Definition {
        Name         fName;
        Memory::BLOB fData; // maybe add MIME:ContentType?
    };

    /**
     */
    struct IResolverRep {
        virtual ~IResolverRep () = default;
        /*
         *  return nullopt if not found
         */
        virtual optional<Definition> Lookup (const Name& n) const = 0;
    };

    /**
     *  3 main kinds of resolvers:
     *      o   NONE (very often you have all the data you need right in the initial parse call
     *      o   in-memory (preloaded)
     *      o   from external URLs (filesystem, or network).
     */
    class ResolverPtr {
    public:
        ResolverPtr () = default;
        ResolverPtr (nullptr_t);
        ResolverPtr (const shared_ptr<IResolverRep>& rep);

    public:
        bool operator== (const ResolverPtr&) const = default;

    public:
        nonvirtual optional<Definition> Lookup (const Name& n) const;

    private:
        shared_ptr<IResolverRep> fRep_;
    };

    namespace MemoryResolver {
        /**
         *  To use in memory or static or preloaded data definitions for parsing xml resources.
         *  NOTE - this could be replaced with a resolver that loads resources from file, or from the network.
         */
        ResolverPtr New (const Iterable<Definition>& defs);
    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Resource.inl"

#endif /*_Stroika_Foundation_DataExchange_XML_Resource_h_*/
