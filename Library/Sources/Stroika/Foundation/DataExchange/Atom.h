﻿/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Atom_h_
#define _Stroika_Foundation_DataExchange_Atom_h_ 1

#include "../StroikaPreComp.h"

#if defined(__cpp_impl_three_way_comparison)
#include <compare>
#endif

#include "../Characters/String.h"
#include "../Common/Compare.h"

/**
 *  \file
 *
 * TODO:
 *      @todo   Very weak (performance), first draft implementaiton of AtomManager_Default.
 *
 *              Instead, store a linked list of BLOBs - and for each BLOB keep
 *              a map<> of strings in each BLOB.
 *
 *              Use that to fairly quickly and cheaply lookup (Intern).
 *              Then change ptrdiff_t to be the pointer to the actual string.
 *
 *      @todo   Better document and define having AtomManagers with custom arenas.
 */

namespace Stroika::Foundation::DataExchange {

    using Characters::String;

    /**
     * Default is single global registey. Implemnt using compact storage - with strings allocated
     *  in memory blcoks next to one another (not full string objects)
     *  and then return String_Constant objects that point into it.
     *
     * like blockallocation - gains in performacne due to not ever having to free anything
     *
     *  @todo - CLEANUP DOCS
     */
    struct AtomManager_Default {
        typedef ptrdiff_t                 AtomInternalType;
        static constexpr AtomInternalType kEmpty = -1;
        // This API must accept an empty string as argument and return kEmpty
        static AtomInternalType Intern (const String& s);
        // if given kEmpty, Extract will return an empty String{} object
        static String Extract (AtomInternalType atomI);
    };

    struct AtomManager_CaseInsensitive {
        typedef ptrdiff_t                 AtomInternalType;
        static constexpr AtomInternalType kEmpty = -1;
        static AtomInternalType           Intern (const String& s);
        static String                     Extract (AtomInternalType atomI);
    };

    /**
     *  \brief An Atom is like a String, except that its much cheaper to copy/store/compare, and the semantics of compare are queer
     *
     *  An Atom is a wrapper on an underlying String object. You can always extract the original String.
     *
     *  The name 'atom' comes from LISP, and how it represented symbol names.
     *
     *  An Atom is much cheaper to copy/store and compare than a String. However - note that the compare is NOT the same as
     *  the traditioanl string compare. One Atom is equal to another IFF the underlying Strings would be
     *  equal. But the < behavior is very arbitrary. The only garuantee is that its consistent for the lifetime
     *  of the ATOM_MANAGER.
     *
     *  Note converting a String to an Atom maybe expensive, but if you can store the values as Atoms, future lookups
     *  with something like a hashtable can be much faster.
     *
     *  Atom can even be used with Set_Bitstring () – esp if you don’t use generic Atom<> but one with its own custom arena!
     *
     *  \em Design Choice:
     *      In some ways, this could be more powerful if the Atom construction took a ATOM_MANAGER as parameter.
     *      Then we could store the pointer, and do wholesale clearing / throwing away of atom names. But to make
     *      that work would have alot of cost (always passing in extra param to construction), and
     *      tracking that pointer in each atom instance (would have to use shared_ptr to manage lifetimes or somehow
     *      assure lifetimes of atoms longer than their owning manager.
     *
     *      That might make sense todo, but would have a lot of cost. It COULD be done in the future (this API would then
     *      just be a type specialization).
     *
     *      But thats not where we start.
     *
     *  \em Extending Atom type and funky Managers
     *
     *      In HealthFrame, we use 'atoms' for user-defined strings, but specially formatted strings
     *      like C00013044, or H342341 are concepts. Also - the objects returned internally are concepts.
     *
     *      This fits perfectly. You can use:
     *          struct  AtomManager_Concepts {
     *              typedef ConceptKey  AtomInternalType;
     *              static  constexpr Concept    kEmpty = ConceptKey();
     *              static  AtomInternalType    Intern (const String& s);
     *              static  String  Extract (AtomInternalType atomI);
     *          };
     *
     *      and then use for
     *
     *          struct Concept : Atom<AtomManager_Concepts> {
     *              // just constructors /forwarding
     *              Concept () = default;
     *              Concept (const String& s) = default;
     *              Concept (const Concept& concept) = default;
     *              Concept (const ConceptKey& concept) = default;
     *
     *              // and access concept key
     *              inline ConceptKey GetConceptKey () const { return _GetInternalRep (); }
     *          };
     *
     *      AtomManager_Concepts::Intern () just indirects to AtomManager_Default::Intern () and
     *      then differently, and Extract() can look at the fields of the concept key and either
     *      algorithmically generate a name or indirect to AtomManager_Default::Extract ();
     *
     *  @see Microsoft.net String::Intern () - http://msdn.microsoft.com/en-us/library/system.string.intern(v=vs.110).aspx
     *
     *  \note <a href="Coding Conventions.md#Comparisons">Comparisons</a>:
     *        o Standard Stroika Comparison support (operator<=>,operator==, etc);
     *
     *          Atom's are compared in a way that will NOT in general be the same as print name compare. 
     *          In general, their comparison will persist  for app lifetime.
     */
    template <typename ATOM_MANAGER = AtomManager_Default>
    class Atom {
    public:
        using AtomInternalType = typename ATOM_MANAGER::AtomInternalType;

    public:
        /**
         *  with the Atom (AtomInternalType) CTOR, we \req the value already be interned in the ATOM_MANAGER.
         */
        constexpr Atom ();
        Atom (const wchar_t* src);
        Atom (const wstring& src);
        Atom (const wstring_view& src);
        Atom (const String& src);
        constexpr Atom (const Atom& src) = default;
        constexpr Atom (const AtomInternalType& src);

    public:
        /**
         */
        nonvirtual Atom& operator= (const Atom& rhs) = default;

    public:
        /**
         */
        nonvirtual String GetPrintName () const;

#if __cpp_impl_three_way_comparison >= 201907
    public:
        /**
         */
        nonvirtual strong_ordering operator<=> (const Atom& rhs) const;

    public:
        /**
         */
        nonvirtual bool operator== (const Atom& rhs) const;
#endif

    public:
        /**
         *  This corresponds to an empty string. An 'empty' Atom can be constructed with:
         *      \code
         *          Atom<> a1;
         *          Atom<> a2 = L"";
         *      \endcode
         */
        nonvirtual constexpr bool empty () const;

    public:
        /**
         *  Set the state to empty
         */
        nonvirtual void clear ();

    public:
        /**
         *  Template on <T> only defined for
         *      T == String
         *      T == wstring
         */
        template <typename T>
        nonvirtual T As () const;

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

    private:
        template <typename T>
        struct type_ {
        };

        template <typename T>
        nonvirtual T       As_ (type_<T>) const;
        nonvirtual String  As_ (type_<String>) const;
        nonvirtual wstring As_ (type_<wstring>) const;

    protected:
        /**
         */
        nonvirtual AtomInternalType _GetInternalRep () const;

    private:
#if __cpp_impl_three_way_comparison < 201907
    public: // for operator access
#endif
        AtomInternalType fValue_;
    };

#if __cpp_impl_three_way_comparison < 201907
    /**
     *  Basic operator overloads with the obvious meaning, and simply indirect to @Version::ThreeWayComparer (const Version& rhs)
     */
    template <typename ATOM_MANAGER>
    bool operator< (Atom<ATOM_MANAGER> lhs, Atom<ATOM_MANAGER> rhs);
    template <typename ATOM_MANAGER>
    bool operator<= (Atom<ATOM_MANAGER> lhs, Atom<ATOM_MANAGER> rhs);
    template <typename ATOM_MANAGER>
    bool operator== (Atom<ATOM_MANAGER> lhs, Atom<ATOM_MANAGER> rhs);
    template <typename ATOM_MANAGER>
    bool operator!= (Atom<ATOM_MANAGER> lhs, Atom<ATOM_MANAGER> rhs);
    template <typename ATOM_MANAGER>
    bool operator>= (Atom<ATOM_MANAGER> lhs, Atom<ATOM_MANAGER> rhs);
    template <typename ATOM_MANAGER>
    bool operator> (Atom<ATOM_MANAGER> lhs, Atom<ATOM_MANAGER> rhs);
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Atom.inl"

#endif /*_Stroika_Foundation_DataExchange_Atom_h_*/
